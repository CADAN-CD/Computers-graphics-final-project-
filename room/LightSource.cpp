#include "LightSource.h"
#include <glm/gtc/type_ptr.hpp>
LightSource::LightSource(const std::string& texturePath) : texture(texturePath)
{
	GLfloat lightVertices[] =
	{ //     COORDINATES     //
		-0.1f, -0.1f,  0.1f,
		-0.1f, -0.1f, -0.1f,
		 0.1f, -0.1f, -0.1f,
		 0.1f, -0.1f,  0.1f,
		-0.1f,  0.1f,  0.1f,
		-0.1f,  0.1f, -0.1f,
		 0.1f,  0.1f, -0.1f,
		 0.1f,  0.1f,  0.1f
	};
	GLuint lightIndices[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 4, 7,
		0, 7, 3,
		3, 7, 6,
		3, 6, 2,
		2, 6, 5,
		2, 5, 1,
		1, 5, 4,
		1, 4, 0,
		4, 5, 6,
		4, 6, 7
	};

	vao.Bind();
	vbo = new VBO(lightVertices, sizeof(lightVertices));
	ebo = new EBO(lightIndices, sizeof(lightIndices));

	vao.LinkVBO(*vbo, 0);
	vao.LinkVBO(*vbo, 1);
	ebo->Bind();

	vao.Unbind();
	vbo->Unbind();
	ebo->Unbind();
}
LightSource::~LightSource()
{
	delete vbo;
	delete ebo;
}

void LightSource::Draw(Shader& shader, glm::mat4 model)
{
	shader.Activate();
	texture.bind(0);
	GLuint modelLoc = glGetUniformLocation(shader.ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	vao.Bind();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}