#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <queue>
#include <iostream>

#include "../globals/global.hpp"

class Merchant
{
private:
    SDL_Texture* texture;
    SDL_Rect destRect;

public:
    Merchant();

    void Render();
};