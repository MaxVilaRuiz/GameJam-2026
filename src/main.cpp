#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include "./entities/enemy.hpp"
#include "./globals/global.hpp"
#include "./attacks/earth_attack.hpp"
#include "./entities/player.hpp"
#include "./utils/tile_map.hpp"


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