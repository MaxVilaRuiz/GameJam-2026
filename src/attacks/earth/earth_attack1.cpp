#include "earth_attack1.hpp"


// Constructor
EarthAttack1::EarthAttack1(SDL_Rect spawnRect) :
    BASE_DURATION(1.0f)
{
    SDL_Surface* temp = IMG_Load("../assets/crack.png");
    texture = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    rect = spawnRect;
    currentTime = BASE_DURATION;
    
    for (int i = 0; i < enemies.size(); ++i) {
        if (enemies[i]->InPlayerRange()) {
            if (SDL_HasIntersection(enemies[i]->EnemyRect(), &rect)) {
                enemies[i]->TakeDamage(1);
            }
        }
    }
}


// Public functions
EarthAttack1::~EarthAttack1()
{
    if(texture) SDL_DestroyTexture(texture);
}

bool EarthAttack1::IsAlive() const { 
    return currentTime > 0.0f; 
} 

void EarthAttack1::Update(double deltaTime)
{
    currentTime -= deltaTime;
}

void EarthAttack1::Render()
{
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}