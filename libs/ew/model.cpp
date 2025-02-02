/*
*	Author: Eric Winebrenner
*/

#include "model.h"

// fastobj
#include "fast_obj/fast_obj.h"

// glm
#include "glm/vec3.hpp"

// sokol-fetch
#include "sokol/sokol_fetch.h"

namespace
{
	static uint8_t file_buffer[1024 * 1024 * 10];

	typedef void (*obj_request_callback_t)(ew::Model*, fastObjMesh *);

	struct obj_request_t
	{
		const char *path;
		ew::Model *model;
		obj_request_callback_t callback;
	};

	struct obj_request_data_t
	{
		fastObjMesh *mesh;
		ew::Model *model;
		obj_request_callback_t callback;
	};

	static void obj_fetch_callback(const sfetch_response_t *response)
	{
		if (response->fetched)
		{
			obj_request_data_t request = *(obj_request_data_t *)response->user_data;
			request.mesh = fast_obj_read((const char *)response->data.ptr, response->data.size);

			if (request.mesh->mtllib_count == 0)
			{
				request.callback(request.model, request.mesh);
				return;
			}
		}
		else if (response->failed)
		{
			printf("[!!!] Failed to load obj file.\n");
		}
	}

	static void load_obj(const obj_request_t &request)
	{
		obj_request_data_t wrapper = {
			.mesh = nullptr,
			.model = request.model,
			.callback = request.callback,
		};

		sfetch_send({
			.path = request.path,
			.callback = obj_fetch_callback,
			.buffer = SFETCH_RANGE(file_buffer),
			.user_data = SFETCH_RANGE(wrapper),
		});
	}

	static void fetch_callback(ew::Model *model, fastObjMesh *obj)
	{
		for (auto i = 0; i < obj->group_count; ++i)
		{
			ew::MeshData data;
			auto group = obj->groups[i];
			for (auto j = 0; j < group.face_count * 3; ++j)
			{
				auto indices = obj->indices[j];
				data.vertices.push_back((ew::Vertex) {
					.pos{
						*((obj->positions + indices.p * 3) + 0),
						*((obj->positions + indices.p * 3) + 1),
						*((obj->positions + indices.p * 3) + 2),
					},
					.normal = {
						*((obj->normals + indices.n * 3) + 0),
						*((obj->normals + indices.n * 3) + 1),
						*((obj->normals + indices.n * 3) + 2),
					},
					.uv = {
						*((obj->texcoords + indices.t * 2) + 0),
						*((obj->texcoords + indices.t * 2) + 1),
					},
				});
				data.indices.push_back(j);
			}

			model->m_meshes[i].load(data);
		}
	}
}

namespace ew
{
	std::unique_ptr<Model> Model::Load(const std::string &path)
	{
    	auto ptr = std::make_unique<Model>();
		load_obj((obj_request_t) {
			.path = path.c_str(),
			.callback = fetch_callback,
			.model = ptr.get(),
		});

		return ptr;
	}

	void Model::draw()
	{
		for (size_t i = 0; i < m_meshes.size(); i++)
		{
			m_meshes[i].draw();
		}
	}
}
