#include "GameManager.h"
#include <iostream>

int main()
{
    GameManager gameManager;

    // 创建角色
    auto player1 = std::make_shared<Player>("剑士", 120, 25, 8, 1);
    auto mage = std::make_shared<Mage>("法师", 100, 15, 5, 1, 30);
    auto braver = std::make_shared<Braver>("勇敢者", 150, 30, 10, 1);

    // 添加角色到游戏管理器
    gameManager.AddCharacter(player1);
    gameManager.AddCharacter(mage);
    gameManager.AddCharacter(braver);

    // 启动游戏
    gameManager.Run();

    return 0;
}