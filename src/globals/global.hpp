#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <queue>
#include <string>

// class Enemy;

extern SDL_Window* window;
extern SDL_Renderer* renderer;

extern SDL_Rect displayBounds;

extern bool running;

extern SDL_GameController* controller;

extern Mix_Music* mainTheme;
extern Mix_Music* shopTheme;

extern std::vector<std::vector<int>> map;
extern std::pair<int, int> playerPos;

extern std::pair<int, int> TILE_SIZE;

// extern std::vector<Enemy*> enemies;