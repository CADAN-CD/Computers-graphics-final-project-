#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "shaderClass.h"

class Skybox
{
public:
	Skybox(const std::vector<std::string>& faces);
	~Skybox();
	void Draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
	void drawSkybox(Skybox& skybox, Shader& skyboxShader, float FOVdeg, float nearPlane, float farPlane);
private:
	GLuint vao = 0, vbo = 0, texID = 0;
};
