#include "merchant.hpp"

Merchant::Merchant()
{
    destRect = {displayBounds.w / 2 - 100, 
    displayBounds.h / 2 - 100 - TILE_SIZE.second * 2, 200, 200};

    SDL_Surface* temp = IMG_Load(((std::string)SDL_GetBasePath() + "../assets/merchant_idle_1.png").c_str());
    texture = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
}

void Merchant::Render()
{
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
}