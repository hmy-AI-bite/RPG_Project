#pragma once
#include "Character.h"
#include "Skill.h"
#include "Monster.h"
#include <vector>
#include <memory>
#include <cstdlib>

class GameManager
{
private:
    std::vector<std::shared_ptr<Character>> characters;
    std::shared_ptr<Character> currentCharacter;
    std::vector<std::shared_ptr<Enemy>> enemies;

public:
    GameManager();
    ~GameManager();

    // 角色管理
    void AddCharacter(std::shared_ptr<Character> character);
    void SetCurrentCharacter(int index);
    std::shared_ptr<Character> GetCurrentCharacter() const { return currentCharacter; }

    // 怪物管理
    void AddEnemy(std::shared_ptr<Enemy> enemy);
    void DisplayEnemies() const;

    // 显示相关
    void ClearScreen() const;
    void DisplayDashboard() const;
    void DisplayMainMenu() const;
    void DisplayCharacterList() const;

    // 游戏循环
    void Run();

private:
    void HandleAction();
    void UpdateDashboard() const;
    void DemonstrateDamageSystem();
    void PerformAttack(std::shared_ptr<Character> attacker, std::shared_ptr<Character> defender);
    void TestSlime();  // 测试史莱姆
};
