#ifndef BASE_OBJECT_H
#define BASE_OBJECT_H

#include <map>
#include <string>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "MathUtils.cpp"
#include "Scene.hpp"

using namespace std;

class Scene;

enum VboType {
    Position = 1,
    TextureCoords = 2
};

class BaseObject {
    private:
        string objectId;

        Scene* scene;

        friend class Scene;

    protected:
        glm::vec2 position;
        glm::vec2 initialPosition;

        int width;
        int height;
        
        int layer;

        const static int MAX_LAYERS = 100;

        // TODO: move program to shader
        // TODO: move vao, vbo to resource manager

        GLuint programId;
        GLuint vao;
        map<VboType, GLuint> vbos;

        Scene* getScene();

    public:
        BaseObject(char* fragmentShaderSource, char* vertexShaderSource, glm::vec2 position, int layer);

        int getLayer() const;

        virtual void init() = 0;
        virtual void draw() = 0;

        glm::vec2 getPosition();

        void setObjectId(string objectId);
        string getObjectId();

        virtual ~BaseObject();
};

#endif