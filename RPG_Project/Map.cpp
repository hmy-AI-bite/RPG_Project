#include "Map.h"
#include <iostream>
#include <cstdlib>

Map::Map() : playerX(0), playerY(0)
{
    for (int y = 0; y < MAP_HEIGHT; ++y)
        for (int x = 0; x < MAP_WIDTH; ++x)
            grid[y][x] = Terrain::Plain;

    grid[0][0] = Terrain::Town;
    grid[0][2] = Terrain::Town;
    grid[2][5] = Terrain::Town;
    grid[5][3] = Terrain::Town;
    grid[7][7] = Terrain::Town;

    grid[1][1] = Terrain::Forest;
    grid[2][2] = Terrain::Forest;
    grid[3][4] = Terrain::Forest;
    grid[5][1] = Terrain::Forest;
    grid[6][5] = Terrain::Forest;
}

void Map::Display() const
{
    std::cout << "  ";
    for (int x = 0; x < MAP_WIDTH; ++x) std::cout << x << " ";
    std::cout << std::endl;

    for (int y = 0; y < MAP_HEIGHT; ++y)
    {
        std::cout << y << " ";
        for (int x = 0; x < MAP_WIDTH; ++x)
        {
            if (x == playerX && y == playerY)
                std::cout << "P ";
            else
            {
                char c = '.';
                switch (grid[y][x])
                {
                case Terrain::Plain: c = '.'; break;
                case Terrain::Forest: c = 'F'; break;
                case Terrain::Town: c = 'T'; break;
                }
                std::cout << c << " ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << "位置: (" << playerX << "," << playerY << ") ";
    if (IsTown()) std::cout << "[城镇]";
    std::cout << std::endl;
}

bool Map::Move(char direction)
{
    int newX = playerX, newY = playerY;
    switch (direction)
    {
    case 'w': newY--; break;
    case 's': newY++; break;
    case 'a': newX--; break;
    case 'd': newX++; break;
    default: return false;
    }
    if (newX < 0 || newX >= MAP_WIDTH || newY < 0 || newY >= MAP_HEIGHT)
        return false;
    playerX = newX;
    playerY = newY;
    return true;
}

bool Map::IsTown() const
{
    return grid[playerY][playerX] == Terrain::Town;
}

std::shared_ptr<Enemy> Map::CheckEncounter(int playerLevel)
{
    if (IsTown()) return nullptr;

    int baseRate = (grid[playerY][playerX] == Terrain::Forest) ? 40 : 25;
    if (rand() % 100 >= baseRate) return nullptr;

    int level = rand() % 3 + playerLevel - 1;  // 等级范围：playerLevel-1 到 playerLevel+1
    if (level < 1) level = 1;

    int type = rand() % 4;
    switch (type)
    {
    case 0: return std::make_shared<Slime>(level);
    case 1: return std::make_shared<Goblin>(level);
    case 2: return std::make_shared<Skeleton>(level);
    case 3: return std::make_shared<DarkMage>(level);
    }
    return nullptr;
}