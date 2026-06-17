// ================================================================
//  RPG ATB 战斗系统 - 程序入口
//
//  这个文件只做一件事：创建游戏管理器，启动游戏主循环！
//  所有的测试功能都已经整合到 GameManager 的主菜单中了。
//
//  菜单操作指南：
//    1-9  = 基础功能（角色、攻击、治疗、技能等）
//    A    = 查看怪物列表
//    T    = 史莱姆战斗测试
//    S/L  = 保存/加载游戏
//    I    = 打开背包
//    P    = 进入商店
//    R    = 刷新面板
//    X    = 获得经验值
//    A    = 属性分配
//    M1   = ATB 1v1 测试（手动操作 vs 史莱姆）
//    M2   = 史莱姆分裂验证
//    M3   = 完整 ATB 多对多战斗
//    M4   = 三种职业速度对比
//    M5   = 元素克制测试
//    T1   = 属性克制验证（自动 PASS/FAIL）
//    T2   = 背包与道具验证（自动 PASS/FAIL）
//    T3   = 完整战斗流程（交互式）
//    0    = 退出游戏
// ================================================================

#include "GameManager.h"
#include "Character.h"
#include "Skill.h"
#include "Monster.h"
#include "DamageSystem.h"
#include "Item.h"
#include <iostream>
#include <memory>
#include <string>

// ================================================================
//  测试菜单模式 - 简洁的独立测试入口
// ================================================================
void ShowTestMenu()
{
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    RPG 系统验证测试                            ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║  测试1：属性克制验证（自动 PASS/FAIL）                           ║" << std::endl;
    std::cout << "║  测试2：背包与道具验证（自动 PASS/FAIL）                           ║" << std::endl;
    std::cout << "║  测试3：完整战斗流程（交互式）                                     ║" << std::endl;
    std::cout << "║  退出：退出程序                                                  ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "请输入你的选择: ";
}

int main()
{
    // 设置控制台编码为 UTF-8，让中文正常显示
    system("chcp 65001 > nul");

    // 创建游戏管理器（游戏总控制器）
    GameManager gameManager;

    // ========== 创建角色（预设角色用于测试）==========

    // 战士·剑心 - 均衡型，SPD=100
    auto warrior = std::make_shared<Player>("战士·剑心");
    warrior->SetPhysicalResistance(0.05);
    warrior->SetMagicalResistance(0.03);
    warrior->AddSkill(std::make_shared<PhysicalSkill>("强力挥斩", 1.5, 15, ElementType::Neutral));
    warrior->AddSkill(std::make_shared<PhysicalSkill>("破甲突刺", 1.8, 25, ElementType::Neutral));
    gameManager.InitPlayerInventory(warrior.get());
    gameManager.AddCharacter(warrior);

    // 法师·火焰 - 高速度高暴击，SPD=120
    auto mage = std::make_shared<Mage>("法师·火焰", 35);
    mage->SetPhysicalResistance(0.0);
    mage->SetMagicalResistance(0.15);
    mage->AddSkill(std::make_shared<MagicalSkill>("火焰术", 1.8, 30, 35, ElementType::Fire));
    gameManager.InitPlayerInventory(mage.get());
    gameManager.AddCharacter(mage);

    // 勇者·破晓 - 高攻击中速，SPD=110
    auto braver = std::make_shared<Braver>("勇者·破晓");
    braver->SetPhysicalResistance(0.0);
    braver->SetMagicalResistance(0.05);
    braver->AddSkill(std::make_shared<TrueSkill>("刺客之刃", 30, 20));
    gameManager.InitPlayerInventory(braver.get());
    gameManager.AddCharacter(braver);

    // 骑士·守护 - 坦克定位，高血量高防御低速，SPD=80
    auto guardian = std::make_shared<Guardian>("骑士·守护");
    guardian->SetPhysicalResistance(0.1);
    guardian->SetMagicalResistance(0.1);
    gameManager.InitPlayerInventory(guardian.get());
    gameManager.AddCharacter(guardian);

    // ========== 测试菜单循环 ==========
    bool running = true;
    while (running)
    {
        gameManager.ClearScreen();
        ShowTestMenu();

        std::string input;
        std::cin >> input;
        std::cin.ignore();

        if (input == "1" || input == "测试1")
        {
            gameManager.TestElementSystemQuick();
        }
        else if (input == "2" || input == "测试2")
        {
            gameManager.TestInventoryQuick();
        }
        else if (input == "3" || input == "测试3")
        {
            gameManager.TestFullBattleQuick();
        }
        else if (input == "退出" || input == "exit" || input == "quit")
        {
            running = false;
        }
        else if (input == "run" || input == "游戏")
        {
            // 进入完整游戏主循环
            gameManager.Run();
            running = false;  // Run() 内部用 exit(0) 退出
        }
        else
        {
            std::cout << "无效选择，请重新输入！" << std::endl;
            std::cout << "提示：输入 1/2/3 选择测试，输入\"退出\"离开，" << std::endl;
            std::cout << "      输入\"游戏\"进入完整游戏模式。" << std::endl;
            GameManager::WaitForKeyPress();
        }
    }

    std::cout << "感谢测试！再见！" << std::endl;
    return 0;
}
