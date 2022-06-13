#ifndef SPRITE_H
#define SPRITE_H

#include <map>
#include <type_traits>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "BaseObject.hpp"
#include "MathUtils.cpp"
#include "Scene.hpp"
#include "Texture.hpp"
#include "Component.hpp"

using namespace std;


class Sprite: public BaseObject {
    private:
        double textureWidth;
        double textureHeight;
        double boxWidth;
        double boxHeight;
        double scale;

        Texture* texture;
        map<string, Component*> components;

        bool isOutOfWindow();

    protected:
        bool isFlipX = false;
        bool isFlipY = false;

        void addComponent(Component* component);
        Component* getComponent(string id);

    public:
        Sprite(Texture* texture, glm::vec2 position, int layer, double scale = 1.0f);
        Sprite(Texture* texture, glm::vec2 position, int layer, double width, double height, double scale = 1.0f);

        int getWidth();
        int getHeight();

        void init();
        void draw();

        MathUtils::Rect* getBoundingRect();

        void setTexture(Texture* texture);

        void updateComponents(double deltaTime);

        // implement this in final objects

        virtual void onStart() = 0;
        virtual void onUpdate(double deltaTime) = 0;

        virtual string getClassId() = 0;
};

#endif