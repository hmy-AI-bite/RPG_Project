#pragma once
#include "Character.h"
#include "Skill.h"
#include "Monster.h"
#include "Map.h"                     // 新增地图模块
#include <vector>
#include <memory>
#include <string>

class GameManager
{
private:
    std::vector<std::shared_ptr<Character>> characters;
    std::shared_ptr<Character> currentCharacter;
    std::vector<std::shared_ptr<Enemy>> enemies;
    Map gameMap;                     // 新增地图对象

public:
    GameManager();
    ~GameManager();

    void AddCharacter(std::shared_ptr<Character> character);
    void SetCurrentCharacter(int index);
    std::shared_ptr<Character> GetCurrentCharacter() const { return currentCharacter; }

    void AddEnemy(std::shared_ptr<Enemy> enemy);
    void DisplayEnemies() const;

    void ClearScreen() const;
    void DisplayDashboard() const;
    void DisplayMainMenu() const;
    void DisplayCharacterList() const;

    void Run();
    void ShowTitleScreen();
    void EnterGame();

private:
    void HandleAction();
    void UpdateDashboard() const;
    void DemonstrateDamageSystem();
    void PerformAttack(std::shared_ptr<Character> attacker, std::shared_ptr<Character> defender);
    void TestSlime();
    void TestATB_1v1();
    void TestSlimeSplit();
    void TestFullBattle();
    void TestAllClasses();

    // 新增：地图探索
    void ExploreMap();
    // 新增：遇敌后的简易战斗
    void BattleEncounter(Player* player, std::shared_ptr<Enemy> enemy);

    static void WaitForKeyPress();
    static void DrawProgressBar(int percent, int barLength,
        const std::string& filledChar, const std::string& emptyChar);
    static void DrawSeparator(const std::string& title = "");
};
