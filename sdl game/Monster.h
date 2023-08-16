#pragma once
#include "Entity.h"

class Monster : public Entity {
public:
    Monster(int x, int y, SDL_Color col);
    ~Monster();
    void takeDamage(int amount);
    int getHealth() const { return Entity:: health; }

private:
    int health;
};
