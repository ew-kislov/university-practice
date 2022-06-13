#pragma once

#include "Texture.hpp"

Texture::Texture(GLuint id, int width, int height) {
    this->id = id;
    this->width = width;
    this->height = height;
}

GLuint Texture::getId() {
    return this->id;
}

int Texture::getWidth() {
    return this->width;
}

int Texture::getHeight() {
    return this->height;
}