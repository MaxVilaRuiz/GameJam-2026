#include "fire_attack2.hpp"

float FireAttack2::angle(SDL_Rect spawnRect, std::pair<float, float> dir){

    float dirx = dir.first;
    float diry = dir.second;
    float angle = atan2(diry, dirx) * 180.0f / M_PI;
    angle -= 180;
    while (angle < 0) angle += 360.0f;
    while(angle > 360) angle -= 360.0f;
    

    return angle;

}

FireAttack2::rectangle FireAttack2::aimer(SDL_Rect spawnRect, std::pair<float, float> dir){

    rectangle aux;

    float x = spawnRect.x;
    float y = spawnRect.y;

    float dx = dir.first;   // vector unitari
    float dy = dir.second;

    float dist = 3.5f;

    // Punt perpendicular esquerra
    aux.c.first  = x + (-dy) * dist;
    aux.c.second = y + ( dx) * dist;

    aux.d.first  = x + ( dy) * dist;
    aux.d.second = y + (-dx) * dist;

    aux.a.first = aux.c.first + rect.w * dx / 18;
    aux.a.second = aux.c.second + rect.h * dy / 18;

    aux.b.first = aux.d.first + rect.w * dx / 18;
    aux.b.second = aux.d.second + rect.h * dy / 18;

    return aux;

}


// Constructor
FireAttack2::FireAttack2(SDL_Rect spawnRect, std::pair<float, float> dir) 
{
    SDL_Surface* temp = IMG_Load("../assets/secondartFireAttk_1.png");
    texture1 = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    temp = IMG_Load("../assets/secondartFireAttk_2.png");
    texture2 = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    rect = spawnRect;
    lifetime = 1.00;
    ang = angle(spawnRect, dir);
}


// Destructor
FireAttack2::~FireAttack2()
{
    if(texture1) SDL_DestroyTexture(texture1);
    if(texture2) SDL_DestroyTexture(texture2);
}


// Public functions
bool FireAttack2::IsAlive() const { 
    return lifetime >= 0; 
} 

void FireAttack2::Update(double deltaTime, std::pair<float, float> dir, SDL_Rect spawnRect)
{
    

    lifetime -= deltaTime;
    cd -= deltaTime;
    ang = angle(spawnRect, dir);
    rect = spawnRect;
    rectangle hitbox = aimer(spawnRect, dir);

    for (int i = 0; i < enemies.size(); ++i) {
        if (enemies[i]->InPlayerRange()) {
            std::pair<float, float> epos = enemies[i]->GetPos();
            FireAttack2::rectangle erect;
            erect.a = epos;
            erect.b = {epos.first + 80, epos.second};
            erect.c = {epos.first, epos.second + 128};
            erect.d = {epos.first + 80, epos.second + 128};

            int offx = hitbox.a.first - hitbox.c.first;
            int offy = hitbox.a.second - hitbox.c.second;

            offx/=6;
            offy/=6;

            bool done = 0;

            for(int j = 0; j<7; j++){

                int eqis = hitbox.c.first + offx * j;
                int iy = hitbox.c.second + offy * j;

                if( eqis < erect.b.first && eqis > erect.a.first && iy < erect.c.second && iy > erect.a.second && cd <= 0 ){
                    enemies[i]->TakeDamage(1);
                    done = 1;
                    cd = 0.27;
                    break;
                }

            }

            offx = hitbox.b.first - hitbox.d.first;
            offy = hitbox.b.second - hitbox.d.second;

            if(!done){
                for(int j = 0; j<7; j++){

                    int eqis = hitbox.d.first + (offx * j);
                    int iy = hitbox.d.second + (offy * j);

                    if( eqis < erect.b.first && eqis > erect.a.first && iy < erect.c.second && iy > erect.a.second && cd <= 0 ){
                        enemies[i]->TakeDamage(1);
                        done = 1;
                        cd = 0.27;
                        break;
                    }

                }
            }
        }

    }
}

void FireAttack2::Render()
{
    SDL_Point roteen = { 80, 35 };
    if(((int)(lifetime * 10) % 2) == 0) SDL_RenderCopyEx(renderer, texture1, NULL, &rect, ang, &roteen, SDL_FLIP_NONE);
    SDL_RenderCopyEx(renderer, texture2, NULL, &rect, ang, &roteen, SDL_FLIP_NONE);
}