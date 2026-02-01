#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <queue>
#include <iostream>

#include "../globals/global.hpp"

class Enemy
{
private:
    SDL_Texture* texture;
    SDL_Rect destRect;

    float posX, posY;

    bool damagePlayer;
    bool inPlayerRange;

    float damageTime;
    const float DAMAGE_TIME_TOTAL = 0.2f;

    const float speed = 100.0f;

    std::vector<SDL_Texture*> sprites;

    int health;

    std::vector<std::pair<int, int>> Directions(std::pair<int, int> cpos);

    bool ValidPosition(std::pair<int, int> p);

public:
    Enemy(std::pair<int, int> spawnPos);

    const SDL_Rect* EnemyRect();

    bool IsNearPlayer();

    std::pair<float, float> BacktrackPath(std::vector<std::vector<std::pair<int, int>>>& parent, std::pair<int, int> start, std::pair<int, int> goal);

    std::pair<float, float> BFS();

    void TakeDamage(int amount);

    bool IsAlive() const;
    bool InPlayerRange() const;

    void Update(double deltaTime);

    void Render();
};