#pragma once

#include "BaseObject.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

#include "OpenglUtils.cpp"
#include "MathUtils.cpp"
#include "OpenGL.cpp"

BaseObject::BaseObject(char* fragmentShaderSource, char* vertexShaderSource, glm::vec2 position, int layer) {
    this->scene = nullptr;

    this->initialPosition = position;
    this->position = position;
    this->layer = layer;

    this->programId = OpenGL::createProgram(vertexShaderSource, fragmentShaderSource);
    OpenGL::useProgram(this->programId);
}

int BaseObject::getLayer() const {
    return this->layer;
}

Scene* BaseObject::getScene() {
    return this->scene;
}

glm::vec2 BaseObject::getPosition() {
    return this->position;
}

void BaseObject::setObjectId(string objectId) {
    this->objectId = objectId;
    
    if (this->getScene() != nullptr) {
        this->getScene()->addNamedObject(this);
    }
}

string BaseObject::getObjectId() {
    return this->objectId;
}

BaseObject::~BaseObject() {
}