#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>

#include "Sprite.hpp"

using namespace std;

class Sprite;

class Component {
    private:
       Sprite* sprite;

       bool enabled = true;

    protected:
        Sprite* getSprite();

    public:
        void setSprite(Sprite* sprite);

        void enable();
        void disable();
        bool isEnabled();

        virtual void updateSprite(double deltaTime) = 0;
        virtual string getId() = 0;
};

#endif