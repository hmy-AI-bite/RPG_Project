#pragma once
#include "Character.h"
#include "Skill.h"
#include "Monster.h"
#include <vector>
#include <memory>
#include <cstdlib>
#include <string>

// ================================================================
//  GameManager 类 - 游戏管理器
//
//  这是游戏的"总控制台"！
//  负责管理：角色列表、怪物列表、主菜单、战斗循环
//
//  就像一个导演，协调所有演员（角色和怪物）的表演
// ================================================================
class GameManager
{
private:
    // 所有角色列表（可以创建多个角色切换使用）
    std::vector<std::shared_ptr<Character>> characters;

    // 当前选中的角色（玩家正在操作的这个）
    std::shared_ptr<Character> currentCharacter;

    // 怪物列表（当前场景中的所有怪物）
    std::vector<std::shared_ptr<Enemy>> enemies;

    // 玩家昵称
    std::string playerNickname;

public:
    GameManager();
    ~GameManager();

    // ========== 角色管理 ==========

    // 添加一个新角色到队伍中
    void AddCharacter(std::shared_ptr<Character> character);

    // 切换当前操作的角色（通过索引）
    void SetCurrentCharacter(int index);

    // 获取当前操作的角色
    std::shared_ptr<Character> GetCurrentCharacter() const { return currentCharacter; }

    // ========== 怪物管理 ==========

    // 添加一个怪物到场景中
    void AddEnemy(std::shared_ptr<Enemy> enemy);

    // 显示所有怪物的信息
    void DisplayEnemies() const;

    // ========== 显示相关 ==========

    // 清屏（Windows 用 cls，Linux/Mac 用 clear）
    void ClearScreen() const;

    // 显示主控面板（角色的 HP/MP/行动条/属性）
    void DisplayDashboard() const;

    // 显示主菜单（所有可用的操作）
    void DisplayMainMenu() const;

    // 显示所有角色的列表
    void DisplayCharacterList() const;

    // ========== 游戏主界面与流程 ==========

    // 标题画面（开始游戏 / 加载存档 / 退出）
    void ShowTitleScreen();

    // 创建角色流程（选择职业 → 输入昵称 → 初始物品 → 技能学习）
    void CreateCharacterFlow();

    // 技能学习界面（学习职业技能）
    void SkillLearningFlow(Player* player);

    // 进入游戏（初始化完成后进入主循环）
    void EnterGame();

    // 开始游戏！不断显示面板→显示菜单→处理操作→循环
    void Run();

    // ========== 测试方法（公开，供 main 调用）==========

    // 测试6：元素克制快速验证（自动 PASS/FAIL）
    void TestElementSystemQuick();

    // 测试7：背包与道具快速验证（自动 PASS/FAIL）
    void TestInventoryQuick();

    // 测试8：完整战斗流程（交互式）
    void TestFullBattleQuick();

    // 初始化玩家背包（给初始道具）
    void InitPlayerInventory(Player* player);

    // 等待玩家按任意键继续（静态方法，方便外部调用）
    static void WaitForKeyPress();

private:
    // 处理玩家的菜单选择，返回 false 表示返回标题画面
    bool HandleAction();

    // 刷新主控面板
    void UpdateDashboard() const;

    // 生成怪物遭遇战（根据玩家等级随机生成怪物）
    void BattleEncounter();

    // ATB 战斗主循环（玩家 vs 怪物列表）
    // 返回 true = 胜利，false = 逃跑/失败
    bool RunBattle(std::vector<std::shared_ptr<Enemy>>& enemies);

    // 使用技能（交互式选择技能和目标）
    void UseSkillInteractively(Player* player);

    // ========== 调试/测试（保留但不在菜单显示）==========

    // 演示伤害计算系统（展示物理/魔法/真实伤害的计算过程）
    void DemonstrateDamageSystem();

    // 执行一次攻击（用于伤害演示）
    void PerformAttack(std::shared_ptr<Character> attacker, std::shared_ptr<Character> defender);

    // 测试史莱姆战斗
    void TestSlime();

    // 测试1：ATB 1v1 - 手动操作 vs 史莱姆
    void TestATB_1v1();

    // 测试2：史莱姆分裂 - 属性继承验证
    void TestSlimeSplit();

    // 测试3：完整ATB战斗 - 多敌人+技能+分裂
    void TestFullBattle();

    // 测试4：三种职业对比 - 速度差异
    void TestAllClasses();

    // ========== UI 工具方法 ==========

    // 画进度条（用于 HP/MP/行动条的可视化显示）
    //   percent    = 百分比（0~100）
    //   barLength  = 进度条长度（多少个字符）
    //   filledChar = 已填充部分的字符（比如"█"）
    //   emptyChar  = 未填充部分的字符（比如"░"）
    static void DrawProgressBar(int percent, int barLength,
        const std::string& filledChar, const std::string& emptyChar);

    // 画分隔线（带标题）
    static void DrawSeparator(const std::string& title = "");
};
