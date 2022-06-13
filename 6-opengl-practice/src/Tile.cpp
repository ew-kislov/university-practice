#pragma once

#include "Tile.hpp"

Tile::Tile(Texture* texture, glm::vec2 position, int layer, bool walkable):
    Sprite(texture, position, layer) {
    this->walkable = walkable;
}

void Tile::onStart() {
    // Do nothing
}

void Tile::onUpdate(double deltaTime) {
    // Do nothing
}

bool Tile::isWalkable() {
    return this->walkable;
}

string Tile::getClassId() {
    return "Tile";
}