#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <random>

#include "../globals/global.hpp"

class Tilemap
{
private:
    SDL_Texture* tilemap_tex;
    SDL_Rect srcRect; // Choose tile
    SDL_Rect destRect;

    SDL_Rect chest = {16, 129, 16, 16};

    SDL_Rect wall1 = {15, 0, 16, 16};
    SDL_Rect wall2 = {31, 0, 16, 16};
    SDL_Rect wall3 = {95, 0, 16, 16};
    SDL_Rect wall4 = {63, 16, 16, 16};
    std::vector<SDL_Rect*> walls = {&wall1, &wall2, &wall3, &wall4};

    SDL_Rect wallr1 = {15, 64, 16, 16};

    SDL_Rect ground1 = {47, 32, 16, 16};
    SDL_Rect ground2 = {111, 48, 16, 16};
    SDL_Rect ground3 = {126, 48, 16, 16};
    SDL_Rect ground4 = {143, 32, 16, 16};
    std::vector<SDL_Rect*> grounds = {&ground1, &ground2, &ground3, &ground4};

    std::vector<std::vector<SDL_Rect*>> tiletype;

public:
    Tilemap(uint64_t seed);

    std::pair<int, int> GetRandomTile();
    void Render();
};