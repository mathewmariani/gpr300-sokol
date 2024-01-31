#define BATTERIES_IMPL
#include "batteries.h"

//
// Assignment2 -- Shadow Mapping
//

#include "fast_obj/fast_obj.h"
#include "glm/gtx/transform.hpp"

#include <string>

// shaders
#include "shaders/shadow_depth.h"
#include "shaders/simple.h"
#include "shaders/debug_quad.h"

typedef struct
{
  glm::mat4 model;
  glm::mat4 view_proj;
} vs_shadow_params_t;

typedef struct
{
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
  glm::mat4 light_view_proj;
} vs_display_params_t;

typedef struct
{
  glm::vec3 light_dir;
  glm::vec3 eye_pos;
} fs_display_params_t;

// application state
static struct
{
  struct
  {
    sg_pass_action pass_action;
    sg_pipeline pip;
    sg_bindings bind;
  } display;

  struct
  {
    sg_pass_action pass_action;
    sg_pass pass;
    sg_pipeline pip;
    sg_bindings bind;
    sg_image map;
    sg_sampler sampler;
  } shadow;

  struct
  {
    sg_pass pass;
    sg_pipeline pip;
    sg_bindings bind;
  } debug;

  uint8_t file_buffer[3 * 1024 * 1024];

  batteries::model_t suzanne;
  sg_buffer plane_vbuf;
  sg_bindings plane_bind;
  float ry;

  glm::vec3 ambient_light;

  struct
  {
    bool suzanne;
    bool failed;
  } loaded;
} state = {
    .ambient_light = glm::vec3(0.25f, 0.45f, 0.65f),
    .ry = 0.0f,
    .loaded = {
        .suzanne = false,
        .failed = false,
    },
};

void create_shadow_pass(void)
{
  // create a texture with only a depth attachment
  state.shadow.map = sg_make_image((sg_image_desc){
      .render_target = true,
      .width = 2048,
      .height = 2048,
      .sample_count = 1,
      .pixel_format = SG_PIXELFORMAT_DEPTH,
      .label = "depth-image",
  });

  // create an image sampler
  state.shadow.sampler = sg_make_sampler((sg_sampler_desc){
      .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
      .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
      .min_filter = SG_FILTER_LINEAR,
      .mag_filter = SG_FILTER_LINEAR,
      .compare = SG_COMPAREFUNC_LESS,
      .label = "shadow-sampler",
  });

  state.shadow.pass = sg_make_pass((sg_pass_desc){
      .depth_stencil_attachment.image = state.shadow.map,
      .label = "shadow-pass",
  });

  auto shadow_shader_desc = (sg_shader_desc){
      .vs = {
          .source = shadow_depth_vs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(vs_shadow_params_t),
              .uniforms = {
                  [0] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                  [1] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
              },
          },
      },
      .fs = {
          .source = shadow_depth_fs,
      },
  };

  state.shadow.pass_action = (sg_pass_action){
      .depth = {
          .load_action = SG_LOADACTION_CLEAR,
          .store_action = SG_STOREACTION_STORE,
          .clear_value = 1.0f,
      },
  };

  state.shadow.pip = sg_make_pipeline((sg_pipeline_desc){
      .layout = {
          // need to provide vertex stride, because normal component is skipped in shadow pass
          .buffers[0].stride = 6 * sizeof(float),
          .attrs = {
              [0].format = SG_VERTEXFORMAT_FLOAT3,
          },
      },
      .shader = sg_make_shader(shadow_shader_desc),
      .index_type = SG_INDEXTYPE_NONE,
      .face_winding = SG_FACEWINDING_CCW,
      .cull_mode = SG_CULLMODE_FRONT,
      .depth = {
          .pixel_format = SG_PIXELFORMAT_DEPTH,
          .compare = SG_COMPAREFUNC_LESS_EQUAL,
          .write_enabled = true,
      },
      .colors[0].pixel_format = SG_PIXELFORMAT_NONE,
      .label = "shadow-pipeline",
  });
}

void create_display_pass(void)
{
  state.display.pass_action = (sg_pass_action){
      .colors[0] = {
          .clear_value = {state.ambient_light.r, state.ambient_light.g, state.ambient_light.b, 1.0f},
          .load_action = SG_LOADACTION_CLEAR,
      },
  };
  auto shader_desc = (sg_shader_desc){
      .vs = {
          .source = simple_vs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(vs_display_params_t),
              .uniforms = {
                  [0] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                  [1] = {.name = "view", .type = SG_UNIFORMTYPE_MAT4},
                  [2] = {.name = "projection", .type = SG_UNIFORMTYPE_MAT4},
                  [3] = {.name = "light_view_proj", .type = SG_UNIFORMTYPE_MAT4},
              },
          },
      },
      .fs = {
          .source = simple_fs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(fs_display_params_t),
              .uniforms = {
                  [0] = {.name = "light_dir", .type = SG_UNIFORMTYPE_FLOAT3},
                  [1] = {.name = "eye_pos", .type = SG_UNIFORMTYPE_FLOAT3},
              },
          },
          .images[0] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_DEPTH},
          .samplers[0] = {.used = true, .sampler_type = SG_SAMPLERTYPE_COMPARISON},
          .image_sampler_pairs[0] = {
              .glsl_name = "shadow_map",
              .image_slot = 0,
              .sampler_slot = 0,
              .used = true,
          },
      },
  };
  state.display.pip = sg_make_pipeline((sg_pipeline_desc){
      .layout = {
          .attrs = {
              [0].format = SG_VERTEXFORMAT_FLOAT3,
              [1].format = SG_VERTEXFORMAT_FLOAT3,
          },
      },
      .shader = sg_make_shader(shader_desc),
      .index_type = SG_INDEXTYPE_NONE,
      .face_winding = SG_FACEWINDING_CCW,
      .cull_mode = SG_CULLMODE_BACK,
      .depth = {
          .compare = SG_COMPAREFUNC_LESS_EQUAL,
          .write_enabled = true,
      },
      .label = "display-pipeline",
  });
  state.display.bind.fs.images[0] = state.shadow.map;
  state.display.bind.fs.samplers[0] = state.shadow.sampler;
}

void create_debug_pass(void)
{
  // a vertex buffer, pipeline and sampler to render a debug visualization of the shadow map
  float dbg_vertices[] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};
  sg_buffer dbg_vbuf = sg_make_buffer((sg_buffer_desc){
      .data = SG_RANGE(dbg_vertices),
      .label = "debug-vertices",
  });
  auto debug_shader_desc = (sg_shader_desc){
      .vs = {
          .source = debug_quad_vs,
      },
      .fs = {
          .source = debug_quad_fs,
          .images[0] = {
              .sample_type = SG_IMAGESAMPLETYPE_UNFILTERABLE_FLOAT,
              .used = true,
          },
          .samplers[0] = {
              .sampler_type = SG_SAMPLERTYPE_NONFILTERING,
              .used = true,
          },
          .image_sampler_pairs[0] = {
              .glsl_name = "debug_image",
              .image_slot = 0,
              .sampler_slot = 0,
              .used = true,
          },
      },
  };
  state.debug.pip = sg_make_pipeline((sg_pipeline_desc){
      .layout = {
          .attrs[0].format = SG_VERTEXFORMAT_FLOAT2,
      },
      .shader = sg_make_shader(debug_shader_desc),
      .primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP,
      .label = "debug-pipeline",
  });
  sg_sampler dbg_smp = sg_make_sampler((sg_sampler_desc){
      .min_filter = SG_FILTER_NEAREST,
      .mag_filter = SG_FILTER_NEAREST,
      .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
      .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
      .label = "debug-sampler",
  });
  state.debug.bind = (sg_bindings){
      .vertex_buffers[0] = dbg_vbuf,
      .fs = {
          .images[0] = state.shadow.map,
          .samplers[0] = dbg_smp,
      },
  };
}

static void suzanne_data_loaded(const sfetch_response_t *response)
{
  if (response->fetched)
  {
    auto *mesh = fast_obj_read((const char *)response->data.ptr, response->data.size);
    if (mesh)
    {
      // https://stackoverflow.com/a/36454139
      // a face has 3 vertices, so multiply by 3.
      state.suzanne.mesh.num_faces = mesh->face_count;
      for (auto i = 0; i < mesh->face_count * 3; ++i)
      {
        auto vertex = mesh->indices[i];
        // vertex
        state.suzanne.mesh.vertices.push_back(*((mesh->positions + vertex.p * 3) + 0));
        state.suzanne.mesh.vertices.push_back(*((mesh->positions + vertex.p * 3) + 1));
        state.suzanne.mesh.vertices.push_back(*((mesh->positions + vertex.p * 3) + 2));
        // normal
        state.suzanne.mesh.vertices.push_back(*((mesh->normals + vertex.n * 3) + 0));
        state.suzanne.mesh.vertices.push_back(*((mesh->normals + vertex.n * 3) + 1));
        state.suzanne.mesh.vertices.push_back(*((mesh->normals + vertex.n * 3) + 2));
      }

      fast_obj_destroy(mesh);

      state.suzanne.mesh.vbuf = sg_make_buffer((sg_buffer_desc){
          .data = {
              .ptr = state.suzanne.mesh.vertices.data(),
              .size = state.suzanne.mesh.vertices.size() * sizeof(float),
          },
          .label = "suzanne-vertices",
      });

      state.display.bind.vertex_buffers[0] = state.suzanne.mesh.vbuf;
      state.shadow.bind.vertex_buffers[0] = state.suzanne.mesh.vbuf;

      state.loaded.suzanne = true;
    }
    else
    {
      state.loaded.failed = true;
    }
  }
  else if (response->failed)
  {
    state.loaded.failed = true;
  }
}

void init(void)
{
  batteries::setup();

  create_shadow_pass();
  create_display_pass();
  create_debug_pass();

  // clang-format off
  const float plane_vertices[] = {
    5.0f,  -2.0f,  5.0f,  0.0f, 1.0f, 0.0f,
    -5.0f,  -2.0f, -5.0f,  0.0f, 1.0f, 0.0f,
    -5.0f,  -2.0f,  5.0f,  0.0f, 1.0f, 0.0f,
    5.0f,  -2.0f, -5.0f,  0.0f, 1.0f, 0.0f,
    -5.0f,  -2.0f, -5.0f,  0.0f, 1.0f, 0.0f,
    5.0f,  -2.0f,  5.0f,  0.0f, 1.0f, 0.0f,
  };
  // clang-format on

  state.plane_vbuf = sg_make_buffer((sg_buffer_desc){
      .data = SG_RANGE(plane_vertices),
      .label = "plane-vertices",
  });

  state.plane_bind = (sg_bindings){
      .vertex_buffers[0] = state.plane_vbuf,
      .fs = {
          .images[0] = state.shadow.map,
          .samplers[0] = state.shadow.sampler,
      },
  };

  // start loading data
  sfetch_send((sfetch_request_t){
      .path = "assets/suzanne.obj",
      .callback = suzanne_data_loaded,
      .buffer = SFETCH_RANGE(state.file_buffer),
  });
}

void draw_ui(void)
{
  ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();
}

void frame(void)
{
  sfetch_dowork();

  const float t = (float)(sapp_frame_duration() * 60.0);
  state.ry += 0.2f * sapp_frame_duration();

  if (!state.loaded.failed && (state.loaded.suzanne))
  {
    const auto width = sapp_width();
    const auto height = sapp_height();

    // rotate suzanne
    // state.suzanne.transform.rotation = glm::rotate(state.suzanne.transform.rotation, (float)sapp_frame_duration(), glm::vec3(0.0, 1.0, 0.0));

    // shadow pass matrices

    const glm::mat4 rym = glm::rotate(state.ry, glm::vec3(0.0f, 1.0f, 0.0f));
    // glm::vec3 light_pos = glm::vec3(0.0, 10.0f, 0.0f);
    glm::vec3 light_pos = rym * glm::vec4(50.0f, 50.0f, -50.0f, 1.0f);

    glm::mat4 light_view = glm::lookAt(light_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    glm::mat4 light_proj = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.0f, 100.0f);
    glm::mat4 light_view_proj = light_proj * light_view;

    // display pass matrices
    glm::vec3 eye = glm::vec3(0.0f, 1.5f, 6.0f);
    glm::mat4 view = glm::lookAt(eye, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 proj = glm::perspective(glm::radians(60.0f), (float)(width / (float)height), 0.01f, 10.0f);
    glm::mat4 view_proj = proj * view;

    // parameters for drawing a shadow
    const vs_shadow_params_t vs_shadow_params = {
        .model = state.suzanne.transform.matrix(),
        .view_proj = light_view_proj,
    };

    const fs_display_params_t fs_display_params = {
        .light_dir = glm::normalize(light_pos),
        .eye_pos = eye,
    };

    // parameters for drawing suzanne
    const vs_display_params_t suzanne_vs_display_params = {
        .model = state.suzanne.transform.matrix(),
        .view = view,
        .proj = proj,
        .light_view_proj = light_view_proj * state.suzanne.transform.matrix(),
    };

    // parameters for drawing the plane
    const vs_display_params_t plane_vs_display_params = {
        .model = glm::mat4(1.0f),
        .view = view,
        .proj = proj,
        .light_view_proj = light_view_proj * glm::mat4(1.0f),
    };

    // render the shadow map
    sg_begin_pass(state.shadow.pass, &state.shadow.pass_action);
    sg_apply_pipeline(state.shadow.pip);
    sg_apply_bindings(&state.shadow.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_shadow_params));
    sg_draw(0, state.suzanne.mesh.num_faces * 3, 1);
    sg_end_pass();

    // render the display
    sg_begin_default_pass(&state.display.pass_action, width, height);
    sg_apply_pipeline(state.display.pip);
    sg_apply_bindings(&state.display.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_display_params));

    // render suzanne
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(suzanne_vs_display_params));
    sg_draw(0, state.suzanne.mesh.num_faces * 3, 1);

    // redner plane
    sg_apply_bindings(&state.plane_bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(plane_vs_display_params));
    sg_draw(0, 6, 1);

    // render debug
    sg_apply_pipeline(state.debug.pip);
    sg_apply_bindings(&state.debug.bind);
    sg_apply_viewport(width - 150, 0, 150, 150, false);
    sg_draw(0, 4, 1);
    sg_end_pass();

    sg_commit();
  }
}

void event(const sapp_event *event)
{
  batteries::event(event);
}

void cleanup(void)
{
  batteries::shutdown();
}

sapp_desc sokol_main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  return (sapp_desc){
      .init_cb = init,
      .frame_cb = frame,
      .event_cb = event,
      .cleanup_cb = cleanup,
      .width = 800,
      .height = 800,
      .window_title = "gpr300-sokol",
      .logger.func = slog_func,
  };
}