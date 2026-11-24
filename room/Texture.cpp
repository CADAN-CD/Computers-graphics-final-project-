#include "Texture.h"
#include "stb_image.h"
#include <iostream>

Texture::Texture(const std::string& path, GLenum /*unused*/)
    : type("diffuse"), unit(0) {
    load(path, true);
}

Texture::Texture(const std::string& path, const std::string& type, GLuint slot, bool flip)
    : type(type), unit(slot) {
    load(path, flip);
}


void Texture::load(const std::string& path, bool flip) {
    GLuint id;
    glGenTextures(1, &id);
    handle = std::shared_ptr<GLuint>(new GLuint(id), [](GLuint* p) {
        glDeleteTextures(1, p);
        delete p;
        });
    ID = id;

    glBindTexture(GL_TEXTURE_2D, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(flip);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        GLenum fmt = GL_RGB;
        if (nrChannels == 1) fmt = GL_RED;
        else if (nrChannels == 2) fmt = GL_RG;
        else if (nrChannels == 4) fmt = GL_RGBA;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else {
        std::cerr << "Me quedo grande sacar la textura: " << path
            << " | " << stbi_failure_reason() << std::endl;
    }
}

void Texture::bind(unsigned int u) const {
    glActiveTexture(GL_TEXTURE0 + u);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Bind() const {
    bind(unit);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint u) {
    unit = u;
    shader.Activate();
    glUniform1i(glGetUniformLocation(shader.ID, uniform), unit);
}

Texture::Texture(const unsigned char* bytes, size_t len, const std::string& type, GLuint slot, bool flip)
    : type(type), unit(slot) {
    loadFromMemory(bytes, len, flip);
}

void Texture::loadFromMemory(const unsigned char* bytes, size_t len, bool flip) {
    GLuint id;
    glGenTextures(1, &id);
    handle = std::shared_ptr<GLuint>(new GLuint(id), [](GLuint* p) {
        glDeleteTextures(1, p);
        delete p;
        });
    ID = id;

    glBindTexture(GL_TEXTURE_2D, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(flip);
    unsigned char* pixels = stbi_load_from_memory(bytes, (int)len, &width, &height, &nrChannels, 0);

    if (pixels) {
        GLenum fmt = GL_RGB;
        if (nrChannels == 1) fmt = GL_RED;
        else if (nrChannels == 2) fmt = GL_RG;
        else if (nrChannels == 4) fmt = GL_RGBA;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(pixels);
    }
    else {
        std::cerr << "lol no pudiste cargar una textura: " << stbi_failure_reason() << std::endl;
    }
}