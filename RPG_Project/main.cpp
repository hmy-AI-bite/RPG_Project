#include "GameManager.h"
#include "Monster.h"
#include "Skill.h"
#include <iostream>
#include <memory>

int main()
{
    // 设置控制台编码为UTF-8
    system("chcp 65001 > nul");

    GameManager gameManager;

    // 创建角色 - 三种职业平衡设计
    // 战士：均衡型，能抗能打
    auto warrior = std::make_shared<Player>("战士·剑心", 120, 20, 10, 1);
    warrior->SetPhysicalResistance(0.05);    // 物理抗性5%
    warrior->SetMagicalResistance(0.03);     // 魔法抗性3%

    // 法师：低生命、低防、高攻击、超高暴击
    auto mage = std::make_shared<Mage>("法师·火焰", 80, 28, 3, 1, 35);
    mage->SetPhysicalResistance(0.0);        // 物理抗性0%
    mage->SetMagicalResistance(0.15);        // 魔法抗性15%

    // 勇者：高攻击、低防、高暴击、真实伤害
    auto braver = std::make_shared<Braver>("勇者·破晓", 140, 35, 6, 1);
    braver->SetPhysicalResistance(0.0);      // 物理抗性0%
    braver->SetMagicalResistance(0.05);      // 魔法抗性5%

    // 添加角色到游戏管理器
    gameManager.AddCharacter(warrior);
    gameManager.AddCharacter(mage);
    gameManager.AddCharacter(braver);

    // 创建初始敌人用于测试
    auto slime1 = std::make_shared<Slime>(1);
    auto slime2 = std::make_shared<Slime>(2);
    auto slime3 = std::make_shared<Slime>(3);

    gameManager.AddEnemy(slime1);
    gameManager.AddEnemy(slime2);
    gameManager.AddEnemy(slime3);

    // 启动游戏
    gameManager.Run();

    return 0;
}