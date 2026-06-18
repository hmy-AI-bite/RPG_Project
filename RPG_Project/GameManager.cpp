#include "GameManager.h"
#include "SaveLoadManager.h"
#include "Shop.h"
#include "DamageSystem.h"
#include "Map.h"
#include "UIManager.h"
#include "TestManager.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include "ATBTest.h"

// ================================================================
//  UI 工具方法
// ================================================================

void GameManager::WaitForKeyPress()
{
    std::cout << "按任意键继续...";
    std::cin.ignore();
    std::cin.get();
}

void GameManager::DrawProgressBar(int percent, int barLength,
    const std::string& filledChar, const std::string& emptyChar)
{
    int filled = (percent * barLength) / 100;
    for (int i = 0; i < barLength; ++i)
        std::cout << (i < filled ? filledChar : emptyChar);
}

void GameManager::DrawSeparator(const std::string& title)
{
    std::cout << "\n============================================================\n";
    if (!title.empty())
        std::cout << "  " << title << "\n";
    std::cout << "============================================================\n";
}

// ================================================================
//  构造函数与角色/怪物管理
// ================================================================

GameManager::GameManager() : currentCharacter(nullptr)
{
}

GameManager::~GameManager()
{
}

void GameManager::AddCharacter(std::shared_ptr<Character> character)
{
    if (character)
    {
        characters.push_back(character);
        if (!currentCharacter)
            currentCharacter = character;
    }
}

void GameManager::AddEnemy(std::shared_ptr<Enemy> enemy)
{
    if (enemy)
    {
        enemies.push_back(enemy);
    }
}

void GameManager::SetCurrentCharacter(int index)
{
    if (index >= 0 && index < static_cast<int>(characters.size()))
        currentCharacter = characters[index];
}

void GameManager::ClearScreen() const
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ================================================================
//  主控面板
// ================================================================
void GameManager::DisplayDashboard() const
{
    ClearScreen();
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    RPG 游戏 - 主控面板                          ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    if (currentCharacter)
    {
        std::cout << "┌─ 当前角色信息 ─────────────────────────────────────────────┐" << std::endl;
        std::cout << "│ " << std::setw(60) << std::left
            << currentCharacter->GetName() + " [LV." + std::to_string(currentCharacter->GetLevel()) + "]"
            << "│" << std::endl;
        int hpPercent = (currentCharacter->GetHp() * 100) / currentCharacter->GetMaxHp();
        std::cout << "│ 生命值: [";
        DrawProgressBar(hpPercent, 40, "█", "?");
        std::cout << "] " << std::setw(3) << std::right << hpPercent << "% │" << std::endl;
        std::cout << "│     " << std::setw(5) << std::left << currentCharacter->GetHp()
            << "/" << std::setw(5) << std::left << currentCharacter->GetMaxHp()
            << "                                        │" << std::endl;

        Player* player = dynamic_cast<Player*>(currentCharacter.get());
        if (player)
        {
            int mpPercent = (player->GetMp() * 100) / player->GetMaxMp();
            std::cout << "│ 蓝量: [";
            DrawProgressBar(mpPercent, 40, "▓", "?");
            std::cout << "] " << std::setw(3) << std::right << mpPercent << "% │" << std::endl;
            std::cout << "│     " << std::setw(5) << std::left << player->GetMp()
                << "/" << std::setw(5) << std::left << player->GetMaxMp()
                << "                                        │" << std::endl;
        }

        int gaugePercent = currentCharacter->GetGaugePercent();
        int displayPercent = (gaugePercent > 100) ? 100 : gaugePercent;
        std::cout << "│ 行动条: [";
        DrawProgressBar(displayPercent, 40, "▌", "─");
        std::cout << "] " << std::setw(3) << std::right << displayPercent << "% │" << std::endl;

        std::cout << "├─ 属性面板 ──────────────────────────────────────────────────┤" << std::endl;
        std::cout << "│ 攻击力 (ATK): " << std::setw(45) << std::left << currentCharacter->GetAttackPower() << "│" << std::endl;
        std::cout << "│ 防御力 (DEF): " << std::setw(45) << std::left << currentCharacter->GetDefend() << "│" << std::endl;
        std::cout << "│ 速度 (SPD):   " << std::setw(45) << std::left << currentCharacter->GetSpeed() << "│" << std::endl;
        std::cout << "│ 等级 (LVL):   " << std::setw(45) << std::left << currentCharacter->GetLevel() << "│" << std::endl;
        std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;
    }
    else
    {
        std::cout << "│ 没有可用的角色" << std::endl;
    }
}

// ================================================================
//  角色列表
// ================================================================
void GameManager::DisplayCharacterList() const
{
    ClearScreen();
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                        角色列表                                ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    for (size_t i = 0; i < characters.size(); ++i)
    {
        std::string marker = (characters[i] == currentCharacter) ? "→" : " ";
        std::cout << "║ " << marker << " [" << i << "] " << std::setw(55) << std::left
            << characters[i]->GetName() << "║" << std::endl;
    }
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "输入角色索引选择（或按其他键返回）: ";
}

// ================================================================
//  怪物列表
// ================================================================
void GameManager::DisplayEnemies() const
{
    ClearScreen();
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                        怪物列表                                ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    if (enemies.empty())
    {
        std::cout << "║ 暂无怪物生成" << std::endl;
    }
    else
    {
        for (size_t i = 0; i < enemies.size(); ++i)
        {
            std::cout << "║ [" << i << "] ";
            enemies[i]->DisplayInfo();
            std::cout << " ║" << std::endl;
        }
    }
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    WaitForKeyPress();
}

// ================================================================
//  主菜单（新增 E 选项）
// ================================================================
void GameManager::DisplayMainMenu() const
{
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                        主菜单                                  ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ 1. 查看角色列表                                                ║" << std::endl;
    std::cout << "║ 2. 普通攻击（平A）                                              ║" << std::endl;
    std::cout << "║ 3. 造成伤害（测试）                                            ║" << std::endl;
    std::cout << "║ 4. 治疗当前角色                                                ║" << std::endl;
    std::cout << "║ 5. 恢复蓝量                                                    ║" << std::endl;
    std::cout << "║ 6. 释放职业技能                                                ║" << std::endl;
    std::cout << "║ 7. 查看当前角色详细信息                                        ║" << std::endl;
    std::cout << "║ 8. 演示伤害计算系统                                            ║" << std::endl;
    std::cout << "║ 9. 查看行动条                                                  ║" << std::endl;
    std::cout << "║ A. 查看怪物列表                                                ║" << std::endl;
    std::cout << "║ T. 测试史莱姆战斗                                              ║" << std::endl;
    std::cout << "║ S. 保存游戏                                                    ║" << std::endl;
    std::cout << "║ L. 加载游戏                                                    ║" << std::endl;
    std::cout << "║ I. 打开背包                                                    ║" << std::endl;
    std::cout << "║ P. 进入商店                                                    ║" << std::endl;
    std::cout << "║ E. 探索地图（开始冒险）                                        ║" << std::endl;
    std::cout << "║ R. 刷新面板                                                    ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║               ATB 战斗测试（新功能！）                           ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ M1. ATB 1v1 - 结算模式 + 行动顺序预测                           ║" << std::endl;
    std::cout << "║ M2. 史莱姆分裂 - 属性继承验证                                   ║" << std::endl;
    std::cout << "║ M3. 完整ATB战斗 - 多敌人+技能+分裂                               ║" << std::endl;
    std::cout << "║ M4. 三种职业对比 - SPD差异影响行动频率                           ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ 0. 退出游戏                                                    ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "请输入你的选择: ";
}

// ================================================================
//  处理操作（新增 E 分支）
// ================================================================
void GameManager::HandleAction()
{
    char choice;
    std::cin >> choice;
    if (!currentCharacter) return;

    Player* player = dynamic_cast<Player*>(currentCharacter.get());

    switch (choice)
    {
        // ===== 1. 探索 / 战斗 =====
    case '1':
        ExploreMap();
        break;

        // ===== 2. 查看角色列表 =====
    case '2':
        DisplayCharacterList();
        int charIndex;
        if (std::cin >> charIndex) SetCurrentCharacter(charIndex);
        break;

        // ===== 3. 使用技能 =====
    case '3':
    {
        if (player)
        {
            ClearScreen();
            std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║                      职业技能演示                             ║" << std::endl;
            std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
            Mage* mage = dynamic_cast<Mage*>(player);
            Braver* braver = dynamic_cast<Braver*>(player);
            if (mage)
            {
                std::cout << "║ [法师技能] 火焰术                                               ║" << std::endl;
                std::cout << "║ 消耗蓝量: 30                                                  ║" << std::endl;
                std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
                if (mage->HasEnoughMp(30))
                {
                    std::cout << "║ ";
                    mage->CastSpell(30);
                    std::cout << " ║" << std::endl;
                    std::cout << "║ 法术攻击力: " << mage->GetSpellPower() << std::endl;
                    std::cout << "║ 技能倍率: 1.8x" << std::endl;
                }
                else std::cout << "║ 蓝量不足！" << std::endl;
            }
            else if (braver)
            {
                std::cout << "║ [勇者技能] 刺客之刃 - 真实伤害                                   ║" << std::endl;
                std::cout << "║ 消耗蓝量: 20                                                  ║" << std::endl;
                std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
                if (braver->HasEnoughMp(20))
                {
                    braver->ConsumeMp(20);
                    std::cout << "║ ";
                    if (!enemies.empty())
                    {
                        braver->ExecuteTrueAttack(enemies[0].get());
                    }
                    else
                    {
                        std::cout << braver->GetName() << " 使出刺客之刃！（没有目标，技能未释放）";
                    }
                    std::cout << " ║" << std::endl;
                    std::cout << "║ 无视防御和抗性！" << std::endl;
                }
                else std::cout << "║ 蓝量不足！" << std::endl;
            }
            else
            {
                std::cout << "║ [战士技能] 挥斩                                               ║" << std::endl;
                std::cout << "║ 消耗蓝量: 15                                                  ║" << std::endl;
                std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
                if (player->HasEnoughMp(15))
                {
                    player->ConsumeMp(15);
                    std::cout << "║ " << player->GetName() << " 使出强力挥斩！伤害倍率: 1.5x ║" << std::endl;
                }
                else std::cout << "║ 蓝量不足！" << std::endl;
            }
            std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
            WaitForKeyPress();
        }
        else std::cout << "该角色不支持技能系统！" << std::endl;
    }
    break;

    // ===== 4. 打开背包 =====
    case '4':
    {
        Player* p = dynamic_cast<Player*>(currentCharacter.get());
        if (p)
        {
            ClearScreen();
            std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║                        背包界面                                ║" << std::endl;
            std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
            p->GetInventory().ShowInventory();
            std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
            std::cout << "║ 输入道具序号使用道具（按 Q 退出）: ";
            char input;
            std::cin >> input;
            if (input != 'Q' && input != 'q')
            {
                int index = input - '0';
                p->GetInventory().UseItem(index, p);
            }
        }
        else std::cout << "当前角色不是玩家！" << std::endl;
        WaitForKeyPress();
    }
    break;

    // ===== 5. 进入商店 =====
    case '5':
    {
        Player* p = dynamic_cast<Player*>(currentCharacter.get());
        if (p)
        {
            Shop shop;
            bool inShop = true;
            while (inShop)
            {
                ClearScreen();
                std::cout << "========================================================" << std::endl;
                std::cout << "                    商店界面" << std::endl;
                std::cout << "========================================================" << std::endl;
                std::cout << " 当前金币: " << p->GetGold() << " 金" << std::endl;
                std::cout << "========================================================" << std::endl;
                shop.ShowShop();
                std::cout << "========================================================" << std::endl;
                std::cout << " B. 购买道具  S. 出售道具  Q. 退出商店" << std::endl;
                std::cout << "========================================================" << std::endl;
                std::cout << "请输入你的选择: ";
                char shopChoice;
                std::cin >> shopChoice;
                switch (shopChoice)
                {
                case 'B': case 'b':
                {
                    std::cout << "请输入要购买的道具序号: ";
                    int index;
                    std::cin >> index;
                    shop.BuyItem(*p, index);
                    WaitForKeyPress();
                }
                break;
                case 'S': case 's':
                {
                    std::cout << "请输入要出售的背包道具序号: ";
                    int index;
                    std::cin >> index;
                    shop.SellItem(*p, index);
                    WaitForKeyPress();
                }
                break;
                case 'Q': case 'q':
                    inShop = false;
                    break;
                }
            }
        }
        else std::cout << "当前角色不是玩家！" << std::endl;
        WaitForKeyPress();
    }
    break;

    // ===== 6. 属性分配 =====
    case '6':
    {
        Player* p = dynamic_cast<Player*>(currentCharacter.get());
        if (p)
        {
            ClearScreen();
            std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║                      属性分配（AP 加点）                       ║" << std::endl;
            std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
            std::cout << "║ 当前可用 AP: " << p->GetAvailableAP() << std::endl;
            std::cout << "║ ATK: " << p->GetAttackPower() << "  DEF: " << p->GetDefend() << std::endl;
            std::cout << "║ HP:  " << p->GetHp() << "/" << p->GetMaxHp() << "  MP: " << p->GetMp() << "/" << p->GetMaxMp() << std::endl;
            std::cout << "║ SPD: " << p->GetSpeed() << std::endl;
            std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
            std::cout << "║ 1. 加 MaxHP (生命上限+8, 消耗1AP)                              ║" << std::endl;
            std::cout << "║ 2. 加 MaxMP (蓝量上限+5, 消耗1AP)                              ║" << std::endl;
            std::cout << "║ 3. 加 ATK  (攻击力+2, 消耗1AP)                                 ║" << std::endl;
            std::cout << "║ 4. 加 DEF  (防御力+1, 消耗1AP)                                 ║" << std::endl;
            std::cout << "║ 5. 加 SPD  (速度+1, 消耗1AP)                                   ║" << std::endl;
            std::cout << "║ Q. 返回                                                        ║" << std::endl;
            std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
            std::cout << "请选择: ";

            char apChoice;
            std::cin >> apChoice;
            if (p->GetAvailableAP() <= 0)
            {
                std::cout << "没有可用的 AP！请升级后获得。" << std::endl;
            }
            else
            {
                switch (apChoice)
                {
                case '1': p->AllocateAttribute(1); break;
                case '2': p->AllocateAttribute(2); break;
                case '3': p->AllocateAttribute(3); break;
                case '4': p->AllocateAttribute(4); break;
                case '5': p->AllocateAttribute(5); break;
                default: break;
                }
            }
        }
        else std::cout << "当前角色不是玩家！" << std::endl;
        WaitForKeyPress();
    }
    break;

    // ===== 7. 查看当前角色详细信息 =====
    case '7':
        ClearScreen();
        std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                      角色详细信息                             ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║ ";
        currentCharacter->DisplayInfo();
        std::cout << "║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
        WaitForKeyPress();
        break;

        // ===== 8. 演示伤害计算系统 =====
    case '8':
        DemonstrateDamageSystem();
        break;

        // ===== 9. 查看行动条 =====
    case '9':
    {
        ClearScreen();
        std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                      所有角色行动条                           ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
        for (size_t i = 0; i < characters.size(); ++i)
        {
            int gaugePercent = characters[i]->GetGaugePercent();
            int displayPercent = (gaugePercent > 100) ? 100 : gaugePercent;
            std::cout << "║ " << std::setw(10) << std::left << characters[i]->GetName() << " [";
            DrawProgressBar(displayPercent, 30, "█", "?");
            std::cout << "] " << std::setw(3) << std::right << displayPercent << "% ║" << std::endl;
        }
        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║ 提示: 行动条满100%时可以行动                                   ║" << std::endl;
        std::cout << "║ 速度越高，行动条填充越快                                       ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
        WaitForKeyPress();
    }
    break;

    // ===== A. 查看怪物列表 =====
    case 'A': case 'a':
        DisplayEnemies();
        break;

        // ===== T. 测试史莱姆战斗 =====
    case 'T': case 't':
        TestSlime();
        WaitForKeyPress();
        break;

        // ===== S. 保存游戏 =====
    case 'S': case 's':
    {
        Player* player = dynamic_cast<Player*>(currentCharacter.get());
        if (player) SaveLoadManager::SaveGame(*player);
        else std::cout << "当前角色无法保存！" << std::endl;
        WaitForKeyPress();
    }
    break;

    // ===== L. 加载游戏 =====
    case 'L': case 'l':
    {
        Player* player = dynamic_cast<Player*>(currentCharacter.get());
        if (player)
        {
            if (SaveLoadManager::LoadGame(*player)) std::cout << "存档加载成功！" << std::endl;
        }
        else std::cout << "当前角色无法加载存档！" << std::endl;
        WaitForKeyPress();
    }
    break;

    // ===== M. ATB 测试 =====
    case 'M': case 'm':
    {
        char sub;
        std::cin >> sub;
        std::cin.ignore();
        switch (sub)
        {
        case '1': TestATB_1v1(); break;
        case '2': TestSlimeSplit(); break;
        case '3': TestFullBattle(); break;
        case '4': TestAllClasses(); break;
        default: std::cout << "无效选择！" << std::endl; WaitForKeyPress(); break;
        }
    }
    break;

    // ===== I. 打开背包 =====
    case 'I': case 'i':
    {
        Player* player = dynamic_cast<Player*>(currentCharacter.get());
        if (player)
        {
            ClearScreen();
            std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║                        背包界面                                ║" << std::endl;
            std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
            player->GetInventory().ShowInventory();
            std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
            std::cout << "║ 输入道具序号使用道具（按 Q 退出）: ";
            char input;
            std::cin >> input;
            if (input != 'Q' && input != 'q')
            {
                int index = input - '0';
                player->GetInventory().UseItem(index, player);
            }
        }
        else std::cout << "当前角色不是玩家！" << std::endl;
        WaitForKeyPress();
    }
    break;

    // ===== P. 进入商店 =====
    case 'P': case 'p':
    {
        Player* player = dynamic_cast<Player*>(currentCharacter.get());
        if (player)
        {
            Shop shop;
            bool inShop = true;
            while (inShop)
            {
                ClearScreen();
                std::cout << "========================================================" << std::endl;
                std::cout << "                    商店界面" << std::endl;
                std::cout << "========================================================" << std::endl;
                std::cout << " 当前金币: " << player->GetGold() << " 金" << std::endl;
                std::cout << "========================================================" << std::endl;
                shop.ShowShop();
                std::cout << "========================================================" << std::endl;
                std::cout << " B. 购买道具  S. 出售道具  Q. 退出商店" << std::endl;
                std::cout << "========================================================" << std::endl;
                std::cout << "请输入你的选择: ";
                char shopChoice;
                std::cin >> shopChoice;
                switch (shopChoice)
                {
                case 'B': case 'b':
                {
                    std::cout << "请输入要购买的道具序号: ";
                    int index;
                    std::cin >> index;
                    shop.BuyItem(*player, index);
                    WaitForKeyPress();
                }
                break;
                case 'S': case 's':
                {
                    std::cout << "请输入要出售的背包道具序号: ";
                    int index;
                    std::cin >> index;
                    shop.SellItem(*player, index);
                    WaitForKeyPress();
                }
                break;
                case 'Q': case 'q':
                    inShop = false;
                    break;
                }
            }
        }
        else std::cout << "当前角色不是玩家！" << std::endl;
        WaitForKeyPress();
    }
    break;

    // ===== E. 探索地图 =====
    case 'E': case 'e':
        ExploreMap();
        break;

        // ===== R. 刷新面板 =====
    case 'R': case 'r':
        break;

        // ===== 0. 退出游戏 =====
    case '0':
        std::cout << "感谢游玩！再见！" << std::endl;
        exit(0);
        break;

    default:
        std::cout << "无效选择！请重新输入。" << std::endl;
        WaitForKeyPress();
    }
}

// ================================================================
//  地图探索（新增）
// ================================================================
void GameManager::ExploreMap()
{
    Player* player = dynamic_cast<Player*>(currentCharacter.get());
    if (!player)
    {
        std::cout << "当前角色无法探索地图！" << std::endl;
        WaitForKeyPress();
        return;
    }

    bool exploring = true;
    while (exploring && player->IsAlive())
    {
        gameMap.Display();
        std::cout << "\n[移动] w/a/s/d  [状态]c  [背包]i  [商店]t(仅城镇)  [退出]q\n>> ";
        char cmd;
        std::cin >> cmd;

        if (cmd == 'q')
        {
            exploring = false;
        }
        else if (cmd == 'w' || cmd == 'a' || cmd == 's' || cmd == 'd')
        {
            if (gameMap.Move(cmd))
            {
                auto enemy = gameMap.CheckEncounter(player->GetLevel());
                if (enemy)
                {
                    ClearScreen();
                    std::cout << "遭遇了 " << enemy->GetName() << "！" << std::endl;
                    BattleEncounter(player, enemy);

                    if (!enemy->IsAlive())
                    {
                        std::cout << "战斗胜利！" << std::endl;
                        player->GainExperience(enemy->GetExperienceReward());
                        player->gold += enemy->GetGoldReward();
                        for (auto* drop : enemy->GenerateDrops())
                            player->inventory.AddItem(std::unique_ptr<Item>(drop));
                    }
                    else
                    {
                        std::cout << "你被击败了……" << std::endl;
                        exploring = false;
                    }
                }
            }
            else
            {
                std::cout << "无法移动！" << std::endl;
            }
        }
        else if (cmd == 'c')
        {
            player->DisplayInfo();
            WaitForKeyPress();
        }
        else if (cmd == 'i')
        {
            player->inventory.ShowInventory();
            WaitForKeyPress();
        }
        else if (cmd == 't')
        {
            if (gameMap.IsTown())
            {
                Shop shop;
                bool inShop = true;
                while (inShop)
                {
                    ClearScreen();
                    std::cout << "========================================================" << std::endl;
                    std::cout << "                    商店界面" << std::endl;
                    std::cout << "========================================================" << std::endl;
                    std::cout << " 当前金币: " << player->GetGold() << " 金" << std::endl;
                    shop.ShowShop();
                    std::cout << "========================================================" << std::endl;
                    std::cout << " B. 购买道具  S. 出售道具  Q. 退出商店" << std::endl;
                    std::cout << "========================================================" << std::endl;
                    std::cout << "请输入你的选择: ";
                    char shopChoice;
                    std::cin >> shopChoice;
                    switch (shopChoice)
                    {
                    case 'B': case 'b':
                    {
                        std::cout << "请输入要购买的道具序号: ";
                        int index;
                        std::cin >> index;
                        shop.BuyItem(*player, index);
                        WaitForKeyPress();
                    }
                    break;
                    case 'S': case 's':
                    {
                        std::cout << "请输入要出售的背包道具序号: ";
                        int index;
                        std::cin >> index;
                        shop.SellItem(*player, index);
                        WaitForKeyPress();
                    }
                    break;
                    case 'Q': case 'q':
                        inShop = false;
                        break;
                    }
                }
            }
            else
            {
                std::cout << "只能在城镇购物！" << std::endl;
            }
        }
        else
        {
            std::cout << "无效指令" << std::endl;
        }
    }
}

// ================================================================
//  遇敌战斗（新增）
// ================================================================
void GameManager::BattleEncounter(Player* player, std::shared_ptr<Enemy> enemy)
{
    bool playerTurn = true;
    while (player->IsAlive() && enemy->IsAlive())
    {
        std::cout << "\n你的 HP: " << player->GetHp() << "/" << player->GetMaxHp()
            << "  MP: " << player->GetMp() << "/" << player->GetMaxMp()
            << "  敌人 HP: " << enemy->GetHp() << "/" << enemy->GetMaxHp() << std::endl;

        if (playerTurn)
        {
            std::cout << "你的回合：1.攻击 2.技能 3.道具\n>> ";
            int act;
            std::cin >> act;
            if (act == 1)
            {
                player->NormalAttack(enemy.get());
            }
            else if (act == 2)
            {
                const auto& skills = player->GetSkills();
                if (!skills.empty())
                {
                    std::cout << "选择技能：\n";
                    for (size_t i = 0; i < skills.size(); ++i)
                        std::cout << i + 1 << ". " << skills[i]->GetName() << " (MP:" << skills[i]->GetMpCost() << ")\n";
                    int si;
                    std::cin >> si;
                    player->UseSkill(si - 1, enemy.get());
                }
                else std::cout << "没有技能！" << std::endl;
            }
            else if (act == 3)
            {
                player->inventory.ShowInventory();
                std::cout << "使用道具序号（0取消）: ";
                int ii;
                std::cin >> ii;
                if (ii > 0) player->inventory.UseItem(ii - 1, player);
            }
            playerTurn = false;
        }
        else
        {
            std::cout << enemy->GetName() << " 行动！" << std::endl;
            enemy->PerformAction(player);
            playerTurn = true;
        }
        if (!enemy->IsAlive() || !player->IsAlive()) break;
    }
}

// ================================================================
//  DrawSeparator（原有）
// ================================================================
// 注意：此函数已在 GameManager.h 中声明为 static，且在第1段中未定义，此处补充定义。
// 如果第1段已定义，则删除此处避免重复。请检查：第1段中是否包含了 DrawSeparator 的实现？
// 为安全起见，这里再次提供，若重复请删除其中一个。


// ================================================================
//  伤害计算系统演示（原有）
// ================================================================
void GameManager::DemonstrateDamageSystem()
{
    if (characters.size() < 2)
    {
        std::cout << "需要至少两个角色来演示伤害系统！" << std::endl;
        return;
    }

    ClearScreen();
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    伤害系统演示                                ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;

    for (size_t i = 0; i < characters.size(); ++i)
        std::cout << "║ [" << i << "] " << std::setw(58) << std::left << characters[i]->GetName() << "║" << std::endl;

    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "请选择攻击者编号: ";
    int attackerIdx, defenderIdx;
    if (!(std::cin >> attackerIdx) || attackerIdx < 0 || attackerIdx >= static_cast<int>(characters.size()))
    {
        std::cout << "无效选择！" << std::endl;
        return;
    }
    std::cout << "请选择防御者编号: ";
    if (!(std::cin >> defenderIdx) || defenderIdx < 0 || defenderIdx >= static_cast<int>(characters.size()))
    {
        std::cout << "无效选择！" << std::endl;
        return;
    }
    if (attackerIdx == defenderIdx)
    {
        std::cout << "不能攻击自己！" << std::endl;
        return;
    }
    std::cout << std::endl;
    PerformAttack(characters[attackerIdx], characters[defenderIdx]);
    std::cout << std::endl;
    WaitForKeyPress();
}

// ================================================================
//  PerformAttack（原有）
// ================================================================
void GameManager::PerformAttack(std::shared_ptr<Character> attacker, std::shared_ptr<Character> defender)
{
    DamageCalculator calculator;

    std::cout << "攻击者信息:" << std::endl;
    attacker->DisplayInfo();
    std::cout << std::endl;
    std::cout << "防御者信息:" << std::endl;
    defender->DisplayInfo();
    std::cout << std::endl;

    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "【物理伤害计算】" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;

    DamageInfo physicalDamage = calculator.CalculatePhysicalDamage(
        attacker->GetAttackPower(),
        defender->GetDefend(),
        attacker->GetLevel(),
        defender->GetLevel(),
        1.5,
        attacker->GetElementType(),
        defender->GetElementType(),
        attacker->GetAgility(),
        defender->GetResistance()
    );

    std::cout << "基础伤害: " << std::fixed << std::setprecision(2) << physicalDamage.baseDamage << std::endl;
    std::cout << "属性克制倍数: " << physicalDamage.elementBonus << "x" << std::endl;
    std::cout << "暴击: " << (physicalDamage.isCritical ? "是 (×1.5)" : "否") << std::endl;
    std::cout << "抗性减伤: " << (1 - physicalDamage.resistanceReduction) * 100 << "%" << std::endl;
    std::cout << "最终伤害: " << static_cast<int>(physicalDamage.finalDamage) << std::endl;

    std::cout << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "【魔法伤害计算】" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;

    DamageInfo magicalDamage = calculator.CalculateMagicalDamage(
        50,
        defender->GetSpellDefense(),
        attacker->GetLevel(),
        defender->GetLevel(),
        1.2,
        attacker->GetElementType(),
        defender->GetElementType(),
        attacker->GetAgility(),
        defender->GetResistance()
    );

    std::cout << "基础法术攻击力: 50" << std::endl;
    std::cout << "防御方法术防御力: " << defender->GetSpellDefense() << std::endl;
    std::cout << "法术减伤比: " << std::fixed << std::setprecision(2)
        << (defender->GetSpellDefense() / (defender->GetSpellDefense() + 200.0)) * 100 << "%" << std::endl;
    std::cout << "基础伤害: " << magicalDamage.baseDamage << std::endl;
    std::cout << "属性克制倍数: " << magicalDamage.elementBonus << "x" << std::endl;
    std::cout << "暴击: " << (magicalDamage.isCritical ? "是 (×1.5)" : "否") << std::endl;
    std::cout << "最终伤害: " << static_cast<int>(magicalDamage.finalDamage) << std::endl;

    std::cout << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "【真实伤害计算】" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;

    DamageInfo trueDamage = calculator.CalculateTrueDamage(
        20,
        attacker->GetAttackPower(),
        attacker->GetLevel(),
        defender->GetLevel()
    );

    std::cout << "固定伤害值: 20" << std::endl;
    std::cout << "攻击力倍数伤害: " << std::fixed << std::setprecision(2)
        << (attacker->GetAttackPower() * 0.5) << std::endl;
    std::cout << "最终伤害: " << static_cast<int>(trueDamage.finalDamage) << std::endl;
    std::cout << "（无视防御和抗性）" << std::endl;
}

// ================================================================
//  TestSlime（原有）
// ================================================================
void GameManager::TestSlime()
{
    ClearScreen();
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    史莱姆战斗测试                             ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;

    if (!currentCharacter)
    {
        std::cout << "║ 没有当前角色" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
        return;
    }

    auto slime1 = std::make_shared<Slime>(1);
    auto slime2 = std::make_shared<Slime>(3);
    auto slime3 = std::make_shared<Slime>(5);

    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ 玩家信息:" << std::endl;
    std::cout << "║ ";
    currentCharacter->DisplayInfo();
    std::cout << " ║" << std::endl;

    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ 史莱姆敌人信息:" << std::endl;
    std::cout << "║ ";
    slime1->DisplayInfo();
    std::cout << " ║" << std::endl;
    std::cout << "║ ";
    slime2->DisplayInfo();
    std::cout << " ║" << std::endl;
    std::cout << "║ ";
    slime3->DisplayInfo();
    std::cout << " ║" << std::endl;

    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ 测试1: 玩家普通攻击史莱姆1" << std::endl;
    Player* player = dynamic_cast<Player*>(currentCharacter.get());
    if (player)
    {
        player->NormalAttack(slime1.get());
    }

    std::cout << std::endl << "║ 测试2: 史莱姆1普通攻击玩家" << std::endl;
    slime1->PerformAction(currentCharacter.get());

    std::cout << std::endl << "║ 测试3: 史莱姆血量低时的分裂攻击" << std::endl;
    while (slime2->GetHp() > slime2->GetMaxHp() / 2)
        slime2->TakeDamage(10);
    slime2->PerformAction(currentCharacter.get());

    std::cout << std::endl << "║ 测试结束！" << std::endl;
    if (player)
    {
        int expReward = 50;
        std::cout << "║ 战斗胜利！获得 " << expReward << " 经验值！" << std::endl;
        player->GainExperience(expReward);
    }
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
}


// ================================================================
//  TestATB_1v1
// ================================================================
void GameManager::TestATB_1v1()
{
    ClearScreen();
    DrawSeparator("测试1：ATB战斗 - 1v1（无跑条动画，仅结算+行动顺序）");

    auto player = std::make_shared<Player>("战士·剑心");
    player->SetPhysicalResistance(0.05);
    player->SetMagicalResistance(0.03);
    player->AddSkill(std::make_shared<PhysicalSkill>("强力挥斩", 1.5, 15, ElementType::Neutral));
    player->AddSkill(std::make_shared<PhysicalSkill>("破甲突刺", 1.8, 25, ElementType::Neutral));

    auto slime = std::make_shared<Slime>(3);
    std::vector<std::shared_ptr<Enemy>> enemies;
    enemies.push_back(slime);

    std::cout << "玩家(SPD=" << player->GetSpeed() << ") vs 史莱姆(SPD=" << slime->GetSpeed() << ")\n";
    std::cout << "行动条上限: " << MAX_GAUGE << "（满了才能行动）\n\n";
    player->DisplayInfo();
    slime->DisplayInfo();
    std::cout << "\n按 Enter 开始...";
    std::cin.get();

    bool quit = false;
    int round = 0;

    while (!quit && player->IsAlive() && slime->IsAlive()) {
        int frames = ATBTest::FastForwardToNextAction(player.get(), enemies);
        auto predictions = ATBTest::PredictNextActors(ATBTest::BuildUnitList(player.get(), enemies));
        DrawSeparator("第 " + std::to_string(++round) + " 次行动 (推进 " + std::to_string(frames) + " 帧)");
        ATBTest::ShowActionOrder(predictions);

        auto queue = ATBTest::CollectReadyActors(player.get(), enemies);

        for (auto& entry : queue) {
            if (entry.isPlayer) {
                player->ResetGauge();
                std::cout << "\n>>> 你的回合！\n";
                std::cout << "HP:" << player->GetHp() << "/" << player->GetMaxHp()
                    << "  MP:" << player->GetMp() << "/" << player->GetMaxMp() << "\n";
                std::cout << "史莱姆 HP:" << slime->GetHp() << "/" << slime->GetMaxHp() << "\n\n";
                std::cout << "  [1] 普通攻击\n";
                std::cout << "  [2] 强力挥斩 (15蓝, 1.5x)\n";
                std::cout << "  [3] 破甲突刺 (25蓝, 1.8x)\n";
                std::cout << "  [Q] 退出\n";
                std::cout << "请输入: ";

                char ch;
                std::cin >> ch;
                std::cin.ignore();
                if (ch == 'Q' || ch == 'q') { quit = true; break; }

                switch (ch) {
                case '1': player->NormalAttack(slime.get()); break;
                case '2': player->UseSkill(0, slime.get()); break;
                case '3': player->UseSkill(1, slime.get()); break;
                default: std::cout << "无效选择。\n"; break;
                }

                std::cout << "\n[结算] 玩家 HP:" << player->GetHp() << "/" << player->GetMaxHp()
                    << " MP:" << player->GetMp() << "/" << player->GetMaxMp()
                    << "  史莱姆 HP:" << slime->GetHp() << "/" << slime->GetMaxHp() << "\n";
                std::cout << "按 Enter 继续...";
                std::cin.get();
            }
            else {
                auto& enemy = enemies[entry.enemyIndex];
                if (!enemy->IsAlive()) continue;
                enemy->ResetGauge();
                std::cout << "\n>>> " << enemy->GetName() << " 行动！\n";
                auto spawned = enemy->PerformAction(player.get());
                if (!spawned.empty())
                    std::cout << "  (分裂，1v1测试不加入战场)\n";

                std::cout << "\n[结算] 玩家 HP:" << player->GetHp() << "/" << player->GetMaxHp()
                    << " MP:" << player->GetMp() << "/" << player->GetMaxMp()
                    << "  史莱姆 HP:" << slime->GetHp() << "/" << slime->GetMaxHp() << "\n";
                std::cout << "按 Enter 继续...";
                std::cin.get();
            }

            if (!player->IsAlive() || !slime->IsAlive()) break;
        }
    }

    if (player->IsAlive() && !slime->IsAlive())
        std::cout << "\n【胜利！】\n";
    else if (!player->IsAlive())
        std::cout << "\n【失败！】\n";

    std::cout << "\n按 Enter 继续...";
    std::cin.get();
}

// ================================================================
//  TestSlimeSplit
// ================================================================
void GameManager::TestSlimeSplit()
{
    ClearScreen();
    DrawSeparator("测试2：史莱姆分裂验证");

    auto player = std::make_shared<Player>("测试勇者");
    player->SetAttackPower(30);

    auto slime = std::make_shared<Slime>(3);
    std::vector<std::shared_ptr<Enemy>> enemies;
    enemies.push_back(slime);

    std::cout << "玩家 SPD=" << player->GetSpeed() << " | 史莱姆 SPD=" << slime->GetSpeed() << "\n";
    std::cout << "分裂阈值: HP < " << slime->GetMaxHp() / 2 << "\n\n";
    player->DisplayInfo();
    slime->DisplayInfo();
    std::cout << "\n按 Enter 开始...";
    std::cin.get();

    int round = 0;
    while (slime->IsAlive() && player->IsAlive() && round < 10) {
        int frames = ATBTest::FastForwardToNextAction(player.get(), enemies);
        auto predictions = ATBTest::PredictNextActors(ATBTest::BuildUnitList(player.get(), enemies));
        round++;

        DrawSeparator("第 " + std::to_string(round) + " 次行动 (推进 " + std::to_string(frames) + " 帧)");
        ATBTest::ShowActionOrder(predictions);

        auto queue = ATBTest::CollectReadyActors(player.get(), enemies);

        for (auto& entry : queue) {
            if (entry.isPlayer) {
                player->ResetGauge();
                std::cout << "\n>>> 玩家行动 -> 普通攻击\n";
                player->NormalAttack(slime.get());
                std::cout << "\n[结算] 玩家 HP:" << player->GetHp()
                    << "  史莱姆 HP:" << slime->GetHp() << "/" << slime->GetMaxHp() << "\n";
                std::cout << "按 Enter 继续...";
                std::cin.get();
            }
            else {
                auto& enemy = enemies[entry.enemyIndex];
                if (!enemy->IsAlive()) continue;
                enemy->ResetGauge();
                std::cout << "\n>>> " << enemy->GetName() << " 行动\n";
                auto spawned = enemy->PerformAction(player.get());

                if (!spawned.empty()) {
                    std::cout << "\n>>> 分裂验证：\n";
                    auto& newS = spawned[0];
                    std::cout << "  原始: ATK=" << slime->GetAttackPower()
                        << " DEF=" << slime->GetDefend()
                        << " MaxHP=" << slime->GetMaxHp()
                        << " HP=" << slime->GetHp()
                        << " LV=" << slime->GetLevel() << "\n";
                    std::cout << "  分裂: ATK=" << newS->GetAttackPower()
                        << " DEF=" << newS->GetDefend()
                        << " MaxHP=" << newS->GetMaxHp()
                        << " HP=" << newS->GetHp()
                        << " LV=" << newS->GetLevel() << "\n";

                    bool match = (slime->GetAttackPower() == newS->GetAttackPower()) &&
                        (slime->GetDefend() == newS->GetDefend()) &&
                        (slime->GetMaxHp() == newS->GetMaxHp()) &&
                        (slime->GetLevel() == newS->GetLevel());
                    std::cout << "  属性继承: " << (match ? "通过 OK" : "失败 X") << "\n";

                    std::cout << "\n[结算] 玩家 HP:" << player->GetHp()
                        << "  史莱姆 HP:" << slime->GetHp() << "/" << slime->GetMaxHp() << "\n";
                    std::cout << "按 Enter 继续...";
                    std::cin.get();

                    round = 99;
                    break;
                }

                std::cout << "\n[结算] 玩家 HP:" << player->GetHp()
                    << "  史莱姆 HP:" << slime->GetHp() << "/" << slime->GetMaxHp() << "\n";
                std::cout << "按 Enter 继续...";
                std::cin.get();
            }

            if (!player->IsAlive() || !slime->IsAlive()) break;
        }
    }

    std::cout << "\n按 Enter 继续...";
    std::cin.get();
}

// ================================================================
//  TestFullBattle
// ================================================================
void GameManager::TestFullBattle()
{
    ClearScreen();
    DrawSeparator("测试3：完整ATB战斗 - 多敌人+技能+分裂");

    auto player = std::make_shared<Player>("战士·剑心");
    player->SetPhysicalResistance(0.05);
    player->SetMagicalResistance(0.03);
    player->AddSkill(std::make_shared<PhysicalSkill>("强力挥斩", 1.5, 15, ElementType::Neutral));
    player->AddSkill(std::make_shared<PhysicalSkill>("破甲突刺", 1.8, 25, ElementType::Neutral));

    std::vector<std::shared_ptr<Enemy>> enemies;
    enemies.push_back(std::make_shared<Slime>(1));
    enemies.push_back(std::make_shared<Slime>(2));
    enemies.push_back(std::make_shared<Slime>(4));

    std::cout << "玩家 SPD=" << player->GetSpeed() << "\n";
    for (auto& e : enemies)
        std::cout << e->GetName() << " SPD=" << e->GetSpeed() << "\n";
    std::cout << "\n";
    player->DisplayInfo();
    std::cout << "\n敌方阵容 (" << enemies.size() << " 个)：\n";
    for (auto& e : enemies) e->DisplayInfo();
    std::cout << "\n按 Enter 开始...";
    std::cin.get();

    bool battleOver = false;
    int round = 0;

    while (!battleOver) {
        int frames = ATBTest::FastForwardToNextAction(player.get(), enemies);
        auto predictions = ATBTest::PredictNextActors(ATBTest::BuildUnitList(player.get(), enemies));
        DrawSeparator("行动 #" + std::to_string(++round) + " (推进 " + std::to_string(frames) + " 帧)");
        ATBTest::ShowActionOrder(predictions);

        auto queue = ATBTest::CollectReadyActors(player.get(), enemies);

        for (auto& entry : queue) {
            if (!player->IsAlive()) { battleOver = true; break; }

            if (entry.isPlayer) {
                player->ResetGauge();

                bool anyAlive = false;
                for (auto& e : enemies)
                    if (e->IsAlive()) { anyAlive = true; break; }
                if (!anyAlive) {
                    std::cout << "\n所有敌人已消灭！\n";
                    battleOver = true;
                    break;
                }

                std::cout << "\n>>> 你的回合！\n";
                std::cout << "HP:" << player->GetHp() << "/" << player->GetMaxHp()
                    << "  MP:" << player->GetMp() << "/" << player->GetMaxMp() << "\n\n";
                std::cout << "  [1] 普通攻击\n";
                std::cout << "  [2] 强力挥斩 (15蓝, 1.5x)\n";
                std::cout << "  [3] 破甲突刺 (25蓝, 1.8x)\n";
                std::cout << "  [Q] 退出\n";
                std::cout << "请输入: ";

                char act;
                std::cin >> act;
                std::cin.ignore();
                if (act == 'Q' || act == 'q') { battleOver = true; break; }
                if (act < '1' || act > '3') { std::cout << "跳过。\n"; continue; }

                std::cout << "\n选择目标:\n";
                for (size_t i = 0; i < enemies.size(); ++i)
                    if (enemies[i]->IsAlive())
                        std::cout << "  [" << i << "] " << enemies[i]->GetName()
                        << " HP:" << enemies[i]->GetHp() << "/" << enemies[i]->GetMaxHp() << "\n";
                std::cout << "目标编号: ";

                int tgt;
                if (!(std::cin >> tgt) || tgt < 0 || tgt >= (int)enemies.size() || !enemies[tgt]->IsAlive()) {
                    std::cin.clear();
                    std::cin.ignore(1000, '\n');
                    std::cout << "无效目标，跳过。\n";
                    continue;
                }
                std::cin.ignore();

                switch (act) {
                case '1': player->NormalAttack(enemies[tgt].get()); break;
                case '2': player->UseSkill(0, enemies[tgt].get()); break;
                case '3': player->UseSkill(1, enemies[tgt].get()); break;
                }

                std::cout << "\n[结算] 玩家 HP:" << player->GetHp() << "/" << player->GetMaxHp()
                    << " MP:" << player->GetMp() << "/" << player->GetMaxMp() << "\n";
                std::cout << "目标 " << enemies[tgt]->GetName()
                    << " HP:" << enemies[tgt]->GetHp() << "/" << enemies[tgt]->GetMaxHp() << "\n";
                std::cout << "按 Enter 继续...";
                std::cin.get();
            }
            else {
                auto& enemy = enemies[entry.enemyIndex];
                if (!enemy->IsAlive()) continue;
                enemy->ResetGauge();
                std::cout << "\n>>> " << enemy->GetName() << " 行动！(SPD=" << enemy->GetSpeed() << ")\n";
                auto spawned = enemy->PerformAction(player.get());

                for (auto& s : spawned) {
                    enemies.push_back(s);
                    std::cout << "  >>> 新 " << s->GetName() << " 加入战场！\n";
                }

                std::cout << "\n[结算] 玩家 HP:" << player->GetHp() << "/" << player->GetMaxHp()
                    << " MP:" << player->GetMp() << "/" << player->GetMaxMp() << "\n";
                std::cout << "按 Enter 继续...";
                std::cin.get();
            }

            if (!player->IsAlive()) {
                std::cout << "\n玩家被击败！\n";
                battleOver = true;
                break;
            }
        }

        if (battleOver) break;

        std::vector<std::shared_ptr<Enemy>> alive;
        for (auto& e : enemies)
            if (e->IsAlive()) alive.push_back(e);
        enemies = alive;

        if (enemies.empty()) {
            std::cout << "\n所有敌人被击败！战斗胜利！\n";
            battleOver = true;
            break;
        }

        if (round >= 30) {
            std::cout << "\n超过30轮，自动结束。\n";
            battleOver = true;
        }
    }

    std::cout << "\n按 Enter 返回...";
    std::cin.get();
}

// ================================================================
//  TestAllClasses
// ================================================================
void GameManager::TestAllClasses()
{
    ClearScreen();
    DrawSeparator("测试4：三种职业 vs 史莱姆（ATB速度对比）");

    auto warrior = std::make_shared<Player>("战士·剑心");
    warrior->SetPhysicalResistance(0.05);
    warrior->SetMagicalResistance(0.03);
    warrior->AddSkill(std::make_shared<PhysicalSkill>("强力挥斩", 1.5, 15, ElementType::Neutral));

    auto mage = std::make_shared<Mage>("法师·火焰", 35);
    mage->SetPhysicalResistance(0.0);
    mage->SetMagicalResistance(0.15);
    mage->AddSkill(std::make_shared<MagicalSkill>("火焰术", 1.8, 30, 35, ElementType::Fire));

    auto braver = std::make_shared<Braver>("勇者·破晓");
    braver->SetPhysicalResistance(0.0);
    braver->SetMagicalResistance(0.05);
    braver->AddSkill(std::make_shared<TrueSkill>("刺客之刃", 30, 20));

    struct TestCase { std::shared_ptr<Player> player; std::string className; };
    std::vector<TestCase> cases = {
        { warrior, "战士" },
        { mage,    "法师" },
        { braver,  "勇者" }
    };

    for (auto& tc : cases) {
        auto slime = std::make_shared<Slime>(3);
        std::vector<std::shared_ptr<Enemy>> enemies;
        enemies.push_back(slime);

        std::cout << "\n========================================\n";
        std::cout << tc.className << " SPD=" << tc.player->GetSpeed()
            << " vs 史莱姆 SPD=" << slime->GetSpeed() << "\n";
        std::cout << "========================================\n";
        tc.player->DisplayInfo();
        slime->DisplayInfo();

        int round = 0;
        while (tc.player->IsAlive() && slime->IsAlive() && round < 3) {
            int frames = ATBTest::FastForwardToNextAction(tc.player.get(), enemies);
            auto predictions = ATBTest::PredictNextActors(ATBTest::BuildUnitList(tc.player.get(), enemies));
            round++;

            std::cout << "\n[行动 #" << round << " 推进" << frames << "帧] ";
            ATBTest::ShowActionOrder(predictions);

            auto queue = ATBTest::CollectReadyActors(tc.player.get(), enemies);

            for (auto& entry : queue) {
                if (entry.isPlayer) {
                    tc.player->ResetGauge();
                    if (round == 1)
                        tc.player->NormalAttack(slime.get());
                    else
                        tc.player->UseSkill(0, slime.get());

                    std::cout << "[结算] " << tc.className << " HP:" << tc.player->GetHp()
                        << "  史莱姆 HP:" << slime->GetHp() << "\n";
                    std::cout << "按 Enter 继续...";
                    std::cin.get();
                }
                else {
                    auto& enemy = enemies[entry.enemyIndex];
                    if (!enemy->IsAlive()) continue;
                    enemy->ResetGauge();
                    auto spawned = enemy->PerformAction(tc.player.get());
                    if (!spawned.empty())
                        std::cout << "  (分裂!)\n";

                    std::cout << "[结算] " << tc.className << " HP:" << tc.player->GetHp()
                        << "  史莱姆 HP:" << slime->GetHp() << "\n";
                    std::cout << "按 Enter 继续...";
                    std::cin.get();
                }

                if (!tc.player->IsAlive() || !slime->IsAlive()) break;
            }
        }

        std::cout << "\n最终: " << tc.className
            << " HP=" << tc.player->GetHp() << "/" << tc.player->GetMaxHp()
            << "  史莱姆 HP=" << slime->GetHp() << "/" << slime->GetMaxHp() << "\n";
    }

    std::cout << "\n按 Enter 继续...";
    std::cin.get();
}

// ================================================================
//  游戏主循环
// ================================================================
void GameManager::Run()
{
    while (true)
    {
        UpdateDashboard();
        UIManager::DisplayMainMenu();
        HandleAction();
    }
}

void GameManager::UpdateDashboard() const
{
    DisplayDashboard();
}

// ================================================================
//  标题画面
// ================================================================

void GameManager::ShowTitleScreen()
{
    while (true)
    {
        char choice = UIManager::ShowTitleScreen();

        switch (choice)
        {
        case '1':
        {
            // 开始新游戏 → 创建角色
            auto newPlayer = UIManager::CreateCharacterFlow();
            if (!newPlayer)
            {
                continue;  // 取消创建，重新显示标题画面
            }

            // 将新角色加入管理
            // 注意：CreateCharacterFlow 内部已经调用了 InitPlayerInventory
            AddCharacter(newPlayer);
            EnterGame();
            return;
        }
        break;

        case '0':
            std::cout << std::endl << "感谢游玩！再见！" << std::endl;
            exit(0);
            break;

        default:
            break;
        }
    }
}

void GameManager::EnterGame()
{
    UIManager::EnterGameScreen(currentCharacter);
    Run();
}