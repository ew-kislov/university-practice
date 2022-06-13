#ifndef FONT_H
#define FONT_H

#include <string>
#include <map>

#include <glm/glm.hpp>

using namespace std;

struct Character {
    unsigned int textureId;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
};

class Font {
    private:
        string source;
        int size;
        map<char, Character*> characters;

    public:
        Font(string source, int size, map<char, Character*> characters);

        Character* getCharacter(char c);
        int getSize();
};

#endif