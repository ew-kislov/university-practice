#pragma once

#include "LoosingDoor.hpp"

#include "Sprite.cpp"
#include "Game.cpp"

LoosingDoor::LoosingDoor(Texture* texture, glm::vec2 position, int layer):
    Sprite(texture, position, layer) {
}

void LoosingDoor::onStart() {
    // Do nothing
}

void LoosingDoor::onUpdate(double deltaTime) {
    Sprite* mainCharacter = (Sprite*)(this->getScene()->find("Main character"));
    if (MathUtils::areRectsIntercepting(this->getBoundingRect(), mainCharacter->getBoundingRect())) {
        Game::setActiveScene("Game Over");
    }
}

string LoosingDoor::getClassId() {
    return "LoosingDoor";
}