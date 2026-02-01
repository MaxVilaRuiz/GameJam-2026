#include "global.hpp"
#include "../entities/enemy.hpp"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

SDL_Rect displayBounds = {0, 0, 0, 0};

bool running = true;

SDL_GameController* controller = nullptr;

Mix_Music* mainTheme = nullptr;
Mix_Music* shopTheme = nullptr;

std::vector<std::vector<int>> map{};
std::pair<int, int> playerPos{0, 0};

std::pair<int, int> TILE_SIZE{0, 0};

std::vector<Enemy*> enemies{};