#include "earth_attack2.hpp"


// Constructor
EarthAttack2::EarthAttack2(SDL_Rect spawnRect) :
    BASE_DURATION(10.0f)
{
    SDL_Surface* temp = IMG_Load("../assets/rock.png");
    texture = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    rect = spawnRect;
    currentTime = BASE_DURATION;

    int tileX0 = rect.x / TILE_SIZE.first;
    int tileY0 = rect.y / TILE_SIZE.second;
    int tileX1 = (rect.x + rect.w - 1) / TILE_SIZE.first;
    int tileY1 = (rect.y + rect.h - 1) / TILE_SIZE.second;
    tileX0 = SDL_clamp(tileX0, 0, 23);
    tileX1 = SDL_clamp(tileX1, 0, 23);
    tileY0 = SDL_clamp(tileY0, 0, 15);
    tileY1 = SDL_clamp(tileY1, 0, 15);

    for(int ty = tileY0; ty <= tileY1; ty++)
    {
        for(int tx = tileX0; tx <= tileX1; tx++)
        {
            map[ty][tx] = 2;
        }
    }
    
    for (int i = 0; i < enemies.size(); ++i) {
        if (enemies[i]->InPlayerRange()) {
            if (SDL_HasIntersection(enemies[i]->EnemyRect(), &rect)) {
                enemies[i]->TakeDamage(2); // Change??
            }
        }
    }
}


// Destructor
EarthAttack2::~EarthAttack2()
{
    if(texture) SDL_DestroyTexture(texture);
    for(int ty = tileY0; ty <= tileY1; ty++)
    {
        for(int tx = tileX0; tx <= tileX1; tx++)
        {
            map[ty][tx] = 0;
        }
    }
}


// Public functions
bool EarthAttack2::IsAlive() const { 
    return currentTime > 0.0f; 
}

void EarthAttack2::Update(double deltaTime)
{
    currentTime -= deltaTime;
}

void EarthAttack2::Render()
{
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}