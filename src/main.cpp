#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

const int WINDOW_HEIGHT = 1080;
const int WINDOW_WIDTH = 1920;

SDL_Window* window;
SDL_Renderer* renderer;

bool running = true;

SDL_GameController* controller = nullptr;

class Player
{
private:
    SDL_Texture* texture;
    SDL_Rect destRect;

    float posX, posY;
    const float speed = 250.0f;

public:
    Player()
    {
        SDL_Surface* temp = IMG_Load("../assets/character.png");
        texture = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);

        destRect = {0, 0, 64, 64};
        posX = (float)destRect.x;
        posY = (float)destRect.y;
    }

    void Update(double deltaTime)
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

        if(controller)
        {
            Sint16 lx = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
            Sint16 ly = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);

            float deadzone = 8000.0f;
            if(abs(lx) > deadzone || abs(ly) > deadzone)
            {
                fx = (abs(lx) > deadzone) ? lx / 32767.0f : 0.0f;
                fy = (abs(ly) > deadzone) ? ly / 32767.0f : 0.0f;
            }
        }

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

    void Render()
    {
        SDL_RenderCopy(renderer, texture, NULL, &destRect);
    }
};

class Tilemap
{
private:
    SDL_Texture* tilemap_tex;
    SDL_Rect srcRect = {0, 0, 16, 16}; // Choose tile
    SDL_Rect destRect = {0, 0, 64, 64};

public:
    Tilemap()
    {
        SDL_Surface* temp = IMG_Load("../assets/Overworld.png");
        tilemap_tex = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);
    }

    void Render()
    {
        for(int i = 0; i < 15; i++) {
            for(int j = 0; j < 23; j++)
            {
                SDL_RenderCopy(renderer, tilemap_tex, &srcRect, &destRect);
                destRect.x += 64;
            }
            destRect.x = 0;
            destRect.y += 64;
        }

        destRect.x = 0;
        destRect.y = 0;
    }
};

int main()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr << "SDL_Init failed" << std::endl;
        return 1;
    }

    window = SDL_CreateWindow("MASK", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);

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

    Uint64 frameStart, frameEnd;
    double deltaTime;
    frameStart = 0;
    frameEnd = SDL_GetPerformanceCounter();
    deltaTime = 0.0;

    Player* player = new Player();
    Tilemap* tilemap = new Tilemap();

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

        SDL_RenderPresent(renderer);
    }

    free(player);
    free(tilemap);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}