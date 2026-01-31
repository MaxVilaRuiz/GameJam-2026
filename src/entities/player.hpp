#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <queue>
#include <string>

#include "../attacks/earth/earth_attack1.hpp"

class Player
{
private:
    SDL_Texture* texture;
    SDL_Texture* aim_target;

    SDL_Rect aimTargetRect;
    SDL_Rect destRect;

    bool aimTargetReady;

    float posX, posY;
    const float speed = 250.0f;

    float aimPosX, aimPosY;
    const int aimRange = 250;

    int maxHealth;
    int currentHealth;
    float damageCooldown;
    const float INVINCIBILITY_TIME = 1.0f;

    std::vector<int> maskLvl;
    int primaryMask;
    int secondaryMask;
    std::vector<SDL_Texture*> masks_textures;
    std::vector<SDL_Rect> masks_bounds;

    std::vector<EarthAttack1*> attacks;
    float primaryCooldownEarth;
    float PRIMARY_COOLDOWN_TIME_EARTH;
    float primaryCooldownFire;
    float PRIMARY_COOLDOWN_TIME_FIRE;
    float primaryCooldownAir;
    float PRIMARY_COOLDOWN_TIME_AIR;
    float primaryCooldownWater;
    float PRIMARY_COOLDOWN_TIME_WATER;

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

    void Update(double deltaTime);

    void Render();
};