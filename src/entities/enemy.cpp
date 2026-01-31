#include "enemy.hpp"

// Constructor
Enemy::Enemy() :
    // Initialize private variables
    damagePlayer(false),
    inPlayerRange(false),
    damageTime(0.0f)
{
    destRect = {100, 100, 128, 128};
    posX = (float)destRect.x;
    posY = (float)destRect.y;
    health = 4;

    sprites = std::vector<SDL_Texture*>(8);
    std::vector<std::string> names = {"zombie_mask_right", "zombie_mask_dright", "zombie_mask_down", "zombie_mask_dleft",
        "zombie_mask_down", "zombie_mask_down", "zombie_mask_down", "zombie_mask_down"};
    for(int i = 0; i < 8; i++)
    {
        SDL_Surface* temp = IMG_Load("../assets/zombie_mask_down.png");
        texture = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);
    }
}


// Private functions
std::vector<std::pair<int, int>> Enemy::Directions(std::pair<int, int> cpos)
{
    return {{cpos.first + 1, cpos.second}, {cpos.first + 1, cpos.second + 1}, {cpos.first, cpos.second + 1}, {cpos.first - 1, cpos.second + 1},
            {cpos.first - 1, cpos.second}, {cpos.first - 1, cpos.second - 1}, {cpos.first, cpos.second - 1}, {cpos.first + 1, cpos.second - 1}};
}

bool Enemy::ValidPosition(std::pair<int, int> p)
{
    return (p.first >= 0 && p.first < 24 && p.second >= 0 && p.second < 16);
}


// Public functions
const SDL_Rect* Enemy::EnemyRect()
{
    return &destRect;
}

bool Enemy::IsNearPlayer()
{
    return damagePlayer;
}

std::pair<float, float> Enemy::BacktrackPath(std::vector<std::vector<std::pair<int, int>>>& parent, std::pair<int, int> start, std::pair<int, int> goal)
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

std::pair<float, float> Enemy::BFS() 
{
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

void Enemy::TakeDamage(int amount)
{
    health -= amount;
    damageTime = DAMAGE_TIME_TOTAL;
}

bool Enemy::IsAlive() const { 
    return health > 0; 
}
bool Enemy::InPlayerRange() const { 
    return inPlayerRange; 
}

void Enemy::Update(double deltaTime)
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

        float angle = atan2f(dir.second, dir.first);
        if(angle < 0.0) angle += 2.0 * M_PI;
        int dirIndex = (int)((angle + M_PI / 8) / (M_PI / 4)) % 8;
    }

    int dx = destRect.x - playerPos.first;
    int dy = destRect.y - playerPos.second;
    float mag = sqrtf(dx * dx + dy * dy);
    if(mag <= 400) inPlayerRange = true;
    else inPlayerRange = false;
}

void Enemy::Render()
{
    if(damageTime > 0.0f) SDL_SetTextureColorMod(texture, 255, 100, 100);
    else SDL_SetTextureColorMod(texture, 255, 255, 255);
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
}