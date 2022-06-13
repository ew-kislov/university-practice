#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Game.cpp"
#include "Scene.cpp"
#include "Sprite.cpp"
#include "ControlledObject.cpp"
#include "WinningDoor.cpp"
#include "LoosingDoor.cpp"
#include "Window.cpp"
#include "Camera.cpp"
#include "Label.cpp"
#include "ResourceManager.cpp"
#include "Font.cpp"

#include "MazeLoader.cpp"

using namespace std;

int main(void) {
    // init window

    Window::setName("2D Game");
    Window::setResolution(1024, 768);
    Window::open();

    // prepare resources

    ResourceManager::loadTexture("assets/textures/floor/center_001.png");
    ResourceManager::loadTexture("assets/textures/walls/wall_001.png");

    ResourceManager::loadTexture("assets/items/empty_door.png");
    ResourceManager::loadTexture("assets/items/door.png");

    ResourceManager::loadTexture("assets/hero/walk_000.png");
    ResourceManager::loadTexture("assets/hero/walk_001.png");
    ResourceManager::loadTexture("assets/hero/walk_002.png");
    ResourceManager::loadTexture("assets/hero/walk_003.png");
    ResourceManager::loadTexture("assets/hero/walk_004.png");
    ResourceManager::loadTexture("assets/hero/walk_005.png");
    ResourceManager::loadTexture("assets/hero/walk_006.png");
    ResourceManager::loadTexture("assets/hero/walk_007.png");
    ResourceManager::loadTexture("assets/hero/walk_008.png");
    ResourceManager::loadTexture("assets/hero/walk_009.png");
    ResourceManager::loadTexture("assets/hero/walk_010.png");
    ResourceManager::loadTexture("assets/hero/walk_011.png");
    ResourceManager::loadTexture("assets/hero/walk_012.png");
    ResourceManager::loadTexture("assets/hero/walk_013.png");
    ResourceManager::loadTexture("assets/hero/walk_014.png");
    ResourceManager::loadTexture("assets/hero/walk_015.png");
    ResourceManager::loadTexture("assets/hero/walk_016.png");
    ResourceManager::loadTexture("assets/hero/walk_017.png");
    ResourceManager::loadTexture("assets/hero/walk_018.png");
    ResourceManager::loadTexture("assets/hero/walk_019.png");

    ResourceManager::loadTexture("assets/hero/idle_000.png");
    ResourceManager::loadTexture("assets/hero/idle_001.png");
    ResourceManager::loadTexture("assets/hero/idle_002.png");
    ResourceManager::loadTexture("assets/hero/idle_003.png");
    ResourceManager::loadTexture("assets/hero/idle_004.png");
    ResourceManager::loadTexture("assets/hero/idle_005.png");
    ResourceManager::loadTexture("assets/hero/idle_006.png");
    ResourceManager::loadTexture("assets/hero/idle_007.png");
    ResourceManager::loadTexture("assets/hero/idle_008.png");
    ResourceManager::loadTexture("assets/hero/idle_009.png");
    ResourceManager::loadTexture("assets/hero/idle_010.png");
    ResourceManager::loadTexture("assets/hero/idle_011.png");
    ResourceManager::loadTexture("assets/hero/idle_012.png");
    ResourceManager::loadTexture("assets/hero/idle_013.png");
    ResourceManager::loadTexture("assets/hero/idle_014.png");
    ResourceManager::loadTexture("assets/hero/idle_015.png");
    ResourceManager::loadTexture("assets/hero/idle_016.png");
    ResourceManager::loadTexture("assets/hero/idle_017.png");
    ResourceManager::loadTexture("assets/hero/idle_018.png");
    ResourceManager::loadTexture("assets/hero/idle_019.png");

    ResourceManager::loadFont("assets/fonts/arial.ttf", 32);
    ResourceManager::loadFont("assets/fonts/arial.ttf", 64);

    // add intro scene

    Scene* introScene = new Scene(true);

    Label* introLabel = new Label("Wake the **** up, samurai.", ResourceManager::getFont("assets/fonts/arial.ttf", 64), glm::vec4(1.f, 0.f, 1.f, 1.f), glm::vec2(100.f, 100.f), 3);
    Label* actionLabel = new Label("Press enter to start the game.", ResourceManager::getFont("assets/fonts/arial.ttf", 32), glm::vec4(1.f, 0.f, 1.f, 1.f), glm::vec2(100.f, 180.f), 3);

    introScene->addUiElement(introLabel);
    introScene->addUiElement(actionLabel);

    introScene->onKeyDown(EKey::Enter, []() { Game::setActiveScene("Level"); });

    // add pause scene

    Scene* pauseScene = new Scene(true);

    Label* pauseLabel = new Label("Press P to resume the game.", ResourceManager::getFont("assets/fonts/arial.ttf", 32), glm::vec4(1.f, 0.f, 1.f, 1.f), glm::vec2(100.f, 100.f), 3);

    pauseScene->addUiElement(pauseLabel);

    pauseScene->onKeyDown(EKey::P, []() { Game::setActiveScene("Level"); });

    // add game over scene

    Scene* gameOverScene = new Scene(true);

    Label* gameOverLabel = new Label("You`r lost. Press enter for main menu", ResourceManager::getFont("assets/fonts/arial.ttf", 32), glm::vec4(1.f, 0.f, 1.f, 1.f), glm::vec2(100.f, 100.f), 3);

    gameOverScene->addUiElement(gameOverLabel);

    gameOverScene->onKeyDown(EKey::Enter, []() { Game::setActiveScene("Intro"); });

    // add game over scene

    Scene* winScene = new Scene(true);

    Label* winLabel = new Label("You won. Press enter for main menu", ResourceManager::getFont("assets/fonts/arial.ttf", 32), glm::vec4(1.f, 0.f, 1.f, 1.f), glm::vec2(100.f, 100.f), 3);

    winScene->addUiElement(winLabel);

    winScene->onKeyDown(EKey::Enter, []() { Game::setActiveScene("Intro"); });

    // add level scene

    Scene* levelScene = new Scene(false);

    levelScene->onKeyDown(EKey::P, []() { Game::setActiveScene("Pause"); });

    // add map to scene

    vector<vector<char>> mazeMap = MazeLoader::getMazeTilemap();

    for (uint8_t i = 0; i < mazeMap.size(); i++) {
        for (uint8_t j = 0; j < mazeMap[i].size(); j++) {
            Sprite* tile;

            switch (mazeMap[i][j]) {
                case '.':
                    tile = new Tile(
                        ResourceManager::getTexture("assets/textures/floor/center_001.png"),
                        glm::vec2(j * 72.f, i * 72.f),
                        0,
                        true
                    );
                    break;
                case '#':
                    tile = new Tile(
                        ResourceManager::getTexture("assets/textures/walls/wall_001.png"),
                        glm::vec2(j * 72.f, i * 72.f),
                        0,
                        false
                    );
                    break;
                case 'L':
                    levelScene->addSprite(new Tile(
                        ResourceManager::getTexture("assets/textures/floor/center_001.png"),
                        glm::vec2(j * 72.f, i * 72.f),
                        0,
                        true
                    ));

                    tile = new LoosingDoor(
                        ResourceManager::getTexture("assets/items/empty_door.png"),
                        glm::vec2(j * 72.f, i * 72.f - 42),
                        1
                    );
                    break;
                case 'W':
                    levelScene->addSprite(new Tile(
                        ResourceManager::getTexture("assets/textures/floor/center_001.png"),
                        glm::vec2(j * 72.f, i * 72.f),
                        0,
                        true
                    ));

                    tile = new WinningDoor(
                        ResourceManager::getTexture("assets/items/door.png"),
                        glm::vec2(j * 72.f, i * 72.f - 42),
                        1
                    );
                    break;
                case '@':
                    levelScene->addSprite(new Tile(
                        ResourceManager::getTexture("assets/textures/floor/center_001.png"),
                        glm::vec2(j * 72.f, i * 72.f),
                        0,
                        true
                    ));
                    tile = new ControlledObject(
                        ResourceManager::getTexture("assets/hero/idle_000.png"),
                        glm::vec2(j * 72.f, i * 72.f),
                        2,
                        3.0f,
                        0.22
                    );
                    tile->setObjectId("Main character");
                    Camera::lookAt(tile);
                    break;
                default:
                    throw runtime_error("Error: unknown tile character");
            }

            levelScene->addSprite(tile);
        }
    }


    // init game

    Game::addScene("Level", levelScene);
    Game::addScene("Intro", introScene);
    Game::addScene("Pause", pauseScene);
    Game::addScene("Game Over", gameOverScene);
    Game::addScene("Win", winScene);

    Game::setActiveScene("Intro");

    Game::init();
    Game::runMainLoop();

    return 0;
}
