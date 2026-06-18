#pragma once
#include "Character.h"
#include "Skill.h"
#include "Monster.h"
#include <vector>
#include <memory>
#include <string>

// ================================================================
//  GameManager 类 - 游戏管理器（精简版）
//
//  职责：角色/怪物管理、主循环、菜单路由分发
//  UI 显示 → UIManager
//  战斗系统 → BattleSystem
//  测试功能 → TestManager
// ================================================================
class GameManager
{
public:
    GameManager();
    ~GameManager();

    // ========== 角色管理 ==========
    void AddCharacter(std::shared_ptr<Character> character);
    void SetCurrentCharacter(int index);
    std::shared_ptr<Character> GetCurrentCharacter() const { return currentCharacter; }

    // ========== 怪物管理 ==========
    void AddEnemy(std::shared_ptr<Enemy> enemy);
    void DisplayEnemies() const;

    // ========== 游戏主界面与流程 ==========
    void ShowTitleScreen();
    void EnterGame();
    void Run();

    // ========== 测试入口（公开，供 main 调用）==========
    void TestElementSystemQuick();
    void TestInventoryQuick();
    void TestFullBattleQuick();

    // ========== 工具 ==========
    static void WaitForKeyPress();

private:
    // 处理玩家的菜单选择，返回 false 表示返回标题画面
    bool HandleAction();

    // 属性分配界面
    void AllocateAttributes(Player* player);

    // 背包界面
    void OpenInventory(Player* player);

    // 商店界面
    void OpenShop(Player* player);

    // 角色详情界面
    void ShowCharacterDetail() const;

    // ========== 成员变量 ==========
    std::vector<std::shared_ptr<Character>> characters;
    std::shared_ptr<Character> currentCharacter;
    std::vector<std::shared_ptr<Enemy>> enemies;
    std::string playerNickname;
};
