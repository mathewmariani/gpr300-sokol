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

struct fetch_wrapper {
	void *ptr;
};

}

namespace ew
{

Model::Model(const std::string& path) {
	fetch_wrapper w = {
		.ptr = this,
	};

	sfetch_send((sfetch_request_t){
		.path = path.c_str(),
		.callback = fetchCallback,
		.buffer = {
			.ptr =  malloc(1024 * 1024 * 10),
			.size = 1024 * 1024 * 10,
		},
		.user_data = SFETCH_RANGE(w),
	});
}

void Model::draw()
{
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].draw();
	}
}

void Model::createModel(const std::vector<ew::MeshData>& data)
{
	for (const auto& meshdata : data)
	{
		m_meshes.push_back({meshdata});
	}
}

void Model::fetchCallback(const sfetch_response_t* response)
{
	if (!response->fetched)
	{
		printf("[!!!] Failed to load model (%s)\n", response->path);
		return;
	}

	auto* self = (Model*)(static_cast<fetch_wrapper*>(response->user_data))->ptr;
	printf("Scene.model at: %p\n", (void*)self);

	auto obj = fast_obj_read((const char *)response->data.ptr, response->data.size);

	std::vector<MeshData> data;

	for (auto i = 0; i < obj->group_count; ++i)
	{
		ew::MeshData mesh;
		auto group = obj->groups[i];
		for (auto j = 0; j < group.face_count * 3; ++j)
		{
			auto indices = obj->indices[j];
			mesh.vertices.push_back((ew::Vertex) {
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
			mesh.indices.push_back(j);
		}

		data.push_back({mesh});
	}

	self->createModel(data);

	fast_obj_destroy(obj);
}

}
