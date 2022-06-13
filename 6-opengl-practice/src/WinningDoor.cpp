#pragma once

#include "WinningDoor.hpp"

#include "Sprite.cpp"
#include "Game.cpp"

WinningDoor::WinningDoor(Texture* texture, glm::vec2 position, int layer):
    Sprite(texture, position, layer) {
}

void WinningDoor::onStart() {
    // Do nothing
}

void WinningDoor::onUpdate(double deltaTime) {
    Sprite* mainCharacter = (Sprite*)(this->getScene()->find("Main character"));
    if (MathUtils::areRectsIntercepting(this->getBoundingRect(), mainCharacter->getBoundingRect())) {
        Game::setActiveScene("Win");
    }
}

string WinningDoor::getClassId() {
    return "WinningDoor";
}