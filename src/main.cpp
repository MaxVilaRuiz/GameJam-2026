#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>

const int WINDOW_HEIGHT = 1080;
const int WINDOW_WIDTH = 1920;

SDL_Window* window;
SDL_Renderer* renderer;

SDL_Rect displayBounds;

bool running = true;

SDL_GameController* controller = nullptr;

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

    std::vector<int> maskLvl = {0, 0, 0, 0};
    int primaryMask = -1;
    int secondaryMask = -1;

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

    }

    void SecondaryAttack()
    {

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
    }

    void Update(double deltaTime)
    {
        Movement(deltaTime);
        Aim();

        if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            PrimaryAttack();
        }

        if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))
        {
            SecondaryAttack();
        }
    }

    void Render()
    {
        if(aimTargetReady) SDL_RenderCopy(renderer, aim_target, NULL, &aimTargetRect);
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

        std::cout << displayBounds.w << ", " << displayBounds.h << std::endl;
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

class Enemy
{
private:
    SDL_Texture* texture;
    SDL_Rect destRect;

public:
    Enemy()
    {
        SDL_Surface* temp = IMG_Load("../assets/zombie_mask_down.png");
        texture = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);

        destRect = {100, 100, 128, 128};
    }

    void BFS()
    {

    }

    void Update(double deltaTime)
    {

    }

    void Render()
    {
        SDL_RenderCopy(renderer, texture, NULL, &destRect);
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

    SDL_GetRendererOutputSize(renderer, &displayBounds.w, &displayBounds.h);

    Uint64 frameStart, frameEnd;
    double deltaTime;
    frameStart = 0;
    frameEnd = SDL_GetPerformanceCounter();
    deltaTime = 0.0;

    Player* player = new Player();
    Tilemap* tilemap = new Tilemap();

    Enemy* enemy = new Enemy();

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
        }

        player->Update(deltaTime);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        tilemap->Render();
        player->Render();
        enemy->Render();

        SDL_RenderPresent(renderer);
    }

    free(player);
    free(tilemap);
    free(enemy);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}