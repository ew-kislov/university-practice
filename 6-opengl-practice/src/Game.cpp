#pragma once

#include "Game.hpp"

#include <iostream>
#include <fstream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

#include "Sprite.cpp"
#include "SpriteOrderer.cpp"
#include "ControlledObject.cpp"
#include "Tile.cpp"
#include "UiElement.cpp"
#include "Camera.cpp"
#include "Window.cpp"
#include "Scene.cpp"

std::map<string, Scene*> Game::scenes = {};
Scene* Game::activeScene = nullptr;
bool Game::shouldChangeScene = NULL;
string Game::nextScene = string();

int Game::framesPerSec = 0;
double Game::lastFpsUpdateTime = 0;
double Game::lastTime = 0;
double Game::deltaTime = 0;

Game::Game() {
}

void Game::init() {
    Camera::setResolution(Window::getWidth(), Window::getHeight());
}

void Game::runMainLoop() {
    do {
        Game::calculateFps();

        double currentTime = Window::getTime();
        Game::deltaTime = 1000 * (currentTime - Game::lastTime);
        Game::lastTime = currentTime;

        Window::clear();

        if (!Game::activeScene || Game::shouldChangeScene) {
            Game::switchScene();
        }

        activeScene->update();

        Camera::move();

        updatePositions();
        draw();

        Window::update();

    } while (Window::isOpened());
}

void Game::draw() {
    auto sprites = activeScene->getSprites();

    for (Sprite* sprite : sprites) {
        sprite->draw();
    }

    set<Label*> ui = activeScene->getUi();

    for (Label* label : ui) {
        label->draw();
    }
}

void Game::updatePositions() {
    auto sprites = activeScene->getSprites();

    for (Sprite* sprite : sprites) {
        sprite->onUpdate(Game::deltaTime);
        sprite->updateComponents(Game::deltaTime);
    }

    set<Label*> ui = activeScene->getUi();

    for (Label* label : ui) {
        label->transform();
    }
}

void Game::addScene(string name, Scene* scene) {
    scenes[name] = scene;
}

void Game::switchScene() {
    Scene* scene = scenes[Game::nextScene];
    if (scene == nullptr) {
        throw runtime_error("Error: scene with given name doesn't exist");
    }

    Scene* prevScene = Game::activeScene;

    if (prevScene != nullptr && !prevScene->isCacheable() && prevScene->isLoaded()) {
        prevScene->unload();
    }
    
    if (!scene->isLoaded()) {
        scene->load();
    }

    Game::activeScene = scene;
    Game::shouldChangeScene = false;
}

void Game::setActiveScene(string name) {
    Game::nextScene = name;
    Game::shouldChangeScene = true;
}

void Game::calculateFps() {
    double currentTime = Window::getTime();
    Game::framesPerSec++;
    if (currentTime - Game::lastFpsUpdateTime >= 1.0 ){
        cout << "FPS: " << Game::framesPerSec << endl;
        Game::framesPerSec = 0;
        Game::lastFpsUpdateTime += 1.0;
    }
}