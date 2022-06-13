#ifndef OPENGL_H
#define OPENGL_H

#include <GL/glew.h>

#include <map>
#include <string>

using namespace std;

class OpenGL {
    private:
        static map<string, GLuint> programs;
        static map<string, GLuint> rectVaos;

        static GLuint activeProgram;
        static GLuint activeTexture;
        static GLuint activeVao;

    public:
        static GLuint createProgram(char* vertexShaderSource, char* fragmentShaderSource);
        static void useProgram(GLuint id);
        static void bindTexture(GLuint id);
        static void bindVao(GLuint id);

        static GLuint createRectVao(GLfloat x0, GLfloat y0, GLfloat width, GLfloat height, GLfloat z, bool isFlipX = false, bool isFlipY = false);

        static string getRectHash(GLfloat x0, GLfloat y0, GLfloat width, GLfloat height, GLfloat z, bool isFlipX, bool isFlipY);
};

#endif