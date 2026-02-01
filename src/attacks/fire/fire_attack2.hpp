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


class FireAttack2
{
private:

    struct rectangle{
        std::pair<int,int> a;
        std::pair<int,int> b;
        std::pair<int,int> c;
        std::pair<int,int> d;
    };

    float ang;
    SDL_Texture* texture1;
    SDL_Texture* texture2;
    SDL_Rect rect;
    double lifetime;

    float angle (std::pair<float, float> dir);

    rectangle aimer(SDL_Rect spawnRect);
    
public:
    FireAttack2(SDL_Rect spawnRect, std::pair<float, float> dir);

    ~FireAttack2();

    bool IsAlive() const;

    void Update(double deltaTime, std::pair<float, float> dir, SDL_Rect spawnRect);

    void Render();
};