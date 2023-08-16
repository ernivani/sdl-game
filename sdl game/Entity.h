#pragma once
#include <SDL.h>

class Entity {
public:
    Entity(int x, int y, int w, int h, SDL_Color color, int health = 100);
    virtual ~Entity();
    virtual void Render(SDL_Renderer* renderer, const SDL_Rect& camera);
    virtual void takeDamage(int amount);
    void respawn(int x, int y) {
        rect.x = x;
        rect.y = y;
        health = 100;
    }
    bool isAlive() { return health > 0; }

    int getX() const { return rect.x; }
    int getY() const { return rect.y; }
    void setX(int x) { rect.x = x; }
    void setY(int y) { rect.y = y; }

    void moveTo(int x, int y) {
        rect.x = x - rect.w / 2;
        rect.y = y - rect.h / 2;
    }

    const SDL_Rect& getRect() const { return rect; }

protected:
    SDL_Rect rect;
    SDL_Color color;
    int health; 
};