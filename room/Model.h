#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Mesh.h"

class Model
{
public:
	Model(const std::string& path);

	void Draw(Shader& shader, Camera& camera,
		glm::vec3 translation = glm::vec3(0.0f),
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f));

private:
	std::vector<Mesh> meshes;
	std::vector<glm::mat4> matrices;   
};
