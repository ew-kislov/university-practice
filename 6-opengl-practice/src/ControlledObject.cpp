#pragma once

#include "ControlledObject.hpp"

#include "Window.cpp"
#include "Camera.cpp"
#include "Animator.cpp"
#include "ResourceManager.cpp"

ControlledObject::ControlledObject(Texture* texture, glm::vec2 position, int layer, GLfloat speed, double scale):
    Sprite(texture, position, layer, scale) {
    this->speed = speed;
}

ControlledObject::ControlledObject(Texture* texture, glm::vec2 position, int layer, GLfloat speed, double width, double height, double scale):
    Sprite(texture, position, layer, width, height, scale) {
    this->speed = speed;
}

void ControlledObject::onStart() {
    Animator* animator = new Animator(20.0f);

    vector<Texture*> walkAnimation = {
        ResourceManager::getTexture("assets/hero/walk_000.png"),
        ResourceManager::getTexture("assets/hero/walk_001.png"),
        ResourceManager::getTexture("assets/hero/walk_002.png"),
        ResourceManager::getTexture("assets/hero/walk_003.png"),
        ResourceManager::getTexture("assets/hero/walk_004.png"),
        ResourceManager::getTexture("assets/hero/walk_005.png"),
        ResourceManager::getTexture("assets/hero/walk_006.png"),
        ResourceManager::getTexture("assets/hero/walk_007.png"),
        ResourceManager::getTexture("assets/hero/walk_008.png"),
        ResourceManager::getTexture("assets/hero/walk_009.png"),
        ResourceManager::getTexture("assets/hero/walk_010.png"),
        ResourceManager::getTexture("assets/hero/walk_011.png"),
        ResourceManager::getTexture("assets/hero/walk_012.png"),
        ResourceManager::getTexture("assets/hero/walk_013.png"),
        ResourceManager::getTexture("assets/hero/walk_014.png"),
        ResourceManager::getTexture("assets/hero/walk_015.png"),
        ResourceManager::getTexture("assets/hero/walk_016.png"),
        ResourceManager::getTexture("assets/hero/walk_017.png"),
        ResourceManager::getTexture("assets/hero/walk_018.png"),
        ResourceManager::getTexture("assets/hero/walk_019.png"),
    };

    vector<Texture*> idleAnimation = {
        ResourceManager::getTexture("assets/hero/idle_000.png"),
        ResourceManager::getTexture("assets/hero/idle_001.png"),
        ResourceManager::getTexture("assets/hero/idle_002.png"),
        ResourceManager::getTexture("assets/hero/idle_003.png"),
        ResourceManager::getTexture("assets/hero/idle_004.png"),
        ResourceManager::getTexture("assets/hero/idle_005.png"),
        ResourceManager::getTexture("assets/hero/idle_006.png"),
        ResourceManager::getTexture("assets/hero/idle_007.png"),
        ResourceManager::getTexture("assets/hero/idle_008.png"),
        ResourceManager::getTexture("assets/hero/idle_009.png"),
        ResourceManager::getTexture("assets/hero/idle_010.png"),
        ResourceManager::getTexture("assets/hero/idle_011.png"),
        ResourceManager::getTexture("assets/hero/idle_012.png"),
        ResourceManager::getTexture("assets/hero/idle_013.png"),
        ResourceManager::getTexture("assets/hero/idle_014.png"),
        ResourceManager::getTexture("assets/hero/idle_015.png"),
        ResourceManager::getTexture("assets/hero/idle_016.png"),
        ResourceManager::getTexture("assets/hero/idle_017.png"),
        ResourceManager::getTexture("assets/hero/idle_018.png"),
        ResourceManager::getTexture("assets/hero/idle_019.png"),
    };

    animator->addAnimation("Idle", idleAnimation);
    animator->addAnimation("Walk", walkAnimation);
    animator->switchAnimation("Idle");
    animator->enable();

    this->addComponent(animator);
}

void ControlledObject::onUpdate(double deltaTime) {
    glm::vec2 xMovementVector = glm::vec2(
        Window::isKeyPressed(EKey::A) ? -1.0 * deltaTime / 20 * this->speed : Window::isKeyPressed(EKey::D) ? 1.0 * deltaTime / 20 * this->speed : 0.0,
        0.0
    );

    glm::vec2 yMovementVector = glm::vec2(
        0.0,
        Window::isKeyPressed(EKey::W) ? -1.0 * deltaTime / 20 * this->speed : Window::isKeyPressed(EKey::S) ? 1.0 * deltaTime / 20 * this->speed : 0.0
    );

    if (xMovementVector.x > 0) {
        this->isFlipX = false;
    } else if (xMovementVector.x < 0) {
        this->isFlipX = true;
    }

    bool isMoving = xMovementVector.x || yMovementVector.y;

    bool canMoveX = true;
    bool canMoveY = true;

    MathUtils::Rect* newRectX = this->getBoundingRect();
    MathUtils::Rect* newRectY = this->getBoundingRect();

    newRectX->x0 += xMovementVector.x;
    newRectX->x1 += xMovementVector.x;
    newRectY->y0 += yMovementVector.y;
    newRectY->y1 += yMovementVector.y;

    auto tiles = this->getScene()->getSprites("Tile");

    for (Sprite* baseTile: tiles) {
        Tile* tile = (Tile*)baseTile;
        if (!tile->isWalkable() && MathUtils::areRectsIntercepting(newRectX, tile->getBoundingRect())) {
            canMoveX = false;
        }
        if (!tile->isWalkable() && MathUtils::areRectsIntercepting(newRectY, tile->getBoundingRect())) {
            canMoveY = false;
        }
        if (!canMoveX && !canMoveY) {
            break;
        }
    }

    if (canMoveX) {
        this->position += xMovementVector;
    }
    if (canMoveY) {
        this->position += yMovementVector;
    }

    Animator* animator = ((Animator*)(this->getComponent("Animator")));
    if (isMoving && animator->getActiveAnimation() != "Walk") {
        animator->switchAnimation("Walk");
    } else if (!isMoving && animator->getActiveAnimation() != "Idle") {
        animator->switchAnimation("Idle");
    }
}

string ControlledObject::getClassId() {
    return "ControlledObject";
}