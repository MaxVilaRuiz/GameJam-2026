#include "ui.hpp"


// Constructor
UI::UI(Player* _player)
{
    player = _player;

    SDL_Surface* temp = IMG_Load("../assets/heart_full.png");
    heart_full_tex = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);

    temp = IMG_Load("../assets/heart_half.png");
    heart_half_tex = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);

    temp = IMG_Load("../assets/heart_empty.png");
    heart_empty_tex = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);

    temp = IMG_Load("../assets/icon_atm_mk.png");
    icon_mainatt_kbm = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);

    temp = IMG_Load("../assets/icon_ats_mk.png");
    icon_secatt_kbm = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);

    heartsStartPos = {32, 32, 64, 64};
    heartsRect = heartsStartPos;
    hearts = std::vector<int>(5, 0);

    mainAttRect = {50, displayBounds.h - 164, 128, 128};
    secAttRect = {208, displayBounds.h - 164, 128, 128};

    SetHearts(10);
}


// Public functions
void UI::SetHearts(int amount)
{
    int hamount = amount / 2;
    for(int i = 1; i <= hearts.size(); i++)
    {
        if(i <= hamount) hearts[i-1] = 0;
        else if(amount%2 != 0 and i == hamount+1) hearts[i-1] = 1;
        else hearts[i-1] = 2;
    }
}

void UI::Render()
{
    for(int i = 0; i < hearts.size(); i++)
    {
        SDL_Texture* heart;

        switch(hearts[i])
        {
            case 0:
                heart = heart_full_tex;
                break;
            case 1:
                heart = heart_half_tex;
                break;
            case 2:
                heart = heart_empty_tex;
                break;
        }

        SDL_RenderCopy(renderer, heart, NULL, &heartsRect);

        heartsRect.x += 80;
    }
    
    heartsRect.x = heartsStartPos.x;

    if(player->GetPrimaryCooldown() > 0.0f) SDL_SetTextureAlphaMod(icon_mainatt_kbm, 100);
    else SDL_SetTextureAlphaMod(icon_mainatt_kbm, 255);

    SDL_RenderCopy(renderer, icon_mainatt_kbm, NULL, &mainAttRect);
    SDL_RenderCopy(renderer, icon_secatt_kbm, NULL, &secAttRect);
}