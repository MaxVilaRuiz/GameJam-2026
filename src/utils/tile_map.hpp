#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <queue>
#include <string>

#include "../globals/global.hpp"

class Tilemap
{
private:
    SDL_Texture* tilemap_tex;
    SDL_Rect srcRect; // Choose tile
    SDL_Rect destRect;

public:
    Tilemap();

    void Render();
};