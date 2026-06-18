#pragma once
#include <string>
#include <vector>
#include <memory>

class Character;
class Player;
class Enemy;

// ================================================================
//  UIManager - UI 显示管理器
//
//  负责所有界面显示：面板、菜单、角色列表、怪物列表、
//  标题画面、创建角色、技能学习等
// ================================================================
class UIManager
{
public:
    // ========== 工具方法 ==========

    // 等待玩家按任意键继续
    static void WaitForKeyPress();

    // 清屏
    static void ClearScreen();

    // 画进度条
    static void DrawProgressBar(int percent, int barLength,
        const std::string& filledChar, const std::string& emptyChar);

    // 画分隔线（带标题）
    static void DrawSeparator(const std::string& title = "");

    // ========== 显示界面 ==========

    // 显示主控面板
    static void DisplayDashboard(const std::shared_ptr<Character>& currentCharacter);

    // 显示主菜单
    static void DisplayMainMenu();

    // 显示角色列表
    static void DisplayCharacterList(
        const std::vector<std::shared_ptr<Character>>& characters,
        const std::shared_ptr<Character>& currentCharacter);

    // 显示怪物列表
    static void DisplayEnemies(const std::vector<std::shared_ptr<Enemy>>& enemies);

    // 显示标题画面，返回玩家选择（'1'/'2'/'0'）
    static char ShowTitleScreen();

    // ========== 创建角色流程 ==========

    // 返回创建好的玩家（调用方负责 AddCharacter）
    static std::shared_ptr<Player> CreateCharacterFlow();

    // 技能学习流程
    static void SkillLearningFlow(Player* player);

    // 进入游戏过渡画面
    static void EnterGameScreen(const std::shared_ptr<Character>& currentCharacter);
};
