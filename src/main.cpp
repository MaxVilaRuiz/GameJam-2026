#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <queue>
#include <stdio.h>

SDL_Window* window;
SDL_Renderer* renderer;

SDL_Rect displayBounds;
std::pair<int, int> TILE_SIZE;

bool running = true;

SDL_GameController* controller = nullptr;

Mix_Music* mainTheme;
Mix_Music* shopTheme;

std::vector<std::vector<int>> map;
std::pair<int, int> playerPos;

class Enemy
{
private:
    SDL_Texture* texture;
    SDL_Rect destRect;

    float posX, posY;

    bool damagePlayer = false;
    bool inPlayerRange = false;

    float damageTime = 0.0f;
    const float DAMAGE_TIME_TOTAL = 0.2f;

    const float speed = 100.0f;

    int health;

    std::vector<std::pair<int, int>> Directions(std::pair<int, int> cpos)
    {
        return {{cpos.first + 1, cpos.second}, {cpos.first + 1, cpos.second + 1}, {cpos.first, cpos.second + 1}, {cpos.first - 1, cpos.second + 1},
                {cpos.first - 1, cpos.second}, {cpos.first - 1, cpos.second - 1}, {cpos.first, cpos.second - 1}, {cpos.first + 1, cpos.second - 1}};
    }

    bool ValidPosition(std::pair<int, int> p)
    {
        return (p.first >= 0 && p.first < 24 && p.second >= 0 && p.second < 16);
    }

public:
    Enemy()
    {
        SDL_Surface* temp = IMG_Load("../assets/zombie_mask_down.png");
        texture = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);

        destRect = {100, 100, 128, 128};
        posX = (float)destRect.x;
        posY = (float)destRect.y;
        health = 4;
    }

    const SDL_Rect* EnemyRect()
    {
        return &destRect;
    }

    bool IsNearPlayer()
    {
        return damagePlayer;
    }

    std::pair<float, float> BacktrackPath(std::vector<std::vector<std::pair<int, int>>>& parent, std::pair<int, int> start, std::pair<int, int> goal)
    {
        std::pair<int, int> curr = goal;
        if(curr == start)
        {
            damagePlayer = true;
            return {0.0f, 0.0f};
        }
        else damagePlayer = false;

        while(!(parent[curr.second][curr.first] == start))
        {
            curr = parent[curr.second][curr.first];
        }

        float dx = curr.first - start.first;
        float dy = curr.second - start.second;
        float mag = sqrtf(dx * dx + dy * dy);
        if(mag <= 0.0f) return {0.0f, 0.0f};
        return {dx / mag, dy / mag};
    }

    std::pair<float, float> BFS() {
    int tileW = displayBounds.w / 24;
    int tileH = displayBounds.h / 16;
    
    if (tileW == 0 || tileH == 0) return {0.0f, 0.0f};

    int startX = (destRect.x + destRect.w / 2) / tileW;
    int startY = (destRect.y + destRect.h / 2) / tileH;

    if (startX < 0 || startX >= 24 || startY < 0 || startY >= 16) {
        return {0.0f, 0.0f}; // Se queda quieto si está fuera del mapa
    }

    std::vector<std::vector<bool>> visited(16, std::vector<bool>(24, false));

    visited[startY][startX] = true; 
    
    std::vector<std::vector<std::pair<int, int>>> parent(16, std::vector<std::pair<int, int>>(24, {startX, startY}));

    std::queue<std::pair<int, int>> q;
    q.push({startX, startY});

    std::pair<int, int> goal = {-1, -1};
    bool found = false;
    
    while(!q.empty())
    {
        std::pair<int, int> c = q.front();
        q.pop();

        if(map[c.second][c.first])
        {
            found = true;
            goal = c;
            break;
        }

        for(std::pair<int, int>& d : Directions(c))
        {
            if(ValidPosition(d) && !visited[d.second][d.first] && map[d.second][d.first] != 2)
            {
                visited[d.second][d.first] = true;
                parent[d.second][d.first] = c;
                q.push(d);
            }
        }
    }

    if(found) return BacktrackPath(parent, {startX, startY}, goal);
    else
    {
        damagePlayer = true;
        return {0.0, 0.0};
    }
}

    void TakeDamage(int amount)
    {
        health -= amount;
        damageTime = DAMAGE_TIME_TOTAL;
    }

    bool IsAlive() const { return health > 0; }
    bool InPlayerRange() const { return inPlayerRange; }

    void Update(double deltaTime)
    {
        if(damageTime > 0.0f)
        {
            damageTime -= deltaTime;
        }
        else
        {
            std::pair<float, float> dir = BFS();

            posX += dir.first * speed * deltaTime;
            posY += dir.second * speed * deltaTime;

            destRect.x = (int)posX;
            destRect.y = (int)posY;
        }

        int dx = destRect.x - playerPos.first;
        int dy = destRect.y - playerPos.second;
        float mag = sqrtf(dx * dx + dy * dy);
        if(mag <= 400) inPlayerRange = true;
        else inPlayerRange = false;
    }

    void Render()
    {
        if(damageTime > 0.0f) SDL_SetTextureColorMod(texture, 255, 100, 100);
        else SDL_SetTextureColorMod(texture, 255, 255, 255);
        SDL_RenderCopy(renderer, texture, NULL, &destRect);
    }
};


std::vector<Enemy*> enemies;

class FireAttack1
{
private:
    SDL_Texture* texture;
    SDL_Rect rect;
public:
    FireAttack1(SDL_Rect spawnRect, SDL_Rect DirectionRect)
    {
        SDL_Surface* temp = IMG_Load("../assets/p_fire_f1.png");
        texture = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);
        rect = spawnRect;
    }

    ~FireAttack1()
    {
        if(texture) SDL_DestroyTexture(texture);
    }

    bool IsAlive() const { return rect.x < -10 | rect.y < -10 | rect.x > 2250 | rect.y > 1250; } 

    void Update()
    {
        
    }
};

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

class EarthAttack2
{
private:
    SDL_Texture* texture;
    SDL_Rect rect;


    float BASE_DURATION = 10.0f;
    float currentTime;

    int tileX0, tileX1, tileY0, tileY1;
    
public:
    EarthAttack2(SDL_Rect spawnRect)
    {
        SDL_Surface* temp = IMG_Load("../assets/rock.png");
        texture = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);
        rect = spawnRect;
        currentTime = BASE_DURATION;

        int tileX0 = rect.x / TILE_SIZE.first;
        int tileY0 = rect.y / TILE_SIZE.second;
        int tileX1 = (rect.x + rect.w - 1) / TILE_SIZE.first;
        int tileY1 = (rect.y + rect.h - 1) / TILE_SIZE.second;
        tileX0 = SDL_clamp(tileX0, 0, 23);
        tileX1 = SDL_clamp(tileX1, 0, 23);
        tileY0 = SDL_clamp(tileY0, 0, 15);
        tileY1 = SDL_clamp(tileY1, 0, 15);

        for(int ty = tileY0; ty <= tileY1; ty++)
        {
            for(int tx = tileX0; tx <= tileX1; tx++)
            {
                map[ty][tx] = 2;
            }
        }
        
        for (int i = 0; i < enemies.size(); ++i) {
            if (enemies[i]->InPlayerRange()) {
                if (SDL_HasIntersection(enemies[i]->EnemyRect(), &rect)) {
                    enemies[i]->TakeDamage(2); // Change??
                }
            }
        }
    }

    ~EarthAttack2()
    {
        if(texture) SDL_DestroyTexture(texture);
        for(int ty = tileY0; ty <= tileY1; ty++)
        {
            for(int tx = tileX0; tx <= tileX1; tx++)
            {
                map[ty][tx] = 0;
            }
        }
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
    int secondaryMask = -1;

    std::vector<FireAttack1*> attacksFire;
    std::vector<EarthAttack1*> ePrimaryAttacks;
    std::vector<EarthAttack2*> eSecondaryAttacks;
    float primaryCooldownEarth = 0.0f;
    float PRIMARY_COOLDOWN_TIME_EARTH = 0.4f;
    float primaryCooldownFire = 0.0f;
    float PRIMARY_COOLDOWN_TIME_FIRE = 0.4f;
    float primaryCooldownAir = 0.0f;
    float PRIMARY_COOLDOWN_TIME_AIR = 0.4f;
    float primaryCooldownWater = 0.0f;
    float PRIMARY_COOLDOWN_TIME_WATER = 0.4f;

    float secondaryCooldownEarth = 0.0f;
    float SECONDARY_COOLDOWN_TIME_EARTH = 5.0f;

    float maskSwitchCooldown = 0.0f;
    float MASK_SWITCH_COOLDOWN_TIME = 1.0f;

    const int offsetX = 40;
    const int offsetY = 0;

    std::vector<SDL_Texture*> maskSprites()
    {
        std::vector<SDL_Texture*> sprites(4);
        std::vector<char*> names = {"aire 1.png", "awa 1.png", "foc 1.png", "terra 1.png"};

        for(int i = 0; i < 4; i++)
        {
            char* name = names[i];
            char namebuffer[128];
            sprintf(namebuffer, "../assets/%s", name);
            SDL_Surface* temp = IMG_Load(namebuffer);
            sprites[i] = SDL_CreateTextureFromSurface(renderer, temp);
            SDL_FreeSurface(temp);
        }
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
            
            ePrimaryAttacks.push_back(new EarthAttack1({aimTargetRect.x - (esize - 64)/2, aimTargetRect.y - (esize - 64)/2 , esize, esize}));
            primaryCooldownEarth = PRIMARY_COOLDOWN_TIME_EARTH;
        }
        else if (primaryMask == 1 && primaryCooldownFire <= 0.0f && aimTargetReady){

            int fsize = 16;
            if(maskLvl[0] >=3) fsize += 10;
            
            attacksFire.push_back(new FireAttack1({destRect.x, destRect.y , fsize, fsize}, aimTargetRect));
            primaryCooldownFire = PRIMARY_COOLDOWN_TIME_FIRE;

        }
    }

    void SecondaryAttack()
    {
        if(primaryMask == 0 && secondaryCooldownEarth <= 0.0f && aimTargetReady)
        {
            int esize = 128;
            eSecondaryAttacks.push_back(new EarthAttack2({aimTargetRect.x - (esize / 2), aimTargetRect.y - (esize / 2), esize, esize}));
            secondaryCooldownEarth = SECONDARY_COOLDOWN_TIME_EARTH;
        }
    }

public:
    Player()
    {
        SDL_Surface* temp = IMG_Load("../assets/character.png");
        texture = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("../assets/aim.png");
        aim_target = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);

        aimTargetRect = {0, 0, 64, 64};
        destRect = {0, 0, 64, 64};
        posX = (float)destRect.x;
        posY = (float)destRect.y;

        currentHealth = maxHealth;
    }

    ~Player()
    {
        if(texture) SDL_DestroyTexture(texture);
        if(aim_target) SDL_DestroyTexture(aim_target);
        for(auto* a : ePrimaryAttacks) delete a;
        ePrimaryAttacks.clear();
        for(auto* a : eSecondaryAttacks) delete a;
        eSecondaryAttacks.clear();
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

    float GetSecondaryCooldown()
    {
        return secondaryCooldownEarth;
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
        if(secondaryCooldownEarth > 0.0f) secondaryCooldownEarth -= deltaTime;
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
    }

    void Render()
    {
        if(aimTargetReady) SDL_RenderCopy(renderer, aim_target, NULL, &aimTargetRect);
        for(auto* a : ePrimaryAttacks) a->Render();
        for(auto* a : eSecondaryAttacks) a->Render();
        
        if(damageCooldown > 0.0f)
        {
            SDL_SetTextureColorMod(texture, 255, 100, 100);
        }
        else
        {
            SDL_SetTextureColorMod(texture, 255, 255, 255);
        }

        SDL_RenderCopy(renderer, texture, NULL, &destRect);
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

        TILE_SIZE.first = displayBounds.w / 24;
        TILE_SIZE.second = displayBounds.h / 24;
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

        temp = IMG_Load("../assets/icon_atm_cont.png");
        icon_mainatt_cont = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);

        temp = IMG_Load("../assets/icon_ats_cont.png");
        icon_secatt_cont = SDL_CreateTextureFromSurface(renderer, temp);
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

        if(!controller)
        {
            if(player->GetPrimaryCooldown() > 0.0f) SDL_SetTextureAlphaMod(icon_mainatt_kbm, 100);
            else SDL_SetTextureAlphaMod(icon_mainatt_kbm, 255);
            SDL_RenderCopy(renderer, icon_mainatt_kbm, NULL, &mainAttRect);

            if(player->GetSecondaryCooldown() > 0.0f) SDL_SetTextureAlphaMod(icon_secatt_kbm, 100);
            else SDL_SetTextureAlphaMod(icon_secatt_kbm, 255);
            SDL_RenderCopy(renderer, icon_secatt_kbm, NULL, &secAttRect);
        }
        else
        {
            if(player->GetPrimaryCooldown() > 0.0f) SDL_SetTextureAlphaMod(icon_mainatt_cont, 100);
            else SDL_SetTextureAlphaMod(icon_mainatt_cont, 255);
            SDL_RenderCopy(renderer, icon_mainatt_cont, NULL, &mainAttRect);

            if(player->GetSecondaryCooldown() > 0.0f) SDL_SetTextureAlphaMod(icon_secatt_cont, 100);
            else SDL_SetTextureAlphaMod(icon_secatt_cont, 255);
            SDL_RenderCopy(renderer, icon_secatt_cont, NULL, &secAttRect);
        }
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