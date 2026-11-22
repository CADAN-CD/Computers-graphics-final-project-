#pragma once
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "shaderClass.h"
#include <glm/glm.hpp>

class LightSource
{
public:
    LightSource(const std::string& texturePath);
    ~LightSource();

    void Draw(Shader& shader, glm::mat4 model);

private:
    VAO vao;
    VBO* vbo;
    EBO* ebo;
    Texture texture;
};