#ifndef LABEL_H
#define LABEL_H

#include <map>
#include <string>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "UiElement.hpp"
#include "MathUtils.cpp"
#include "Font.hpp"

using namespace std;

class Label: public UiElement {
    private:
        string text;
        Font* font;
        glm::vec4 color;

    public:
        Label(string text, Font* font, glm::vec4 color, glm::vec2 position, int layer);

        void init();
        void draw();

        string getClassId();
};

#endif