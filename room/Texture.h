#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cstddef>
#include <glad/glad.h>
#include "shaderClass.h"

class Texture {
public:
    GLuint ID;
    std::string type;
    GLuint unit;
    int width, height, nrChannels;
    std::shared_ptr<GLuint> handle;

    Texture(const std::string& path, GLenum);
    Texture(const std::string& path, const std::string& type, GLuint slot, bool flip);
    Texture(const unsigned char* bytes, size_t len, const std::string& type, GLuint slot, bool flip);

    void load(const std::string& path, bool flip);
    void loadFromMemory(const unsigned char* bytes, size_t len, bool flip);

    void bind(unsigned int u) const;
    void Bind() const;
    void texUnit(Shader& shader, const char* uniform, GLuint u);
};