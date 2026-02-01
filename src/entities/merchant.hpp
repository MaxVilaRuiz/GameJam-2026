#pragma once

#include "../globals/global.hpp"

class Merchant
{
private:
    SDL_Texture* texture;
    SDL_Texture* textureunactive;
    SDL_Texture* textureactive;
    SDL_Rect destRect;

public:
    Merchant();

    void SwitchMerchantState(bool sw);
    const SDL_Rect* GetRect();

    void Render();
};