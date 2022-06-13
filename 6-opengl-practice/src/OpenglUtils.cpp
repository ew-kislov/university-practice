#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

namespace OpenglUtils {
    GLuint createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
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
        const char* vertexSourcePointer = vertexShaderCode.c_str();
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
        const char* fragmentSourcePointer = fragmentShaderCode.c_str();
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

        return programId;
    }

    void setUniformMat4(GLuint programId, char* name, glm::mat4 matrix) {
        GLuint MatrixId = glGetUniformLocation(programId, name);
        glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void setUniformMat4(GLuint programId, char* name, GLfloat* matrix) {
        GLuint MatrixId = glGetUniformLocation(programId, name);
        glUniformMatrix4fv(MatrixId, 1, GL_FALSE, matrix);
    }
}