#ifndef SPRITE_ORDERER_H
#define SPRITE_ORDERER_H

#include "BaseSpriteOrderer.hpp"

struct SpriteOrderer: BaseSpriteOrderer {
    virtual bool operator()(const Sprite* lhs, const Sprite* rhs) const;
};

#endif