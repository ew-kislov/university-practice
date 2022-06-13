#pragma once

#include "Window.hpp"

#include <stdio.h>
#include <stdlib.h>

int Window::width = 0;
int Window::height = 0;
string Window::name = "";
GLFWwindow* Window::window = nullptr;
map<int, bool> Window::keyState = {};

void Window::setName(string pName) {
    name = pName;
}

void Window::setResolution(int pWidth, int pHeight) {
    width = pWidth;
    height = pHeight;
}

int Window::getWidth() {
    return width;
}

int Window::getHeight() {
    return height;
}

void Window::open() {
    if (!glfwInit()) {
        fprintf(stderr, "Error: failed to initialize GLFW\n");
        exit(1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);

    if (window == NULL) {
        fprintf(stderr, "Error: failed tp create GLFW window.\n");
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Error: failed to initialize GLEW\n");
        glfwTerminate();
        exit(1);
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glfwSwapInterval(0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool Window::isOpened() {
    return glfwWindowShouldClose(window) == 0;
}

void Window::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::update() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool Window::isKeyPressed(EKey key) {
    bool result = glfwGetKey(window, key) == GLFW_PRESS;
    Window::keyState[key] = result;
    return result;
}

bool Window::isKeyDown(EKey key) {
    bool result = glfwGetKey(window, key) == GLFW_PRESS;
    bool downResult = result && !Window::keyState[key];
    Window::keyState[key] = result;
    return downResult;
}

bool Window::isKeyUp(EKey key) {
    bool result = glfwGetKey(window, key) == GLFW_RELEASE;
    Window::keyState[key] = !result;
    return result;
}

double Window::getTime() {
    return glfwGetTime();
}