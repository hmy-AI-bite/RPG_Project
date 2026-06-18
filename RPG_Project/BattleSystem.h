#pragma once
#include <vector>
#include <memory>
#include <string>

class Character;
class Player;
class Enemy;

// ================================================================
//  BattleSystem - 战斗系统
//
//  负责：ATB 战斗逻辑、怪物遭遇、技能交互使用
// ================================================================
class BattleSystem
{
public:
    // ========== 战斗核心 ==========

    // 怪物遭遇战（根据玩家等级随机生成怪物 → ATB 战斗）
    // 返回 true = 胜利，false = 失败/逃跑
    static bool BattleEncounter(Player* player,
        std::vector<std::shared_ptr<Enemy>>& enemies);

    // ATB 战斗主循环
    // 返回 true = 胜利，false = 失败/逃跑
    static bool RunBattle(Player* player,
        std::vector<std::shared_ptr<Enemy>>& enemies);

    // ========== 技能 ==========

    // 交互式使用技能（非战斗状态）
    static void UseSkillInteractively(Player* player,
        const std::vector<std::shared_ptr<Enemy>>& enemies);

    // ========== 伤害演示 ==========

    // 执行一次攻击演示
    static void PerformAttack(std::shared_ptr<Character> attacker,
        std::shared_ptr<Character> defender);

private:
    // 战斗 UI 辅助（显示回合状态）
    static void ShowBattleStatus(Player* player,
        const std::vector<std::shared_ptr<Enemy>>& enemies, int round);

    // 处理玩家战斗输入
    static bool HandleBattleInput(Player* player,
        std::vector<std::shared_ptr<Enemy>>& enemies,
        bool& quit);

    // 处理怪物行动
    static void HandleEnemyAction(Enemy* enemy, Player* player,
        std::vector<std::shared_ptr<Enemy>>& enemies);

    // 发放战斗奖励
    static void GiveBattleReward(Player* player,
        const std::vector<std::shared_ptr<Enemy>>& enemies);
};
