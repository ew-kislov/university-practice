#pragma once

#include "UiElement.hpp"

UiElement::UiElement(char* fragmentShaderSource, char* vertexShaderSource, glm::vec2 position, int layer):
    BaseObject(fragmentShaderSource, vertexShaderSource, position, layer) {
}

void UiElement::transform() {
    OpenGL::useProgram(this->programId);
    glm::mat4 cameraMatrix = Camera::getProjectionMatrix();
    OpenglUtils::setUniformMat4(
        this->programId,
        "transform_matrix",
        cameraMatrix * glm::translate(glm::vec3(this->position, 0.0f))
    );
}