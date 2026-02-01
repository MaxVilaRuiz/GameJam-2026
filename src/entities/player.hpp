#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <queue>
#include <string>

#include "../attacks/earth/earth_attack1.hpp"
#include "../attacks/earth/earth_attack2.hpp"
#include "../attacks/fire/fire_attack1.hpp"
#include "../attacks/air/air_attack1.hpp"
#include "../attacks/water/water_attack1.hpp"

class Player
{
private:
    SDL_Texture* texture;
    SDL_Texture* aim_target;

    std::vector<SDL_Texture*> sprites;

    SDL_Rect aimTargetRect;
    SDL_Rect destRect;

    bool aimTargetReady;

    int dirIndex;

    float posX, posY;
    const float speed = 250.0f;

    float aimPosX, aimPosY;
    const int aimRange = 250;

    int maxHealth;
    int currentHealth;
    float damageCooldown;
    const float INVINCIBILITY_TIME = 1.0f;

    std::vector<int> maskLvl;
    int primaryMask;                                // 0: Earth; 1: Fire; 2: Air; 3: Water 
    int secondaryMask;                              // 0: Earth; 1: Fire; 2: Air; 3: Water 
    std::vector<SDL_Texture*> masks_textures;
    std::vector<SDL_Rect> masks_bounds;

    std::vector<FireAttack1*> attacksFire;
    std::vector<AirAttack1*> attacksAir;
    std::vector<WaterAttack1*> attacksWater;
    std::vector<EarthAttack1*> ePrimaryAttacks;
    std::vector<EarthAttack2*> eSecondaryAttacks;
    float primaryCooldownEarth;
    float PRIMARY_COOLDOWN_TIME_EARTH;
    float primaryCooldownFire;
    float PRIMARY_COOLDOWN_TIME_FIRE;
    float primaryCooldownAir;
    float PRIMARY_COOLDOWN_TIME_AIR;
    float primaryCooldownWater;
    float PRIMARY_COOLDOWN_TIME_WATER;

    float secondaryCooldownEarth;
    float SECONDARY_COOLDOWN_TIME_EARTH;

    float maskSwitchCooldown;
    float MASK_SWITCH_COOLDOWN_TIME;

    const int offsetX = 40;
    const int offsetY = 0;

    std::vector<SDL_Texture*> maskSprites();

    void Movement(double deltaTime);

    void Aim();

    void PrimaryAttack();

    void SecondaryAttack();

public:
    Player();

    ~Player();

    const SDL_Rect* PlayerRect();

    void TakeDamage(int amount);

    int GetCurrentHealth();

    float GetPrimaryCooldown();

    float GetSecondaryCooldown();

    void Update(double deltaTime);

    void Render();
};