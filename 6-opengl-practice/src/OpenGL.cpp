#pragma once

#include "OpenGL.hpp"

map<string, GLuint> OpenGL::programs = {};
map<string, GLuint> OpenGL::rectVaos = {};
GLuint OpenGL::activeProgram = -1;
GLuint OpenGL::activeTexture = -1;
GLuint OpenGL::activeVao = -1;

GLuint OpenGL::createProgram(char *vertexShaderSource, char *fragmentShaderSource) {
    string shadersString = string(vertexShaderSource) + string(fragmentShaderSource);
    GLuint cachedProgramId = OpenGL::programs[shadersString];
    if (cachedProgramId) {
        return cachedProgramId;
    }

    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    string vertexShaderCode;
    ifstream vertexShaderStream(vertexShaderSource, ios::in);

    if (vertexShaderStream.is_open()) {
        stringstream sstr;
        sstr << vertexShaderStream.rdbuf();
        vertexShaderCode = sstr.str();
        vertexShaderStream.close();
    } else {
        fprintf(stderr, "Error: can't open %s.", vertexShaderSource);
        exit(1);
    }

    string fragmentShaderCode;
    ifstream fragmentShaderStream(fragmentShaderSource, ios::in);

    if (fragmentShaderStream.is_open()) {
        stringstream sstr;
        sstr << fragmentShaderStream.rdbuf();
        fragmentShaderCode = sstr.str();
        fragmentShaderStream.close();
    } else {
        fprintf(stderr, "Error: can't open %s.", vertexShaderSource);
        exit(1);
    }

    GLint result = GL_FALSE;
    int infoLogLength;

    // printf("Compiling shader: %s\n", vertexShaderSource);
    const char *vertexSourcePointer = vertexShaderCode.c_str();
    glShaderSource(vertexShaderId, 1, &vertexSourcePointer, NULL);
    glCompileShader(vertexShaderId);

    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        vector<char> vertexShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(vertexShaderId, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
        printf("%s\n", &vertexShaderErrorMessage[0]);
    }

    // printf("Compiling shader: %s\n", fragmentShaderSource);
    const char *fragmentSourcePointer = fragmentShaderCode.c_str();
    glShaderSource(fragmentShaderId, 1, &fragmentSourcePointer, NULL);
    glCompileShader(fragmentShaderId);

    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(fragmentShaderId, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
        printf("%s\n", &fragmentShaderErrorMessage[0]);
    }

    // printf("Linking program\n");
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);

    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        vector<char> ProgramErrorMessage(infoLogLength + 1);
        glGetProgramInfoLog(programId, infoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(programId, vertexShaderId);
    glDetachShader(programId, fragmentShaderId);

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    OpenGL::programs[shadersString] = programId;

    return programId;
}

void OpenGL::useProgram(GLuint id) {
    if (id != OpenGL::activeProgram) {
        glUseProgram(id);
        OpenGL::activeProgram = id;
    }
}

void OpenGL::bindTexture(GLuint id) {
    if (id != OpenGL::activeTexture) {
        glBindTexture(GL_TEXTURE_2D, id);
        OpenGL::activeTexture = id;
    }
}

void OpenGL::bindVao(GLuint id) {
    if (id != OpenGL::activeVao) {
        glBindVertexArray(id);
        OpenGL::activeVao = id;
    }
}

GLuint OpenGL::createRectVao(GLfloat x0, GLfloat y0, GLfloat width, GLfloat height, GLfloat z, bool isFlipX, bool isFlipY) {
    GLuint vao = OpenGL::rectVaos[OpenGL::getRectHash(x0, y0, width, height, z, isFlipX, isFlipY)];
    if (vao) {
        return vao;
    }

    GLuint positionVbo;
    GLuint textureCoordVbo;

    glGenVertexArrays(1, &vao);
    OpenGL::bindVao(vao);

    /**
     * initiate vertex data
     */

    GLfloat* positionData = new GLfloat[18] {
        1.0f * x0, 1.0f * y0, z,
        1.0f * x0, 1.0f * (y0 + height), z,
        1.0f * (x0 + width), y0, z,

        1.0f * x0, 1.0f * (y0 + height), z,
        1.0f * (x0 + width), y0, z,
        1.0f * (x0 + width), 1.0f * (y0 + height), z
    };

    glGenBuffers(1, &positionVbo);
    glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 18, positionData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    /**
     * initiate texture coord data
     */

    GLfloat* textureCoords = new GLfloat[12] {
        isFlipX ? 1.0f : 0.0f, 0.0f,
        isFlipX ? 1.0f : 0.0f, 1.0f,
        isFlipX ? 0.0f : 1.0f, 0.0f,

        isFlipX ? 1.0f : 0.0f, 1.0f,
        isFlipX ? 0.0f : 1.0f, 0.0f,
        isFlipX ? 0.0f : 1.0f, 1.0f
    };

    glGenBuffers(1, &textureCoordVbo);
    glBindBuffer(GL_ARRAY_BUFFER, textureCoordVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, textureCoords, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

    OpenGL::rectVaos[OpenGL::getRectHash(x0, y0, width, height, z, isFlipX, isFlipY)] = vao;

    return vao;
}

string OpenGL::getRectHash(GLfloat x0, GLfloat y0, GLfloat width, GLfloat height, GLfloat z, bool isFlipX, bool isFlipY) {
    return to_string(x0) + '/' + to_string(y0) + '/' + to_string(width) + '/' + to_string(height) + '/' + to_string(z) + '/' + to_string(isFlipX) + '/' + to_string(isFlipY);
}