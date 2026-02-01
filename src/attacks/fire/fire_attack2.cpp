#include "fire_attack2.hpp"

float FireAttack2::angle(std::pair<float, float> dir){

    float mag = sqrtf((spawnRect.x-dir.first) * (spawnRect.x-dir.first) + (spawnRect.y-dir.second)*(spawnRect.y-dir.second));
    float dirx = (dir.first - spawnRect.x) / mag;
    float diry = (dir.second - spawnRect.y) / mag;
    float angle = atan2(diry, dirx) * 180.0f / M_PI;
    if (angle < 0) angle += 360.0f;

}

rectangle FireAttack2::aimer(SDL_Rect spawnRect, std::pair<float, float> dir){

    rectangle aux;
    float angx = ang-90;
    if(angx < 0) angx += 360;
    
    aux.a.first = spawnRect.x + 5 / tan(angx);  
    aux.a.second = spawnRect.y + 5 * tan(angx);  

    float x = spawnRect.x;
    float y = spawnRect.y;

    float dx = dir.first;   // vector unitari
    float dy = dir.second;

    float dist = 3.5f;

    // Punt perpendicular esquerra
    aux.d.first  = x + (-dy) * dist;
    aux.d.second = y + ( dx) * dist;

    aux.c.first  = x + ( dy) * dist;
    aux.c.second = y + (-dx) * dist;

    aux.a.first = aux.c.first + rect.w;
    aux.a.second = aux.c.second + rect.h;

    aux.b.first = aux.d.first + rect.w;
    aux.b.second = aux.d.second + rect.h;

    return aux;

}


// Constructor
FireAttack2::FireAttack2(SDL_Rect spawnRect, std::pair<float, float> dir) 
{
    SDL_Surface* temp = IMG_Load("../assets/secondaryFireAttk_1.png");
    texture1 = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    SDL_Surface* temp = IMG_Load("../assets/secondaryFireAttk_2.png");
    texture2 = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    rect = spawnRect;
    lifetime = 1;00;
    ang = angle(dir);
}


// Destructor
FireAttack2::~FireAttack2()
{
    if(texture1) SDL_DestroyTexture(texture1);
    if(texture2) SDL_DestroyTexture(texture2);
}


// Public functions
bool FireAttack2::IsAlive() const { 
    return rect.x > -10 & rect.y > -10 & rect.x < 2250 & rect.y < 1250; 
} 

void FireAttack2::Update(double deltaTime, std::pair<float, float> dir, SDL_Rect spawnRect)
{

    lifetime -= deltaTime;
    ang = angle(dir);

    std::pair<int, int> start1 = {spawnRect.x- 4, spawnRect.y};
    std::pair<int, int> start2 = {spawnRect.x+ 4, spawnRect.y};


    for (int i = 0; i < enemies.size(); ++i) {
        if (enemies[i]->InPlayerRange()) {
            if (SDL_HasIntersection(enemies[i]->EnemyRect(), &rect)) {
                enemies[i]->TakeDamage(1);
            }
        }
    }
}

void FireAttack2::Render()
{
    
    SDL_RenderCopyEx(renderer, texture, NULL, &rect, angle, NULL, SDL_FLIP_NONE);
}