#pragma once

#include "Animator.hpp"

Animator::Animator(double speed) {
    this->speed;
    this->msPerClip = 1000.0 / speed;

    this->activeAnimation = "";
}

void Animator::changeSpeed(double speed) {
    this->speed;
    this->msPerClip = 1000.0 / speed;
}

void Animator::addAnimation(string name, vector<Texture*> clips) {
    this->animations[name] = clips;
}

void Animator::switchAnimation(string name) {
    if (this->animations[name].empty()) {
        throw runtime_error("Error: no animation with given name");
    }

    this->activeAnimation = name;
    this->activeClip = 0;
    this->msSinceLastClip = 0;
}

string Animator::getActiveAnimation() {
    return this->activeAnimation;
}

void Animator::updateSprite(double deltaTime) {
    if (this->activeAnimation.empty()) {
        throw runtime_error("Error: you didn't set active animation to animator");
    }

    this->msSinceLastClip += deltaTime;
    
    if (this->msSinceLastClip >= this->msPerClip) {
        this->activeClip = (this->activeClip + 1) % (this->animations[this->activeAnimation].size());
        this->getSprite()->setTexture(this->animations[this->activeAnimation][this->activeClip]);
        this->msSinceLastClip = 0;
    }
}

string Animator::getId() {
    return "Animator";
}