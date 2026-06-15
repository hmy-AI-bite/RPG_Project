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
//    M1   = ATB 1v1 测试（手动操作 vs 史莱姆）
//    M2   = 史莱姆分裂验证
//    M3   = 完整 ATB 多对多战斗
//    M4   = 三种职业速度对比
//    0    = 退出游戏
// ================================================================

#include "GameManager.h"
#include "Character.h"
#include "Skill.h"
#include "Monster.h"
#include "DamageSystem.h"
#include <iostream>
#include <memory>

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
    gameManager.AddCharacter(warrior);

    // 法师·火焰 - 高速度高暴击，SPD=120
    auto mage = std::make_shared<Mage>("法师·火焰", 35);
    mage->SetPhysicalResistance(0.0);
    mage->SetMagicalResistance(0.15);
    mage->AddSkill(std::make_shared<MagicalSkill>("火焰术", 1.8, 30, 35, ElementType::Fire));
    gameManager.AddCharacter(mage);

    // 勇者·破晓 - 高攻击中速，SPD=110
    auto braver = std::make_shared<Braver>("勇者·破晓");
    braver->SetPhysicalResistance(0.0);
    braver->SetMagicalResistance(0.05);
    braver->AddSkill(std::make_shared<TrueSkill>("刺客之刃", 30, 20));
    gameManager.AddCharacter(braver);

    // ========== 启动游戏主循环 ==========
    // 之后的所有操作都由 GameManager 的菜单控制
    gameManager.Run();

    return 0;
}
