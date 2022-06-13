#ifndef CONTROLLED_OBJECT_H
#define CONTROLLED_OBJECT_H

#include "Sprite.hpp"

#include <iostream>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "Tile.hpp"

using namespace std;

class ControlledObject : public Sprite {
    private:
        GLfloat speed;

    public:
        ControlledObject(Texture* texture, glm::vec2 position, int layer, GLfloat speed, double scale = 1.0f);
        ControlledObject(Texture* texture, glm::vec2 position, int layer, GLfloat speed, double width, double height, double scale = 1.0f);

        void onStart();
        void onUpdate(double deltaTime);

        string getClassId();
};

#endif