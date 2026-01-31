#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include "enemy.hpp"
#include "global.hpp"

class EarthAttack1
{
private:
    SDL_Texture* texture;
    SDL_Rect rect;

    float BASE_DURATION = 1.0f;
    float currentTime;

public:

    EarthAttack1(SDL_Rect spawnRect)
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

    ~EarthAttack1()
    {
        if(texture) SDL_DestroyTexture(texture);
    }

    bool IsAlive() const { return currentTime > 0.0f; } 

    void Update(double deltaTime)
    {
        currentTime -= deltaTime;
    }

    void Render()
    {
        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }
};

class Player
{
private:
    SDL_Texture* texture;
    SDL_Texture* aim_target;

    SDL_Rect aimTargetRect;
    SDL_Rect destRect;

    bool aimTargetReady = false;

    float posX, posY;
    const float speed = 250.0f;

    float aimPosX, aimPosY;
    const int aimRange = 250;

    int maxHealth = 10;
    int currentHealth;
    float damageCooldown = 0.0f;
    const float INVINCIBILITY_TIME = 1.0f;

    std::vector<int> maskLvl = {0, 0, 0, 0};
    int primaryMask = 0;
    int secondaryMask = 1;
    std::vector<SDL_Texture*> masks_textures;
    std::vector<SDL_Rect> masks_bounds;

    std::vector<EarthAttack1*> attacks;
    float primaryCooldownEarth = 0.0f;
    float PRIMARY_COOLDOWN_TIME_EARTH = 0.4f;
    float primaryCooldownFire = 0.0f;
    float PRIMARY_COOLDOWN_TIME_FIRE = 0.4f;
    float primaryCooldownAir = 0.0f;
    float PRIMARY_COOLDOWN_TIME_AIR = 0.4f;
    float primaryCooldownWater = 0.0f;
    float PRIMARY_COOLDOWN_TIME_WATER = 0.4f;

    float maskSwitchCooldown = 0.0f;
    float MASK_SWITCH_COOLDOWN_TIME = 1.0f;

    const int offsetX = 40;
    const int offsetY = 0;

    std::vector<SDL_Texture*> maskSprites()
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

    void Movement(double deltaTime)
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

    void Aim()
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

                aimPosX = destRect.x + dirx * scaled;
                aimPosY = destRect.y + diry * scaled;

                aimTargetRect.x = (int)aimPosX;
                aimTargetRect.y = (int)aimPosY;
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

    void PrimaryAttack()
    {
        if(primaryMask == 0 && primaryCooldownEarth <= 0.0f && aimTargetReady)
        {
            int esize = 64;
            if(maskLvl[0] >=3) esize += 25;
            
            attacks.push_back(new EarthAttack1({aimTargetRect.x - (esize - 64)/2, aimTargetRect.y - (esize - 64)/2 , esize, esize}));
            primaryCooldownEarth = PRIMARY_COOLDOWN_TIME_EARTH;
        }
    }

    void SecondaryAttack()
    {

    }

public:
    Player()
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

    ~Player()
    {
        if(texture) SDL_DestroyTexture(texture);
        if(aim_target) SDL_DestroyTexture(aim_target);
        for(auto* a : attacks) delete a;
        attacks.clear();
    }

    const SDL_Rect* PlayerRect()
    {
        return &destRect;
    }

    void TakeDamage(int amount)
    {
        if(damageCooldown > 0.0f) return;
        currentHealth -= amount;
        damageCooldown = INVINCIBILITY_TIME;
    }

    int GetCurrentHealth()
    {
        return currentHealth;
    }

    float GetPrimaryCooldown()
    {
        return primaryCooldownEarth;
    }

    void Update(double deltaTime) {
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
        if(maskSwitchCooldown > 0.0f) maskSwitchCooldown -= deltaTime;

        for(auto it = attacks.begin(); it != attacks.end();)
        {
            (*it)->Update(deltaTime);
            if(!(*it)->IsAlive())
            {
                delete *it;
                it = attacks.erase(it);
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

        if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)){
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

    void Render()
    {
        if(aimTargetReady) SDL_RenderCopy(renderer, aim_target, NULL, &aimTargetRect);
        for(auto* a : attacks) a->Render();
        SDL_RenderCopy(renderer, texture, NULL, &destRect);

        SDL_RenderCopy(renderer, masks_textures[primaryMask], NULL, &masks_bounds[primaryMask]);
    }
};

class Tilemap
{
private:
    SDL_Texture* tilemap_tex;
    SDL_Rect srcRect = {0, 0, 16, 16}; // Choose tile
    SDL_Rect destRect = {0, 0, displayBounds.w / 24 + 1, displayBounds.h / 16 + 1};

public:
    Tilemap()
    {
        SDL_Surface* temp = IMG_Load("../assets/Overworld.png");
        tilemap_tex = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);
    }

    void Render()
    {
        for(int i = 0; i < 16; i++) {
            for(int j = 0; j < 24; j++)
            {
                SDL_RenderCopy(renderer, tilemap_tex, &srcRect, &destRect);
                destRect.x += displayBounds.w / 24 + 1;
            }
            destRect.x = 0;
            destRect.y += displayBounds.h / 16 + 1;
        }

        destRect.x = 0;
        destRect.y = 0;
    }
};


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

    UI(Player* _player)
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

    void SetHearts(int amount)
    {
        int hamount = amount / 2;
        for(int i = 1; i <= hearts.size(); i++)
        {
            if(i <= hamount) hearts[i-1] = 0;
            else if(amount%2 != 0 and i == hamount+1) hearts[i-1] = 1;
            else hearts[i-1] = 2;
        }
    }

    void Render()
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
};

int main()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr << "SDL_Init failed" << std::endl;
        return 1;
    }

    SDL_GetDisplayUsableBounds(0, &displayBounds);
    window = SDL_CreateWindow("MASK", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, displayBounds.w, displayBounds.h, SDL_WINDOW_FULLSCREEN_DESKTOP);

    if(!window)
    {
        std::cerr << "SDL_CreateWindow failed" << std::endl;
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if(!renderer)
    {
        std::cerr << "SDL_CreateRenderer failed" << std::endl;
        return 1;
    }

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cerr << "Mix_OpenAudio failed" << std::endl;
        return 1;
    }

    int flags = MIX_INIT_MP3;
    int initted = Mix_Init(flags);
    if ((initted & flags) != flags) {
        printf("No se pudo inicializar soporte MP3: %s\n", Mix_GetError());
        return 1;
    }

    mainTheme = Mix_LoadMUS("../assets/palo.mp3");
    shopTheme = Mix_LoadMUS("../assets/shop_music.mp3");

    if(!mainTheme || !shopTheme)
    {
        std::cerr << "Mix_LoadMUS failed" << std::endl;
        return 1;
    }

    SDL_GetRendererOutputSize(renderer, &displayBounds.w, &displayBounds.h);

    Uint64 frameStart, frameEnd;
    double deltaTime;
    frameStart = 0;
    frameEnd = SDL_GetPerformanceCounter();
    deltaTime = 0.0;

    map = std::vector<std::vector<int>>(16, std::vector<int>(24, 0));

    Player* player = new Player();
    Tilemap* tilemap = new Tilemap();
    enemies.push_back(new Enemy());
    
    UI* canvas = new UI(player);

    for(int i = 0; i < SDL_NumJoysticks(); i++)
    {
        if(SDL_IsGameController(i))
        {
            controller = SDL_GameControllerOpen(i);
            if(controller)
            {
                std::cout << "Controller connected: " << SDL_GameControllerName(controller) << std::endl;
                break;
            }
        }
    }

    while(running)
    {
        frameStart = frameEnd;
        frameEnd = SDL_GetPerformanceCounter();
        deltaTime = (double)((double)((frameEnd - frameStart) * 1000) / (double)SDL_GetPerformanceFrequency());
        deltaTime /= 1000.0;

        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT)
                running = false;


            else if(e.type == SDL_KEYDOWN)
            {
                switch(e.key.keysym.sym)
                {
                    case SDLK_1:
                        Mix_HaltMusic();
                        Mix_PlayMusic(mainTheme, -1);
                        break;
                    case SDLK_2:
                        Mix_HaltMusic();
                        Mix_PlayMusic(shopTheme, -1);
                        break;
                }
            }
        }

        player->Update(deltaTime);
        

        for(auto it = enemies.begin(); it != enemies.end(); )
        {
            Enemy* CurrentEnemy = *it;

            if (CurrentEnemy->IsNearPlayer()) {
                if(SDL_HasIntersection(CurrentEnemy->EnemyRect(), player->PlayerRect()) && player->GetCurrentHealth() > 0)
                {
                    player->TakeDamage(1);
                    canvas->SetHearts(player->GetCurrentHealth());
                }
            }
            (CurrentEnemy)->Update(deltaTime);

            if(!(CurrentEnemy)->IsAlive())
            {
                delete CurrentEnemy;
                it = enemies.erase(it);
            }
            else ++it;
        }
        

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        tilemap->Render();
        player->Render();

        for (auto e : enemies) e->Render();
        
        canvas->Render();

        SDL_RenderPresent(renderer);
        }

    free(player);
    free(tilemap);
    free(canvas);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}