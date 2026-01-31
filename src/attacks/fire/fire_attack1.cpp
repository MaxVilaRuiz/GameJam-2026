#include "fire_attack1.hpp"


// Constructor
FireAttack1::FireAttack1(SDL_Rect spawnRect, SDL_Rect DirectionRect) :
    speedf(467)
{
    SDL_Surface* temp = IMG_Load("../assets/p_fire_f1.png");
    texture = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    rect = spawnRect;
    float mag = sqrtf((spawnRect.x-DirectionRect.x) * (spawnRect.x-DirectionRect.x) + (spawnRect.y-DirectionRect.y)*(spawnRect.y-DirectionRect.y));
    dirf.first = (DirectionRect.x - spawnRect.x) / mag;
    dirf.second = (DirectionRect.y - spawnRect.y) / mag;
}


// Destructor
FireAttack1::~FireAttack1()
{
    if(texture) SDL_DestroyTexture(texture);
}


// Public functions
bool FireAttack1::IsAlive() const { 
    return rect.x > -10 & rect.y > -10 & rect.x < 2250 & rect.y < 1250; 
} 

void FireAttack1::Update(double deltaTime)
{

    float aux = deltaTime * speedf * dirf.first;
    float auy = deltaTime * speedf * dirf.second;

    rect.x += (int)aux;
    rect.y += (int)auy;

    for (int i = 0; i < enemies.size(); ++i) {
        if (enemies[i]->InPlayerRange()) {
            if (SDL_HasIntersection(enemies[i]->EnemyRect(), &rect)) {
                enemies[i]->TakeDamage(1);
                rect.x = -1000;
            }
        }
    }
}

void FireAttack1::Render()
{
    float angle = atan2(dirf.second, dirf.first) * 180.0f / M_PI;
    if (angle < 0) angle += 360.0f;
    
    SDL_RenderCopyEx(renderer, texture, NULL, &rect, angle, NULL, SDL_FLIP_NONE);
}