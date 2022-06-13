#pragma once

#include "ResourceManager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>

#include "Texture.cpp"
#include "Font.cpp"

map<char*, Texture*> ResourceManager::textures = {};
map<string, Font*> ResourceManager::fonts = {};

void ResourceManager::loadTexture(char* textureSource) {
    int width, height, channels;
    GLuint textureId;

    unsigned char *textureData = stbi_load(textureSource, &width, &height, &channels, 0);

    if (!textureData) {
        throw runtime_error("Error: failed to load texture");
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, channels == 4 ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, channels == 4 ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, channels == 3 ? GL_RGB : GL_RGBA, width, height, 0, channels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(textureData);

    ResourceManager::textures[textureSource] = new Texture(textureId, width, height);
}

void ResourceManager::loadFont(string fontSource, int size) {
    FT_Library ft;

    if (FT_Init_FreeType(&ft)) {
        throw runtime_error("Error: could not init FreeType library");
    }

    FT_Face face;
    if (FT_New_Face(ft, fontSource.c_str(), 0, &face)) {
        throw runtime_error("Error: could not init font from source");
    }

    FT_Set_Pixel_Sizes(face, 0, size);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    map<char, Character*> characters;
  
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            throw runtime_error("Error: could not load character");
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character* character = new Character {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        characters[c] = character;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    ResourceManager::fonts[ResourceManager::getFontHash(fontSource, size)] = new Font(fontSource, size, characters);
}

Texture* ResourceManager::getTexture(char* textureSource) {
    Texture* texture = ResourceManager::textures[textureSource];
    if (!texture) {
        throw runtime_error("Error: no texture with given source");
    }
    return texture;
}

Font* ResourceManager::getFont(string fontSource, int size) {
    Font* font = ResourceManager::fonts[ResourceManager::getFontHash(fontSource, size)];
    if (!font) {
        throw runtime_error("Error: no font with given source and size");
    }
    return font;
}

string ResourceManager::getFontHash(string source, int size) {
    return source + to_string(size);
}