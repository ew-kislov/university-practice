#ifndef TILE_H
#define TILE_H

#include "Sprite.hpp"

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace std;

class Tile : public Sprite {
    private:
        bool walkable;

    public:
        Tile(Texture* texture, glm::vec2 position, int layer, bool walkable);

        void onStart();
        void onUpdate(double deltaTime);
        
        bool isWalkable();

        string getClassId();
};

#endif