#pragma once

#include "SpriteOrderer.hpp"

bool SpriteOrderer::operator()(const Sprite* lhs, const Sprite* rhs) const {
    return lhs->getLayer() < rhs->getLayer();
}