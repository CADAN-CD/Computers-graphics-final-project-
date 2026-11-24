#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Mesh.h"
#include "Json/json.h"
#include <cstring>                      
#include <iostream>
#include <stdexcept>
#include <glm/gtc/matrix_transform.hpp>  
#include <glm/gtc/type_ptr.hpp>

using json = nlohmann::json;

class Model
{
public:
	Model(const char* file);

	void Draw(Shader& shader, Camera& camera, glm::mat4 matrix = glm::mat4(1.0f));
private:
	std::string file;
	std::vector<unsigned char> data;
	json JSON;

	std::vector<unsigned char> getData();
	std::vector<float> getFloats(json accesor);
	std::vector<GLuint> getIndices(json accesor);

	std::vector<std::string> loadedTexName;
	std::vector<Texture> loadedTex;

	std::vector<Mesh> meshes;
	std::vector<glm::vec3> translationsMeshes;
	std::vector<glm::quat> rotationsMeshes;
	std::vector<glm::vec3> scalesMeshes;
	std::vector<glm::mat4> matricesMeshes;

	std::vector<Vertex> assembleVertices(std::vector<glm::vec3> positions,
		std::vector<glm::vec3> normals,
		std::vector<glm::vec2> texUVs);

	std::vector<glm::vec2> groupFloatsVec2(std::vector<float> floatVec);
	std::vector<glm::vec3> groupFloatsVec3(std::vector<float> floatVec);

	std::vector<Texture> getTextures(unsigned int indMesh);
	void loadMesh(unsigned int indMesh);
	void traverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f));
	void loadGLB(const std::string& content);
};