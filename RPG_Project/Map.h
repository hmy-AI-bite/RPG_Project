#pragma once
#include "Monster.h"
#include <memory>

const int MAP_WIDTH = 8;
const int MAP_HEIGHT = 8;

enum class Terrain { Plain, Forest, Town };

class Map
{
private:
    Terrain grid[MAP_HEIGHT][MAP_WIDTH];
    int playerX, playerY;

public:
    Map();
    void Display() const;
    bool Move(char direction);
    bool IsTown() const;
    std::shared_ptr<Enemy> CheckEncounter(int playerLevel); // 根据玩家等级生成敌人
    int GetX() const { return playerX; }
    int GetY() const { return playerY; }
    void SetPosition(int x, int y) { playerX = x; playerY = y; }
};