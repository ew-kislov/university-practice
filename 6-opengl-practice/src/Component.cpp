#pragma once

#include "Component.hpp"

#include "Sprite.cpp"

void Component::setSprite(Sprite* sprite) {
    this->sprite = sprite;
}

Sprite* Component::getSprite() {
    return this->sprite;
}

void Component::enable() {
    this->enabled = true;
}

void Component::disable() {
    this->enabled = false;
}

bool Component::isEnabled() {
    return this->enabled;
}