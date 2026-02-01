#include "tile_map.hpp"

// Constructor
Tilemap::Tilemap() :
    srcRect({0, 0, 16, 16}),
    destRect({0, 0, displayBounds.w / 24 + 1, displayBounds.h / 16 + 1})
    {
        SDL_Surface* temp = IMG_Load("../assets/Overworld.png");
        tilemap_tex = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);

        TILE_SIZE.first = displayBounds.w / 24;
        TILE_SIZE.second = displayBounds.h / 24;
    }


// Public functions
void Tilemap::Render()
{
    for(int i = 0; i < 16; i++) {
        for(int j = 0; j < 24; j++)
        {
            SDL_RenderCopy(renderer, tilemap_tex, &srcRect, &destRect);
            destRect.x += displayBounds.w / 24 + 1;
        }
        destRect.x = 0;
        destRect.y += displayBounds.h / 16 + 1;
    }

    destRect.x = 0;
    destRect.y = 0;
}