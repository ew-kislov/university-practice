#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <set>
#include <map>
#include <functional>

#include "SpriteOrderer.hpp"
#include "BaseObject.hpp"
#include "Sprite.hpp"
#include "Window.hpp"
#include "Label.hpp"
#include "Game.hpp"

using namespace std;

class Sprite;

class Scene {
    private:
        multiset<Sprite*, SpriteOrderer> sprites;
        map<string, BaseObject*> namedObjects;
        set<Label*> ui;

        map<EKey, pair<EKeyEvent, function<void(void)> > > keyHandlers;

        bool cacheable;
        bool loaded = false;

    public:
        Scene(bool cacheable);

        void load();
        void unload();

        multiset<Sprite*, SpriteOrderer> getSprites(string typeId = string());
        set<Label*> getUi();
        BaseObject* find(string objectId);

        void addSprite(Sprite* sprite);
        void addNamedObject(BaseObject* object);
        void addUiElement(Label* label);

        void onKeyPress(EKey key, function<void(void)> handler);
        void onKeyDown(EKey key, function<void(void)> handler);
        void onKeyUp(EKey key, function<void(void)> handler);
        
        void update();

        bool isCacheable();
        bool isLoaded();
};

#endif