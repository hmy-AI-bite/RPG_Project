#include "GameManager.h"
#include <iostream>
#include <iomanip>

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
        {
            currentCharacter = character;
        }
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
        
        std::cout << "├─ 属性面板 ──────────────────────────────────────────────────┤" << std::endl;
        
        std::cout << "│ 攻击力 (ATK): " << std::setw(45) << std::left << currentCharacter->GetAttackPower() << "│" << std::endl;
        std::cout << "│ 防御力 (DEF): " << std::setw(45) << std::left << currentCharacter->GetDefend() << "│" << std::endl;
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
    std::cout << "║ 2. 对当前角色造成伤害（测试）                                  ║" << std::endl;
    std::cout << "║ 3. 治疗当前角色（测试）                                        ║" << std::endl;
    std::cout << "║ 4. 恢复蓝量（测试）                                            ║" << std::endl;
    std::cout << "║ 5. 释放技能（消耗蓝量）                                        ║" << std::endl;
    std::cout << "║ 6. 查看当前角色详细信息                                        ║" << std::endl;
    std::cout << "║ 7. 刷新面板                                                    ║" << std::endl;
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
    int choice;
    std::cin >> choice;

    if (!currentCharacter) return;

    Player* player = dynamic_cast<Player*>(currentCharacter.get());

    switch (choice)
    {
    case 1:
        DisplayCharacterList();
        int charIndex;
        if (std::cin >> charIndex)
        {
            SetCurrentCharacter(charIndex);
        }
        break;

    case 2:
    {
        std::cout << "请输入伤害值: ";
        int damage;
        std::cin >> damage;
        currentCharacter->TakeDamage(damage);
        std::cout << "按任意键继续...";
        std::cin.ignore();
        std::cin.get();
    }
    break;

    case 3:
    {
        std::cout << "请输入治疗量: ";
        int heal;
        std::cin >> heal;
        currentCharacter->Heal(heal);
        std::cout << "按任意键继续...";
        std::cin.ignore();
        std::cin.get();
    }
    break;

    case 4:
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

    case 5:
        if (player)
        {
            std::cout << "请输入技能消耗的蓝量: ";
            int mpCost;
            std::cin >> mpCost;
            
            Mage* mage = dynamic_cast<Mage*>(player);
            if (mage)
            {
                mage->CastSpell(mpCost);
            }
            else
            {
                // 一般Player直接消耗蓝量
                if (player->HasEnoughMp(mpCost))
                {
                    player->ConsumeMp(mpCost);
                    std::cout << player->GetName() << " 施放了一个技能！" << std::endl;
                }
                else
                {
                    std::cout << player->GetName() << " 蓝量不足！" << std::endl;
                }
            }
            std::cout << "按任意键继续...";
            std::cin.ignore();
            std::cin.get();
        }
        else
        {
            std::cout << "该角色不支持蓝量系统！" << std::endl;
        }
        break;

    case 6:
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

    case 7:
        // 刷新面板，继续循环即可
        break;

    case 0:
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

void GameManager::Run()
{
    while (true)
    {
        UpdateDashboard();
        DisplayMainMenu();
        HandleAction();
    }
}
