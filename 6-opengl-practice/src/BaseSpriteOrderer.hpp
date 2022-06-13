#ifndef BASE_SPRITE_ORDERER_H
#define BASE_SPRITE_ORDERER_H

#include "Sprite.hpp"

class Sprite; // TODO: delete

struct BaseSpriteOrderer {
    virtual bool operator()(const Sprite* lhs, const Sprite* rhs) const = 0;
};

#endif