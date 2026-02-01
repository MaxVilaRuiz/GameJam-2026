#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <queue>
#include <string>

#include "../../globals/global.hpp"
#include "../../entities/enemy.hpp"

class EarthAttack1
{
private:
    SDL_Texture* texture;
    SDL_Rect rect;

    float BASE_DURATION;
    float currentTime;

public:
    EarthAttack1(SDL_Rect spawnRect);

    ~EarthAttack1();

    bool IsAlive() const;

    void Update(double deltaTime);

    void Render();
};