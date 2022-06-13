#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

class Texture {
    private:
        GLuint id;
        int width;
        int height;

    public:
        Texture(GLuint id, int width, int height);

        GLuint getId();
        int getWidth();
        int getHeight();
};

#endif