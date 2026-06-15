#include "GameManager.h"
#include "SaveLoadManager.h"
#include "Shop.h"

#include <iostream>
#include <iomanip>

GameManager::GameManager() : currentCharacter(nullptr)
{
    // 加载排行榜
    SaveLoadManager::LoadLeaderboard();
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
        {
            currentCharacter = character;
        }
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
    {
        currentCharacter = characters[index];
    }
}

void GameManager::ClearScreen() const
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void GameManager::DisplayDashboard() const
{
    ClearScreen();
    
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    RPG 游戏 - 主控面板                          ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;

    if (currentCharacter)
    {
        std::cout << "┌─ 当前角色信息 ─────────────────────────────────────────────┐" << std::endl;
        
        // 角色名称和等级
        std::cout << "│ " << std::setw(60) << std::left << currentCharacter->GetName() + " [LV." + std::to_string(currentCharacter->GetLevel()) + "]" << "│" << std::endl;
        
        // HP 条形图
        int hpPercent = (currentCharacter->GetHp() * 100) / currentCharacter->GetMaxHp();
        int barLength = 40;
        int filledLength = (hpPercent * barLength) / 100;
        
        std::cout << "│ 生命值: [";
        for (int i = 0; i < barLength; ++i)
        {
            if (i < filledLength)
                std::cout << "█";
            else
                std::cout << "░";
        }
        std::cout << "] " << std::setw(3) << std::right << hpPercent << "% │" << std::endl;
        
        std::cout << "│     " << std::setw(5) << std::left << currentCharacter->GetHp() 
                  << "/" << std::setw(5) << std::left << currentCharacter->GetMaxHp() << "                                        │" << std::endl;
        
        // MP 条形图
        Player* player = dynamic_cast<Player*>(currentCharacter.get());
        if (player)
        {
            int mpPercent = (player->GetMp() * 100) / player->GetMaxMp();
            int mpFilledLength = (mpPercent * barLength) / 100;
            
            std::cout << "│ 蓝量: [";
            for (int i = 0; i < barLength; ++i)
            {
                if (i < mpFilledLength)
                    std::cout << "▓";
                else
                    std::cout << "░";
            }
            std::cout << "] " << std::setw(3) << std::right << mpPercent << "% │" << std::endl;
            
            std::cout << "│     " << std::setw(5) << std::left << player->GetMp() 
                      << "/" << std::setw(5) << std::left << player->GetMaxMp() << "                                        │" << std::endl;
        }
        
        // 行动条显示
        int gaugePercent = currentCharacter->GetGaugePercent();
        int gaugeBarLength = 40;
        int gaugeFilled = (gaugePercent * gaugeBarLength) / 100;
        
        std::cout << "│ 行动条: [";
        for (int i = 0; i < gaugeBarLength; ++i)
        {
            if (i < gaugeFilled)
                std::cout << "▌";
            else
                std::cout << "─";
        }
        std::cout << "] " << std::setw(3) << std::right << (gaugePercent > 100 ? 100 : gaugePercent) << "% │" << std::endl;
        
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

    std::cout << std::endl;
}

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
    std::cout << "║ B. 显示排行榜                                                  ║" << std::endl;
    std::cout << "║ I. 打开背包                                                    ║" << std::endl;
    std::cout << "║ P. 进入商店                                                    ║" << std::endl;
    std::cout << "║ R. 刷新面板                                                    ║" << std::endl;
    std::cout << "║ 0. 退出游戏                                                    ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "请输入你的选择: ";
}

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

void GameManager::HandleAction()
{
    char choice;
    std::cin >> choice;

    if (!currentCharacter) return;

    Player* player = dynamic_cast<Player*>(currentCharacter.get());

    switch (choice)
    {
    case '1':
        DisplayCharacterList();
        int charIndex;
        if (std::cin >> charIndex)
        {
            SetCurrentCharacter(charIndex);
        }
        break;

    case '2':
    {
        // 普通攻击
        if (player)
        {
            ClearScreen();
            std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║                    普通攻击（平A）                             ║" << std::endl;
            std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;

            std::cout << "║ 可攻击目标：" << std::endl;
            for (size_t i = 0; i < characters.size(); ++i)
            {
                if (characters[i] != currentCharacter)
                {
                    std::cout << "║ [" << i << "] " << characters[i]->GetName() << std::endl;
                }
            }

            std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
            std::cout << "请选择目标编号: ";

            int defenderIdx;
            if (!(std::cin >> defenderIdx) || defenderIdx < 0 || defenderIdx >= static_cast<int>(characters.size()))
            {
                std::cout << "无效选择！" << std::endl;
                return;
            }

            if (defenderIdx == static_cast<int>(characters.size()) || characters[defenderIdx] == currentCharacter)
            {
                std::cout << "不能攻击自己！" << std::endl;
                return;
            }

            std::cout << std::endl;
            player->NormalAttack(characters[defenderIdx].get());
            std::cout << std::endl;
            std::cout << "按任意键继续...";
            std::cin.ignore();
            std::cin.get();
        }
        else
        {
            std::cout << "该角色不支持攻击！" << std::endl;
        }
    }
    break;

    case '3':
    {
        // 造成伤害（测试）
        std::cout << "请输入伤害值: ";
        int damage;
        std::cin >> damage;
        currentCharacter->TakeDamage(damage);
        std::cout << "按任意键继续...";
        std::cin.ignore();
        std::cin.get();
    }
    break;

    case '4':
    {
        // 治疗当前角色
        std::cout << "请输入治疗量: ";
        int heal;
        std::cin >> heal;
        currentCharacter->Heal(heal);
        std::cout << "按任意键继续...";
        std::cin.ignore();
        std::cin.get();
    }
    break;

    case '5':
        // 恢复蓝量
        if (player)
        {
            std::cout << "请输入恢复蓝量: ";
            int restoreMp;
            std::cin >> restoreMp;
            player->RestoreMp(restoreMp);
            std::cout << "按任意键继续...";
            std::cin.ignore();
            std::cin.get();
        }
        else
        {
            std::cout << "该角色不支持蓝量系统！" << std::endl;
        }
        break;

    case '6':
    {
        // 释放职业技能
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
                else
                {
                    std::cout << "║ 蓝量不足！" << std::endl;
                }
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
                    braver->ExecuteTrueAttack();
                    std::cout << " ║" << std::endl;
                    std::cout << "║ 无视防御和抗性！" << std::endl;
                }
                else
                {
                    std::cout << "║ 蓝量不足！" << std::endl;
                }
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
                else
                {
                    std::cout << "║ 蓝量不足！" << std::endl;
                }
            }
            
            std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
            std::cout << "按任意键继续...";
            std::cin.ignore();
            std::cin.get();
        }
        else
        {
            std::cout << "该角色不支持技能系统！" << std::endl;
        }
    }
    break;

    case '7':
        ClearScreen();
        std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                      角色详细信息                             ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║ ";
        currentCharacter->DisplayInfo();
        std::cout << "║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << "按任意键继续...";
        std::cin.ignore();
        std::cin.get();
        break;

    case '8':
        DemonstrateDamageSystem();
        break;

    case '9':
    {
        // 查看行动条
        ClearScreen();
        std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                      所有角色行动条                           ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;

        for (size_t i = 0; i < characters.size(); ++i)
        {
            int gaugePercent = characters[i]->GetGaugePercent();
            int barLength = 30;
            int filled = (gaugePercent * barLength) / 100;

            std::cout << "║ " << std::setw(10) << std::left << characters[i]->GetName() << " [";
            for (int j = 0; j < barLength; ++j)
            {
                if (j < filled)
                    std::cout << "█";
                else
                    std::cout << "░";
            }
            std::cout << "] " << std::setw(3) << std::right << (gaugePercent > 100 ? 100 : gaugePercent) << "% ║" << std::endl;
        }

        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║ 提示: 行动条满100%时可以行动                                   ║" << std::endl;
        std::cout << "║ 速度越高，行动条填充越快                                       ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << "按任意键继续...";
        std::cin.ignore();
        std::cin.get();
    }
    break;

    case 'A':
    case 'a':
        DisplayEnemies();
        break;

    case 'T':
    case 't':
        TestSlime();
        std::cout << "按任意键继续...";
        std::cin.ignore();
        std::cin.get();
        break;

    case 'S':
    case 's':
    {
        // 保存游戏
        Player* player = dynamic_cast<Player*>(currentCharacter.get());
        if (player)
        {
            SaveLoadManager::SaveGame(*player);
        }
        else
        {
            std::cout << "当前角色无法保存！" << std::endl;
        }
        std::cout << "按任意键继续...";
        std::cin.ignore();
        std::cin.get();
    }
    break;

    case 'L':
    case 'l':
    {
        // 加载游戏
        Player* player = dynamic_cast<Player*>(currentCharacter.get());
        if (player)
        {
            if (SaveLoadManager::LoadGame(*player))
            {
                std::cout << "存档加载成功！" << std::endl;
            }
        }
        else
        {
            std::cout << "当前角色无法加载存档！" << std::endl;
        }
        std::cout << "按任意键继续...";
        std::cin.ignore();
        std::cin.get();
    }
    break;

    case 'B':
    case 'b':
    {
        // 显示排行榜
        SaveLoadManager::ShowLeaderboard();
        std::cout << "按任意键继续...";
        std::cin.ignore();
        std::cin.get();
    }
    break;

    case 'I':
    case 'i':
    {
        // 打开背包
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
        else
        {
            std::cout << "当前角色不是玩家！" << std::endl;
            std::cout << "按任意键继续...";
            std::cin.ignore();
            std::cin.get();
        }
    }
    break;

    case 'P':
    case 'p':
    {
        // 进入商店
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
                
                char choice;
                std::cin >> choice;
                
                switch (choice)
                {
                case 'B':
                case 'b':
                {
                    std::cout << "请输入要购买的道具序号: ";
                    int index;
                    std::cin >> index;
                    shop.BuyItem(*player, index);
                    std::cout << "按任意键继续...";
                    std::cin.ignore();
                    std::cin.get();
                }
                break;
                
                case 'S':
                case 's':
                {
                    std::cout << "请输入要出售的背包道具序号: ";
                    int index;
                    std::cin >> index;
                    shop.SellItem(*player, index);
                    std::cout << "按任意键继续...";
                    std::cin.ignore();
                    std::cin.get();
                }
                break;
                
                case 'Q':
                case 'q':
                    inShop = false;
                    break;
                }
            }
        }
        else
        {
            std::cout << "当前角色不是玩家！" << std::endl;
            std::cout << "按任意键继续...";
            std::cin.ignore();
            std::cin.get();
        }
    }
    break;

    case 'R':
    case 'r':
        // 刷新面板，继续循环即可
        break;

    case '0':
        std::cout << "感谢游玩！再见！" << std::endl;
        exit(0);
        break;

    default:
        std::cout << "无效选择！请重新输入。" << std::endl;
        std::cout << "按任意键继续...";
        std::cin.ignore();
        std::cin.get();
    }
}

void GameManager::UpdateDashboard() const
{
    DisplayDashboard();
}

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
    {
        std::cout << "║ [" << i << "] " << std::setw(58) << std::left << characters[i]->GetName() << "║" << std::endl;
    }

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
    std::cout << "按任意键继续...";
    std::cin.ignore();
    std::cin.get();
}

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
        1.5,  // 技能倍率
        attacker->GetElementType(),
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
        50,  // 法术攻击力
        defender->GetSpellDefense(),
        attacker->GetLevel(),
        defender->GetLevel(),
        1.2,  // 技能倍率
        attacker->GetElementType(),
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
        20,  // 固定伤害值
        attacker->GetAttackPower(),
        attacker->GetLevel(),
        defender->GetLevel()
    );

    std::cout << "固定伤害值: 20" << std::endl;
    std::cout << "攻击力倍数伤害: " << std::fixed << std::setprecision(2) << (attacker->GetAttackPower() * 0.5) << std::endl;
    std::cout << "最终伤害: " << static_cast<int>(trueDamage.finalDamage) << std::endl;
    std::cout << "（无视防御和抗性）" << std::endl;
}

void GameManager::Run()
{
    while (true)
    {
        UpdateDashboard();
        DisplayMainMenu();
        HandleAction();
    }
}

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
    std::cout << "按任意键继续...";
    std::cin.ignore();
    std::cin.get();
}

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

    // 创建不同等级的史莱姆用于测试
    std::cout << "║ 生成史莱姆..." << std::endl;
    auto slime1 = std::make_shared<Slime>(1);  // 1级史莱姆
    auto slime2 = std::make_shared<Slime>(3);  // 3级史莱姆
    auto slime3 = std::make_shared<Slime>(5);  // 5级史莱姆

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
    // 降低史莱姆血量到分裂阈值
    while (slime2->GetHp() > slime2->GetMaxHp() / 2)
    {
        slime2->TakeDamage(10);
    }
    slime2->PerformAction(currentCharacter.get());

    std::cout << std::endl << "║ 测试结束！" << std::endl;
    
    // 战斗胜利后给予经验值奖励
    if (player)
    {
        int expReward = 50;  // 测试战斗奖励50经验
        std::cout << "║ 战斗胜利！获得 " << expReward << " 经验值！" << std::endl;
        player->GainExperience(expReward);
        
        // 记录排行榜
        SaveLoadManager::AddScore(player->GetName(), player->GetExperience());
    }
    
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
}
