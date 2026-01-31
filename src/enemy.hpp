#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <queue>

#include "global.hpp"

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
            if(ValidPosition(d) && !visited[d.second][d.first])
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