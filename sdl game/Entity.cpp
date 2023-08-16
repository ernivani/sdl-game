#include "Entity.h"
#include <iostream>

Entity::Entity(int x, int y, int w, int h, SDL_Color color, int health)
    : rect{ x, y, w, h }, color{ color }, health(health) {}

Entity::~Entity() {}


void Entity::takeDamage(int amount) {
    health -= amount;
    if (health < 0) health = 0;  // Ensure health doesn't go below 0
}

void Entity::Render(SDL_Renderer* renderer, const SDL_Rect& camera) {
    SDL_Rect renderRect = { rect.x - camera.x, rect.y - camera.y, rect.w, rect.h };
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &renderRect);


    // Render Health bar (just for the player for now)
    if (health < 100) {
        SDL_Rect healthBar = { rect.x - camera.x, rect.y - camera.y - 10, rect.w, 5 };
        SDL_Rect currentHealth = { rect.x - camera.x, rect.y - camera.y - 10, rect.w * health / 100, 5 };
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &healthBar);
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &currentHealth);
    }

    if (health <= 0) {
        SDL_Rect deathRect = { rect.x - camera.x, rect.y - camera.y, rect.w, rect.h };
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &deathRect);
    }
}

void Entity::moveTo(int x, int y) {
    destination = { x - rect.w / 2, y - rect.h / 2 };
}

void Entity::updatePosition() {
    // Linear interpolation
    
    if (rect.x == destination.x && rect.y == destination.y) return;
    rect.x = rect.x + lerpSpeed * (destination.x - rect.x);
    rect.y = rect.y + lerpSpeed * (destination.y - rect.y);
}
