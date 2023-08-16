#include "Monster.h"

Monster::Monster(int x, int y, SDL_Color col) : Entity(x, y, 50, 50, col) {
    Entity::health = 100;
}

Monster::~Monster() {}

void Monster::takeDamage(int amount) {
    Entity::takeDamage(amount);
    if (Entity::health <= 0) {
        color = { 100, 100, 100, 255 };
    }
}
