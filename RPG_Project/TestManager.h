#pragma once
#include <vector>
#include <memory>

class Character;
class Player;
class Enemy;

// ================================================================
//  TestManager - 测试管理器
//
//  负责所有测试/调试功能的入口
// ================================================================
class TestManager
{
public:
    // 测试6：元素克制快速验证（自动 PASS/FAIL）
    static void TestElementSystemQuick();

    // 测试7：背包与道具快速验证（自动 PASS/FAIL）
    static void TestInventoryQuick();

    // 测试8：完整战斗流程（交互式）
    static void TestFullBattleQuick();

    // ========== 旧测试（保留） ==========
    static void TestSlime();
    static void TestATB_1v1();
    static void TestSlimeSplit();
    static void TestFullBattle();
    static void TestAllClasses();
    static void DemonstrateDamageSystem();

    // 初始化玩家背包
    static void InitPlayerInventory(Player* player);

private:
    static void ExecuteAttack(std::shared_ptr<Character> attacker,
        std::shared_ptr<Character> defender);
};
