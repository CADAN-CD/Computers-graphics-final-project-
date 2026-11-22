#define _CRT_SECURE_NO_WARNINGS
#define CGLTF_IMPLEMENTATION      
#include <cgltf.h>

#include "Model.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

// color of the material
static glm::vec3 materialColor(const cgltf_material* mat)
{
	glm::vec3 color(0.8f);   
	if (!mat) return color;

	if (mat->has_pbr_metallic_roughness) {
		const float* f = mat->pbr_metallic_roughness.base_color_factor;
		color = glm::vec3(f[0], f[1], f[2]);
	}
	// models with specular-glossiness extension
	if (mat->has_pbr_specular_glossiness) {
		const float* d = mat->pbr_specular_glossiness.diffuse_factor;
		color = glm::vec3(d[0], d[1], d[2]);
	}
	return color;
}

// converts a glft primitive into a Mesh
static void loadPrimitive(const cgltf_primitive& prim, const glm::mat4& world,
	std::vector<Mesh>& meshes, std::vector<glm::mat4>& matrices)
{
	if (prim.type != cgltf_primitive_type_triangles) return;

	// Buscar los atributos que nos interesan
	const cgltf_accessor* posAcc = nullptr;
	const cgltf_accessor* norAcc = nullptr;
	for (cgltf_size a = 0; a < prim.attributes_count; a++) {
		const cgltf_attribute& attr = prim.attributes[a];
		if (attr.type == cgltf_attribute_type_position)    posAcc = attr.data;
		else if (attr.type == cgltf_attribute_type_normal) norAcc = attr.data;
	}
	if (!posAcc) return;

	glm::vec3 color = materialColor(prim.material);

	// Vértices: cgltf_accessor_read_float respeta stride y convierte el tipo solo
	std::vector<Vertex> vertices(posAcc->count);
	for (cgltf_size i = 0; i < posAcc->count; i++) {
		float p[3] = { 0, 0, 0 };
		float n[3] = { 0, 1, 0 };
		cgltf_accessor_read_float(posAcc, i, p, 3);
		if (norAcc) cgltf_accessor_read_float(norAcc, i, n, 3);

		vertices[i].position = glm::vec3(p[0], p[1], p[2]);
		vertices[i].normal = glm::vec3(n[0], n[1], n[2]);
		vertices[i].color = color;
		vertices[i].texUV = glm::vec2(0.0f);
	}

	// Índices: cgltf_accessor_read_index los entrega siempre como número normal,
	// sin importar si vienen en 8, 16 o 32 bits
	std::vector<GLuint> indices;
	if (prim.indices) {
		indices.resize(prim.indices->count);
		for (cgltf_size i = 0; i < prim.indices->count; i++)
			indices[i] = (GLuint)cgltf_accessor_read_index(prim.indices, i);
	}
	else {
		indices.resize(posAcc->count);
		for (cgltf_size i = 0; i < posAcc->count; i++) indices[i] = (GLuint)i;
	}

	std::vector<Texture> textures;   // vacío: no hay texturas
	meshes.emplace_back(vertices, indices, textures);
	matrices.push_back(world);
}

Model::Model(const std::string& path)
{
	cgltf_options options = {};
	cgltf_data* data = nullptr;

	// 1) reads the file 
	if (cgltf_parse_file(&options, path.c_str(), &data) != cgltf_result_success) {
		std::cerr << "No se pudo leer el modelo: " << path << std::endl;
		return;
	}
	// 2) loads the scene data
	if (cgltf_load_buffers(&options, data, path.c_str()) != cgltf_result_success) {
		std::cerr << "No se pudo cargar los buffers mensota" << std::endl;
		cgltf_free(data);
		return;
	}

	// 3) Travels through nodes and meshes, creating a Mesh for each primitive
	for (cgltf_size n = 0; n < data->nodes_count; n++) {
		const cgltf_node* node = &data->nodes[n];
		if (!node->mesh) continue;

		float m[16];
		cgltf_node_transform_world(node, m);   // ya combina padre -> hijo
		glm::mat4 world = glm::make_mat4(m);

		for (cgltf_size p = 0; p < node->mesh->primitives_count; p++)
			loadPrimitive(node->mesh->primitives[p], world, meshes, matrices);
	}

	cgltf_free(data);
	std::cout << "Modelo cargado: " << meshes.size() << " mallas" << std::endl;
}

void Model::Draw(Shader& shader, Camera& camera,
	glm::vec3 translation, glm::quat rotation, glm::vec3 scale)
{
	for (size_t i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader, camera, matrices[i], translation, rotation, scale);
}