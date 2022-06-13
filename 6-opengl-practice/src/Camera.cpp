#pragma once

#include "Camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

#include "Sprite.cpp"

int Camera::width = 0;
int Camera::height = 0;

glm::mat4 Camera::projectionMatrix = glm::mat4(1.0);
glm::mat4 Camera::lookAtMatrix = glm::mat4(1.0);
glm::vec2 Camera::position = glm::vec2(0.0, 0.0);

Sprite* Camera::target = nullptr;

void Camera::setResolution(int pWidth, int pHeight) {
    width = pWidth;
    height = pHeight;

    projectionMatrix =
        glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1, 0, 0)) *
        glm::translate(glm::vec3(-1.0, -1.0, 0.0)) *
        glm::scale(glm::vec3(2.0 / width, 2.0 / height, 1.f));
}

void Camera::lookAt(Sprite* pTarget) {
    target = pTarget;
}

glm::mat4 Camera::getResultMatrix() {
    return projectionMatrix * lookAtMatrix;
}

glm::mat4 Camera::getProjectionMatrix() {
    return projectionMatrix;
}

glm::mat4 Camera::getLookAtMatrix() {
    return lookAtMatrix;
}

glm::vec2 Camera::getPosition() {
    return position;
}

void Camera::move() {
    if (target) {
        position = glm::vec2(
            -target->getPosition().x + width / 2,
            -target->getPosition().y + height / 2
        );
        lookAtMatrix = glm::translate(
            glm::mat4(1.0),
            glm::vec3(
                position.x,
                position.y,
                0.0f
            )
        );
    }
}