#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

using namespace std;

enum EKey {
    W = GLFW_KEY_W,
    A = GLFW_KEY_A,
    S = GLFW_KEY_S,
    D = GLFW_KEY_D,
    P = GLFW_KEY_P,
    Enter = GLFW_KEY_ENTER,
    Escape = GLFW_KEY_ESCAPE
};

enum EKeyEvent {
    PRESS = 1,
    DOWN = 2,
    UP = 3
};

class Window {
    private:
        static GLFWwindow* window;

        static int width;
        static int height;

        static string name;

        static map<int, bool> keyState;

    public:
        static void setName(string pName);
        static void setResolution(int pWidth, int pHeight);

        static int getWidth();
        static int getHeight();

        static void open();

        static bool isOpened();
        static void clear();
        static void update();

        static bool isKeyPressed(EKey key);
        static bool isKeyDown(EKey key);
        static bool isKeyUp(EKey key);

        static double getTime();
};

#endif