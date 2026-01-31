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

class EarthAttack2
{
private:
    SDL_Texture* texture;
    SDL_Rect rect;

    float BASE_DURATION;
    float currentTime;

    int tileX0, tileX1, tileY0, tileY1;
    
public:
    EarthAttack2(SDL_Rect spawnRect);

    ~EarthAttack2();

    bool IsAlive() const;

    void Update(double deltaTime);

    void Render();
};