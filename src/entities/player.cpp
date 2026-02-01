#include "player.hpp"


// Constructor
Player::Player() :
    aimTargetReady(false),
    maxHealth(10),
    damageCooldown(0.0f),
    maskLvl({0, 0, 0, 0}),
    primaryMask(0),
    secondaryMask(1),
    primaryCooldownEarth(0.0f),
    PRIMARY_COOLDOWN_TIME_EARTH(0.4f),
    primaryCooldownFire(0.0f),
    PRIMARY_COOLDOWN_TIME_FIRE(0.4f),
    primaryCooldownAir(0.0f),
    PRIMARY_COOLDOWN_TIME_AIR(0.4f),
    primaryCooldownWater(0.0f),
    PRIMARY_COOLDOWN_TIME_WATER(0.4f),
    secondaryCooldownEarth(0.0f),
    SECONDARY_COOLDOWN_TIME_EARTH(5.0f),
    maskSwitchCooldown(0.0f),
    MASK_SWITCH_COOLDOWN_TIME(1.0f)
    {
        SDL_Surface* temp = IMG_Load("../assets/monk de frente.png");
        texture = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("../assets/aim.png");
        aim_target = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);

        aimTargetRect = {0, 0, 64, 64};
        destRect = {0, 0, 128, 128};
        posX = (float)destRect.x;
        posY = (float)destRect.y;

        currentHealth = maxHealth;
        masks_textures = maskSprites();
    }


// Destructor
Player::~Player()
{
    if(texture) SDL_DestroyTexture(texture);
    if(aim_target) SDL_DestroyTexture(aim_target);
    for(auto* a : attacksFire) delete a;
    attacksFire.clear();
    for(auto* a : ePrimaryAttacks) delete a;
    ePrimaryAttacks.clear();
    for(auto* a : eSecondaryAttacks) delete a;
    eSecondaryAttacks.clear();
    // for(auto* a : fSecondaryAttacks) delete a;
    // fSecondaryAttacks.clear();
}


// Private functions
std::vector<SDL_Texture*> Player::maskSprites()
{
    std::vector<SDL_Texture*> sprites(4);
    masks_bounds = std::vector<SDL_Rect>(4);
    std::vector<std::string> names = {"terra 1.png", "foc 1.png", "aire 1.png", "awa 1.png"};

    for(int i = 0; i < 4; i++)
    {
        SDL_Surface* temp = IMG_Load(("../assets/masks/" + names[i]).c_str());
        sprites[i] = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);
        masks_bounds[i] = {destRect.x, destRect.y, 48, 48};
    }

    return sprites;
}

void Player::Movement(double deltaTime)
{
    // MOVEMENT
    if(controller)
    {
        Sint16 lx = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
        Sint16 ly = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);

        const float deadzone = 8000.0f / 32767.0f;
        float nx = lx / 32767.0f;
        float ny = ly / 32767.0f;
        float mag = sqrtf(nx * nx + ny * ny);

        if(mag > deadzone)
        {
            float scaledMag = (mag - deadzone) / (1.0f - deadzone);
            if(scaledMag > 1.0f) scaledMag = 1.0f;

            float dirx = nx / mag;
            float diry = ny / mag;

            float scaled = scaledMag * speed * deltaTime;

            posX += dirx * scaled;
            posY += diry * scaled;

            destRect.x = (int)posX;
            destRect.y = (int)posY;
        }
    }
    else
    {
        const Uint8* state = SDL_GetKeyboardState(NULL);
        int xdir = 0;
        int ydir = 0;
        if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP])    ydir -= 1;
        if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN])  ydir += 1;
        if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT])  xdir -= 1;
        if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) xdir += 1;
        
        float fx = (float)xdir;
        float fy = (float)ydir;

        float lennorm = sqrt(fx * fx + fy * fy);

        if(lennorm > 0.0)
        {
            fx /= lennorm;
            fy /= lennorm;
            
            posX += fx * speed * deltaTime;
            posY += fy * speed * deltaTime;

            destRect.x = (int)posX;
            destRect.y = (int)posY;
        }
    }

    playerPos.first = destRect.x + destRect.w / 2;
    playerPos.second = destRect.y + destRect.h / 2;
}

void Player::Aim()
{
    // AIM
    if(controller)
    {
        Sint16 rx = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
        Sint16 ry = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);

        const float deadzone = 8000.0f / 32767.0f;
        float nx = rx / 32767.0f;
        float ny = ry / 32767.0f;
        float mag = sqrtf(nx * nx + ny * ny);

        if(mag > deadzone)
        {
            aimTargetReady = true;

            float scaledMag = (mag - deadzone) / (1.0f - deadzone);
            if(scaledMag > 1.0f) scaledMag = 1.0f;

            float dirx = nx / mag;
            float diry = ny / mag;

            float scaled = scaledMag * aimRange;

            aimPosX = (destRect.x + destRect.w/2) + dirx * scaled;
            aimPosY = (destRect.y + destRect.h/2) + diry * scaled;

            aimTargetRect.x = (int)aimPosX;
            aimTargetRect.y = (int)aimPosY;

            aimTargetRect.x = (int)aimPosX - aimTargetRect.w / 2;
            aimTargetRect.y = (int)aimPosY - aimTargetRect.h / 2;
        }
        else
        {
            aimTargetReady = false;
        }
    }
    else
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        float deltaMouseX = (float)mouseX - (destRect.x + destRect.w * 0.5f);
        float deltaMouseY = (float)mouseY - (destRect.y + destRect.h * 0.5f);
        float mDirMag = sqrtf(deltaMouseX * deltaMouseX + deltaMouseY * deltaMouseY);

        if(mDirMag > 0.01f)
        {
            float dirX = mDirMag > aimRange ? deltaMouseX / mDirMag : deltaMouseX / aimRange;
            float dirY = mDirMag > aimRange ? deltaMouseY / mDirMag : deltaMouseY / aimRange;

            aimPosX = (destRect.x + destRect.w * 0.5f) + dirX * (float)aimRange;
            aimPosY = (destRect.y + destRect.h * 0.5f) + dirY * (float)aimRange;

            aimTargetRect.x = (int)aimPosX - aimTargetRect.w / 2;
            aimTargetRect.y = (int)aimPosY - aimTargetRect.h / 2;
            aimTargetReady = true;
        }
        else
        {
            aimTargetReady = false;
        }
    }
}

void Player::PrimaryAttack()
{
    if(primaryMask == 0 && primaryCooldownEarth <= 0.0f && aimTargetReady)
    {
        int esize = 64;
        if(maskLvl[0] >=3) esize += 25;
        
        ePrimaryAttacks.push_back(new EarthAttack1({aimTargetRect.x - (esize - 64)/2, aimTargetRect.y - (esize - 64)/2 , esize, esize}));
        primaryCooldownEarth = PRIMARY_COOLDOWN_TIME_EARTH;
    }
    else if (primaryMask == 1 && primaryCooldownFire <= 0.0f && aimTargetReady){

        int fsize = 40;
        if(maskLvl[0] >=3) fsize += 20;
        
        attacksFire.push_back(new FireAttack1({destRect.x + destRect.w/2, destRect.y+destRect.h/2, fsize, fsize}, aimTargetRect));
        primaryCooldownFire = PRIMARY_COOLDOWN_TIME_FIRE;

    }
}

void Player::SecondaryAttack() 
{
    if(primaryMask == 0 && secondaryCooldownEarth <= 0.0f && aimTargetReady)
    {
        int esize = 128;
        eSecondaryAttacks.push_back(new EarthAttack2({aimTargetRect.x - 40, aimTargetRect.y - 40, esize, esize}));
        secondaryCooldownEarth = SECONDARY_COOLDOWN_TIME_EARTH;
    }

    if(primaryMask == 1 && secondaryCooldownFire <= 0.0f)
    {
        int fsize = 80;
        //fSecondaryAttacks.push_back(new EarthAttack2({aimTargetRect.x - 40, aimTargetRect.y - 40, fsize/9, fsize}));
        secondaryCooldownFire = SECONDARY_COOLDOWN_TIME_FIRE;
    }
}


// Public functions
const SDL_Rect* Player::PlayerRect()
{
    return &destRect;
}

void Player::TakeDamage(int amount)
{
    if(damageCooldown > 0.0f) return;
    currentHealth -= amount;
    damageCooldown = INVINCIBILITY_TIME;
}

int Player::GetCurrentHealth()
{
    return currentHealth;
}

float Player::GetPrimaryCooldown()
{
    switch(primaryMask)
    {
        case 0:
            return primaryCooldownEarth;
            break;
        case 1:
            return primaryCooldownFire;
            break;
        case 2:
            return primaryCooldownAir;
            break;
        case 3:
            return primaryCooldownWater;
            break;
    }

    return 0.0;
}

float Player::GetSecondaryCooldown()
{
    return secondaryCooldownEarth;
}

void Player::Update(double deltaTime) 
{
    int tileW = displayBounds.w / 24;
    int tileH = displayBounds.h / 16;
    int gridX = (destRect.x + destRect.w / 2) / tileW;
    int gridY = (destRect.y + destRect.h / 2) / tileH;

    if (gridX >= 0 && gridX < 24 && gridY >= 0 && gridY < 16) {
        map[gridY][gridX] = 0;
    }

    if(damageCooldown > 0.0f){
        damageCooldown -= deltaTime;
    }
    if(primaryCooldownEarth > 0.0f) primaryCooldownEarth -= deltaTime;
    if(secondaryCooldownEarth > 0.0f) secondaryCooldownEarth -= deltaTime;
    if(primaryCooldownFire > 0.0f) primaryCooldownFire -= deltaTime;
    if(maskSwitchCooldown > 0.0f) maskSwitchCooldown -= deltaTime;

    for(auto it = ePrimaryAttacks.begin(); it != ePrimaryAttacks.end();)
    {
        (*it)->Update(deltaTime);
        if(!(*it)->IsAlive())
        {
            delete *it;
            it = ePrimaryAttacks.erase(it);
        }
        else ++it;
    }
    for(auto it = eSecondaryAttacks.begin(); it != eSecondaryAttacks.end();)
    {
        (*it)->Update(deltaTime);
        if(!(*it)->IsAlive())
        {
            delete *it;
            it = eSecondaryAttacks.erase(it);
        }
        else ++it;
    }
    // for(auto it = fSecondaryAttacks.begin(); it != fSecondaryAttacks.end();)
    // {
    //     (*it)->Update(deltaTime);
    //     if(!(*it)->IsAlive())
    //     {
    //         delete *it;
    //         it = fSecondaryAttacks.erase(it);
    //     }
    //     else ++it;
    // }
    for(auto it = attacksFire.begin(); it != attacksFire.end();)
    {
        (*it)->Update(deltaTime);
        if(!(*it)->IsAlive())
        {
            delete *it;
            it = attacksFire.erase(it);
        }
        else ++it;
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);
    bool maskSwitchButton = (state[SDL_SCANCODE_E] 
    || (controller && SDL_GameControllerGetButton(controller, SDL_GameControllerButton(SDL_CONTROLLER_BUTTON_X))));
    if(maskSwitchButton && maskSwitchCooldown <= 0.0f)
    {
        if(secondaryMask != -1)
        {
            int temp = primaryMask;
            primaryMask = secondaryMask;
            secondaryMask = temp;
            maskSwitchCooldown = MASK_SWITCH_COOLDOWN_TIME;
        }
    }

    Movement(deltaTime);
    Aim();

    if((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) || 
    (controller && SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 1000)){
        PrimaryAttack();
    }

    if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT) || 
    (controller && SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) > 1000)){
        SecondaryAttack();
    }

    gridX = (destRect.x + destRect.w / 2) / tileW;
    gridY = (destRect.y + destRect.h / 2) / tileH;

    if (gridX >= 0 && gridX < 24 && gridY >= 0 && gridY < 16) {
        map[gridY][gridX] = 1;
    }

    masks_bounds[primaryMask].x = destRect.x + offsetX;
    masks_bounds[primaryMask].y = destRect.y + offsetY;
}

void Player::Render()
{
    if(aimTargetReady) SDL_RenderCopy(renderer, aim_target, NULL, &aimTargetRect);
    for(auto* a : ePrimaryAttacks) a->Render();
    for(auto* a : eSecondaryAttacks) a->Render();
    //for(auto* a : fSecondaryAttacks) a->Render();
    for(auto* a : attacksFire) a->Render();
    
    if(damageCooldown > 0.0f)
    {
        SDL_SetTextureColorMod(texture, 255, 100, 100);
    }
    else
    {
        SDL_SetTextureColorMod(texture, 255, 255, 255);
    }

    SDL_RenderCopy(renderer, texture, NULL, &destRect);

    SDL_RenderCopy(renderer, masks_textures[primaryMask], NULL, &masks_bounds[primaryMask]);
}