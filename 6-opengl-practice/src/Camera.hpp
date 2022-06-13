#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

#include "Sprite.hpp"

#include "Game.hpp"

class Camera {
    private:
        static int width;
        static int height;

        static glm::vec2 position;
        static glm::mat4 projectionMatrix;
        static glm::mat4 lookAtMatrix;

        static Sprite* target;

        friend class Game;

    public:
        static void move();
        static void setResolution(int width, int height);
        static void lookAt(Sprite* target);
        static glm::vec2 getPosition();
        static glm::mat4 getResultMatrix();
        static glm::mat4 getProjectionMatrix();
        static glm::mat4 getLookAtMatrix();
};

#endif