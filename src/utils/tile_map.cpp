#include "tile_map.hpp"

// Constructor
Tilemap::Tilemap(uint64_t seed) :
    srcRect({15, 0, 16, 16}),
    destRect({0, 0, displayBounds.w / 24 + 1, displayBounds.h / 16 + 1})
    {
        srand(seed);
        SDL_Surface* temp = IMG_Load("../assets/DungeonTilesetScaled.png");
        tilemap_tex = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);

        TILE_SIZE.first = displayBounds.w / 24;
        TILE_SIZE.second = displayBounds.h / 16;

        tiletype = std::vector<std::vector<SDL_Rect*>>(16, std::vector<SDL_Rect*>(24));

        int chestpos = -1;

        if(rand() % 100 < 30)
        {
            do {
                chestpos = rand() % (24 * 16);
            } while ((chestpos / 24) == 0 || (chestpos / 24) == 15);
        }

        for(int i = 0; i < 16; i++)
        {
            for(int j = 0; j < 24; j++)
            {
                if(i*24+j == chestpos)
                {
                    tiletype[i][j] = &chest;
                    continue;
                }

                if(i == 0)
                {
                    int r = rand() % walls.size();
                    tiletype[i][j] = walls[r];
                }
                else if(i == 15)
                {
                    tiletype[i][j] = &wallr1;
                }
                else
                {
                    int r = rand() % grounds.size();
                    tiletype[i][j] = grounds[r];
                }
            }
        }
    }


// Public functions
void Tilemap::Render()
{
    for(int i = 0; i < 16; i++) {
        for(int j = 0; j < 24; j++)
        {
            SDL_RenderCopy(renderer, tilemap_tex, tiletype[i][j], &destRect);
            destRect.x += displayBounds.w / 24 + 1;
        }
        destRect.x = 0;
        destRect.y += displayBounds.h / 16 + 1;
    }

    destRect.x = 0;
    destRect.y = 0;
}

std::pair<int, int> Tilemap::GetRandomTile()
{
    int randpos = -1;
    do {
        randpos = rand() % (24 * 16);
    } while ((randpos / 24) == 0 || (randpos / 24) == 15);

    int row = randpos / 24;
    int col = randpos % 24;

    int tx = TILE_SIZE.first ? TILE_SIZE.first : (displayBounds.w / 24);
    int ty = TILE_SIZE.second ? TILE_SIZE.second : (displayBounds.h / 16);

    int x = col * tx + tx / 2;
    int y = row * ty + ty / 2;

    return {x, y};
}