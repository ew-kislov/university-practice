#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include <map>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "BaseObject.hpp"
#include "MathUtils.cpp"

using namespace std;

class UiElement: public BaseObject {
    public:
        UiElement(char* fragmentShaderSource, char* vertexShaderSource, glm::vec2 position, int layer);

        void transform();

        virtual void draw() = 0;
        virtual void init() = 0;

        virtual string getClassId() = 0;
};

#endif