#pragma once

#include "Label.hpp"

#include "Camera.cpp"
#include "UiElement.cpp"
#include "Font.cpp"

Label::Label(string text, Font* font, glm::vec4 color, glm::vec2 position, int layer):
    UiElement("src/label_fragment_shader.glsl", "src/vertex_shader.glsl", position, layer) {
    this->text = text;
    this->font = font;
    this->color = color;
}

void Label::init() {
    OpenGL::useProgram(this->programId);

    // init vao

    glGenVertexArrays(1, &this->vao);
    OpenGL::bindVao(this->vao);

    // init texture

    GLuint textureCoordVbo;

    GLfloat textureCoords[12] {
        0.0f, 1.0f,            
        0.0f, 0.0f,
        1.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f     
    };

    glGenBuffers(1, &textureCoordVbo);
    glBindBuffer(GL_ARRAY_BUFFER, textureCoordVbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12 * text.size(), NULL, GL_STATIC_DRAW);

    for (int i = 0; i < this->text.size(); i++) {
        glBufferSubData(GL_ARRAY_BUFFER, i * 12 * sizeof(GLfloat), sizeof(GLfloat) * 12, &textureCoords);
    }

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

    this->vbos[VboType::TextureCoords] = textureCoordVbo;

    // init vertices

    GLfloat z = 1.0 * this->layer / MAX_LAYERS;

    GLuint positionVbo;

    glGenBuffers(1, &positionVbo);
    glBindBuffer(GL_ARRAY_BUFFER, positionVbo);

    this->vbos[VboType::Position] = positionVbo;

    float x = this->position.x;
    float y = this->position.y;
    
    GLfloat* buffer = new GLfloat[text.size() * 18];

    for (int i = 0; i < text.size(); i++) {
        Character* ch = this->font->getCharacter(text[i]);

        float xpos = x + ch->bearing.x;
        float ypos = y + this->font->getSize() - ch->bearing.y;

        float w = ch->size.x;
        float h = ch->size.y;

        GLfloat* vertices = new GLfloat[18] {
             xpos,     ypos + h, z,          
             xpos,     ypos,     z,
             xpos + w, ypos,     z,

             xpos,     ypos + h, z,
             xpos + w, ypos,     z,
             xpos + w, ypos + h, z    
        };

        memcpy(buffer + i * 18, vertices, 18 * sizeof(GLfloat));

        delete[] vertices;

        x += (ch->advance >> 6);
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 18 * text.size(), buffer, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    delete[] buffer;

    // init color 

    glUniform3f(glGetUniformLocation(programId, "text_color"), color.x, color.y, color.z);
}

void Label::draw() {
    OpenGL::useProgram(this->programId);
    OpenGL::bindVao(this->vao);

    for (int i = 0; i < text.size(); i++) {
        Character* ch = font->getCharacter(text[i]);

        OpenGL::bindTexture(ch->textureId);
        glDrawArrays(GL_TRIANGLES, i * 6, 6);
    }
}

string Label::getClassId() {
    return "Label";
}