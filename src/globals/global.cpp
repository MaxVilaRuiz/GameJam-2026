#include "global.hpp"
// #include "../entities/enemy.hpp"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

bool running = true;

SDL_GameController* controller = nullptr;

Mix_Music* mainTheme = nullptr;
Mix_Music* shopTheme = nullptr;