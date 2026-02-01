#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <queue>
#include <stdio.h>

#include "../../globals/global.hpp"
#include "../../entities/enemy.hpp"


class AirAttack1
{
private:
    SDL_Texture* texture;
    SDL_Rect rect;
    float speedf;

    std::pair<float, float> dirf;
public:
    AirAttack1(SDL_Rect spawnRect, SDL_Rect DirectionRect);

    ~AirAttack1();

    bool IsAlive() const;

    void Update(double deltaTime);

    void Render();
};