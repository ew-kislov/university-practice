#pragma once

#include <algorithm>

#include "Scene.hpp"

#include "Sprite.cpp"
#include "SpriteOrderer.cpp"
#include "Window.cpp"

Scene::Scene(bool cacheable) {
    this->cacheable = cacheable;
}

void Scene::load() {
    for (Sprite* sprite: this->sprites) {
        sprite->init();
        sprite->onStart();
    }

    for (UiElement* uiElement: this->ui) {
        uiElement->init();
    }

    this->loaded = true;
}

void Scene::unload() {
    // TODO: detach objects from opengl
    // for (auto sprite: this->sprites) {
    //     delete sprite;
    // }

    // for (auto uiElement: this->ui) {
    //     delete uiElement;
    // }

    // this->sprites = {};
    // this->ui = {};

    this->loaded = false;
}

multiset<Sprite*, SpriteOrderer> Scene::getSprites(string typeId) {
    if (typeId.empty()) {
        return this->sprites;
    }

    multiset<Sprite*, SpriteOrderer> filteredSprites;

    copy_if(
        this->sprites.begin(),
        this->sprites.end(),
        inserter(filteredSprites, filteredSprites.end()),
        [typeId](Sprite* sprite){ return sprite->getClassId() == typeId; }
    );

    return filteredSprites;
}

set<Label*> Scene::getUi() {
    return this->ui;
}

BaseObject* Scene::find(string objectId) {
    BaseObject* object = this->namedObjects[objectId];
    if (object == nullptr) {
        throw runtime_error("Object with given id wasn't found");
    }
    return object;
}

void Scene::addSprite(Sprite* sprite) {
    this->sprites.insert(sprite);
    sprite->scene = this;

    if (!sprite->getObjectId().empty()) {
        this->namedObjects[sprite->getObjectId()] = sprite;
    }
}

void Scene::addUiElement(Label* label) {
    this->ui.insert(label);
    label->scene = this;

    if (!label->getObjectId().empty()) {
        this->namedObjects[label->getObjectId()] = label;
    }
}

void Scene::addNamedObject(BaseObject* object) {
    this->namedObjects[object->getObjectId()] = object;
}

void Scene::onKeyPress(EKey key, function<void(void)> handler) {
    this->keyHandlers[key] = pair<EKeyEvent, function<void(void)> >(EKeyEvent::PRESS, handler);
}

void Scene::onKeyDown(EKey key, function<void(void)> handler) {
    this->keyHandlers[key] = pair<EKeyEvent, function<void(void)> >(EKeyEvent::DOWN, handler);
}

void Scene::onKeyUp(EKey key, function<void(void)> handler) {
    this->keyHandlers[key] = pair<EKeyEvent, function<void(void)> >(EKeyEvent::UP, handler);
}

void Scene::update() {
    map<EKey, pair<EKeyEvent, function<void(void)> > >::iterator it;

    for (it = this->keyHandlers.begin(); it != this->keyHandlers.end(); it++) {
        pair<EKeyEvent, function<void(void)> > handlerData = it->second;
        bool isHandlerTriggered;

        switch (handlerData.first) {
            case EKeyEvent::DOWN:
                isHandlerTriggered = Window::isKeyDown(it->first);
                break;
            case EKeyEvent::PRESS:
                isHandlerTriggered = Window::isKeyPressed(it->first);
                break;
            case EKeyEvent::UP:
                isHandlerTriggered = Window::isKeyUp(it->first);
                break;
            default:
                throw runtime_error("Error: nsupported key event.");
                exit(1);
        }

        if (isHandlerTriggered) {
            handlerData.second();
        }
    }
}

bool Scene::isCacheable() {
    return this->cacheable;
}

bool Scene::isLoaded() {
    return this->loaded;
}