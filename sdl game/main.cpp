#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include "Monster.h"
#include <iostream>

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("Mini Jeu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 24);
    if (!font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
    }
    std::cout << TTF_GetError() << std::endl;
    SDL_Color textColor = { 0, 0, 0, 255 };

    const int MAP_WIDTH = 3000;
    const int MAP_HEIGHT = 2000;
    Entity character(MAP_WIDTH / 2, MAP_HEIGHT / 2, 50, 50, { 255, 0, 0, 255 });
    character.moveTo(MAP_WIDTH / 2, MAP_HEIGHT / 2);
    SDL_Rect camera = { character.getX() - 400, character.getY() - 300, 800, 600 };

    std::vector<Monster> allyCreeps;
    std::vector<Monster> enemyCreeps;
    int moveTimer = 0;
    const int MOVE_RATE = 10;
    int score = 0;
    int highScore = 0;
    int timeElapsed = 0;
    const int SCORE_RATE = 20000;

    bool running = true;
    SDL_Event event;
    int monsterSpawnTimer = 0;
    const int MONSTER_SPAWN_RATE = 300;
    int deathTimer = 0;
    const int RESPAWN_TIME = 5000;


    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_RIGHT && character.isAlive()) {
                    character.moveTo(event.button.x + camera.x, event.button.y + camera.y);
                }
            }
        }
        character.updatePosition(); // add this just before rendering the character

        if (!character.isAlive()) {
            deathTimer += MOVE_RATE;
            if (deathTimer >= RESPAWN_TIME) {
                character.respawn(MAP_WIDTH / 2, MAP_HEIGHT / 2);
                character.moveTo(MAP_WIDTH / 2, MAP_HEIGHT / 2);
                deathTimer = 0;
            }
            if (score > highScore) {
                highScore = score;
            }
            score = 0;
        }
        else {
            deathTimer = 0;
        }

        if (camera.x < 0) { camera.x = 0; }
        if (camera.y < 0) { camera.y = 0; }
        if (camera.x > MAP_WIDTH - camera.w) { camera.x = MAP_WIDTH - camera.w; }
        if (camera.y > MAP_HEIGHT - camera.h) { camera.y = MAP_HEIGHT - camera.h; }



        for (auto& monster : enemyCreeps) {
            if (character.isAlive() && SDL_HasIntersection(&character.getRect(), &monster.getRect())) {
                character.takeDamage(1);
                monster.takeDamage(1);
            }
        }

        for (auto& monster : allyCreeps) {
            if (character.isAlive() && SDL_HasIntersection(&character.getRect(), &monster.getRect())) {
                character.takeDamage(1);
                monster.takeDamage(1);
            }
        }

        if (monsterSpawnTimer++ >= MONSTER_SPAWN_RATE) {
            Monster newAllyCreep(MAP_WIDTH * 0.25, rand() % MAP_HEIGHT, { 128, 0, 128, 255 });
            Monster newEnemyCreep(MAP_WIDTH * 0.75, rand() % MAP_HEIGHT, { 0, 128, 128, 255 });
            allyCreeps.push_back(newAllyCreep);
            enemyCreeps.push_back(newEnemyCreep);
            monsterSpawnTimer = 0;
        }

        if (moveTimer++ >= MOVE_RATE) {
            for (auto& creep : allyCreeps) {
                creep.setX(creep.getX() + 1);
            }
            for (auto& creep : enemyCreeps) {
                creep.setX(creep.getX() - 1);
            }
            moveTimer = 0;
        }

        for (auto& ally : allyCreeps) {
            for (auto& enemy : enemyCreeps) {
                if (SDL_HasIntersection(&ally.getRect(), &enemy.getRect())) {
                    ally.takeDamage(1);
                    enemy.takeDamage(1);
                }
            }
        }

        allyCreeps.erase(
            std::remove_if(allyCreeps.begin(), allyCreeps.end(),
                [](const Monster& m) { return m.getHealth() <= 0; }),
            allyCreeps.end()
        );

        enemyCreeps.erase(
            std::remove_if(enemyCreeps.begin(), enemyCreeps.end(),
                [](const Monster& m) { return m.getHealth() <= 0; }),
            enemyCreeps.end()
        );

        timeElapsed += MOVE_RATE;
        if (timeElapsed >= SCORE_RATE) {
            score++;
            timeElapsed = 0;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);


        SDL_Rect mapRect = { 0 - camera.x, 0 - camera.y, MAP_WIDTH, MAP_HEIGHT };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &mapRect);
        character.Render(renderer, camera);

        for (auto& creep : allyCreeps) {
            creep.Render(renderer, camera);
        }
        for (auto& creep : enemyCreeps) {
            creep.Render(renderer, camera);
        }
        TTF_SetFontStyle(font, TTF_STYLE_BOLD);

        std::string scoreText = "Score: " + std::to_string(score);
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        SDL_Rect textRect;
        textRect.x = 10;
        textRect.y = 10;
        textRect.w = textSurface->w;
        textRect.h = textSurface->h;

        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        std::string highScoreText = "High Score: " + std::to_string(highScore);
        SDL_Surface* highScoreSurface = TTF_RenderText_Solid(font, highScoreText.c_str(), textColor);
        SDL_Texture* highScoreTexture = SDL_CreateTextureFromSurface(renderer, highScoreSurface);

        SDL_Rect highScoreRect;
        highScoreRect.x = 10;
        highScoreRect.y = textRect.y + textRect.h + 5; // positionne le high score juste en dessous du score actuel
        highScoreRect.w = highScoreSurface->w;
        highScoreRect.h = highScoreSurface->h;

        SDL_RenderCopy(renderer, highScoreTexture, NULL, &highScoreRect);

        SDL_FreeSurface(highScoreSurface);
        SDL_DestroyTexture(highScoreTexture);
        TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
