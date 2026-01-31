#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <queue>
#include <string>

#include "../entities/player.hpp"

class UI
{
private:
    Player* player;

    SDL_Texture* heart_full_tex;
    SDL_Texture* heart_half_tex;
    SDL_Texture* heart_empty_tex;

    SDL_Rect heartsRect;
    SDL_Rect heartsStartPos;

    std::vector<int> hearts; // 0 full, 1 half, 2 empty

    SDL_Texture* icon_mainatt_kbm;
    SDL_Texture* icon_mainatt_cont;
    SDL_Texture* icon_secatt_kbm;
    SDL_Texture* icon_secatt_cont;

    SDL_Rect mainAttRect, secAttRect;

public:
    UI(Player* _player);

    void SetHearts(int amount);

    void Render();
};