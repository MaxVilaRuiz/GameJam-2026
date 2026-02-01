#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <queue>
#include <stdio.h>

#include "./entities/enemy.hpp"
#include "./globals/global.hpp"
#include "./attacks/earth/earth_attack1.hpp"
#include "./attacks/earth/earth_attack2.hpp"
#include "./attacks/fire/fire_attack1.hpp"
#include "./entities/player.hpp"
#include "./utils/tile_map.hpp"
#include "./utils/ui.hpp"


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