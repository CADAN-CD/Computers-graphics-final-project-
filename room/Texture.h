#pragma once
#include <glad/glad.h>
#include <string>
#include <memory>
#include "shaderClass.h"

class Texture {
public:
    GLuint ID = 0;
    std::string type;          
    GLuint unit = 0;
    int width = 0, height = 0, nrChannels = 0;

    // cube construct 
    Texture(const std::string& path, GLenum format = GL_RGBA);

	//construct for Mesh
    Texture(const std::string& path, const std::string& type, GLuint slot, bool flip = false);

    void bind(unsigned int unit) const;                       // cube
    void Bind() const;                                        // Mesh
    void texUnit(Shader& shader, const char* uniform, GLuint unit);

private:
    std::shared_ptr<GLuint> handle;   
    void load(const std::string& path, bool flip);
};