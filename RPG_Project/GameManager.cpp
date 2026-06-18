#include "GameManager.h"
#include "SaveLoadManager.h"
#include "Shop.h"
#include "DamageSystem.h"
#include "Item.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>

// ================================================================
//  UI 工具方法 - 画界面用的小帮手
// ================================================================

// ---------- 等待按键 ----------
// 让程序暂停，等玩家按 Enter 键再继续
// 防止信息一闪而过看不清
void GameManager::WaitForKeyPress()
{
    std::cout << "按任意键继续...";
    std::cin.ignore();
    std::cin.get();
}

// ---------- 画进度条 ----------
// 比如 HP 75%，进度条长度 40：
//   输出：██████████████████████████████░░░░░░░░░░
//         |← 30 个实心 = 75% →|← 10 个空心 →|
void GameManager::DrawProgressBar(int percent, int barLength,
    const std::string& filledChar, const std::string& emptyChar)
{
    int filled = (percent * barLength) / 100;   // 计算实心部分有几个字符
    for (int i = 0; i < barLength; ++i)
    {
        std::cout << (i < filled ? filledChar : emptyChar);
    }
}


// ================================================================
//  GameManager - 游戏管理器
// ================================================================

// ---------- 构造函数 ----------
GameManager::GameManager() : currentCharacter(nullptr)
{
}

GameManager::~GameManager()
{
}

// ---------- 添加角色 ----------
// 把角色加入队伍，如果是第一个角色就自动设为当前角色
void GameManager::AddCharacter(std::shared_ptr<Character> character)
{
    if (character)
    {
        characters.push_back(character);
        if (!currentCharacter)
        {
            currentCharacter = character;   // 第一个角色默认选中
        }
    }
}

// ---------- 添加怪物 ----------
void GameManager::AddEnemy(std::shared_ptr<Enemy> enemy)
{
    if (enemy)
    {
        enemies.push_back(enemy);
    }
}

// ---------- 切换当前角色 ----------
void GameManager::SetCurrentCharacter(int index)
{
    if (index >= 0 && index < static_cast<int>(characters.size()))
    {
        currentCharacter = characters[index];
    }
}

// ---------- 清屏 ----------
void GameManager::ClearScreen() const
{
#ifdef _WIN32
    system("cls");      // Windows 清屏命令
#else
    system("clear");    // Linux/Mac 清屏命令
#endif
}


// ================================================================
//  主控面板 - 显示当前角色的完整状态
//  包括：HP 血条、MP 蓝条、行动条、各项属性
// ================================================================
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
        std::cout << "│ " << std::setw(60) << std::left
                  << currentCharacter->GetName() + " [LV." + std::to_string(currentCharacter->GetLevel()) + "]"
                  << "│" << std::endl;

        // HP 血条（实心 = █，空心 = ░）
        int hpPercent = (currentCharacter->GetHp() * 100) / currentCharacter->GetMaxHp();
        std::cout << "│ 生命值: [";
        DrawProgressBar(hpPercent, 40, "█", "░");
        std::cout << "] " << std::setw(3) << std::right << hpPercent << "% │" << std::endl;
        std::cout << "│     " << std::setw(5) << std::left << currentCharacter->GetHp()
                  << "/" << std::setw(5) << std::left << currentCharacter->GetMaxHp()
                  << "                                        │" << std::endl;

        // MP 蓝条（只有 Player 才有蓝量）
        Player* player = dynamic_cast<Player*>(currentCharacter.get());
        if (player)
        {
            int mpPercent = (player->GetMp() * 100) / player->GetMaxMp();
            std::cout << "│ 蓝量: [";
            DrawProgressBar(mpPercent, 40, "▓", "░");
            std::cout << "] " << std::setw(3) << std::right << mpPercent << "% │" << std::endl;
            std::cout << "│     " << std::setw(5) << std::left << player->GetMp()
                      << "/" << std::setw(5) << std::left << player->GetMaxMp()
                      << "                                        │" << std::endl;

            // 经验条（EXP）
            int expNeed = player->GetExpToNextLevel();
            int expPercent = (expNeed > 0) ? (player->GetExperience() * 100) / expNeed : 100;
            if (expPercent > 100) expPercent = 100;
            std::cout << "│ 经验值: [";
            DrawProgressBar(expPercent, 40, "▒", "░");
            std::cout << "] " << std::setw(3) << std::right << expPercent << "% │" << std::endl;
            std::cout << "│     " << std::setw(5) << std::left << player->GetExperience()
                      << "/" << std::setw(5) << std::left << expNeed
                      << " (Lv." << player->GetLevel() << " → " << (player->GetLevel() + 1) << ")                                     │" << std::endl;
        }

        // 行动条（满 100% 就能行动）
        int gaugePercent = currentCharacter->GetGaugePercent();
        int displayPercent = (gaugePercent > 100) ? 100 : gaugePercent;
        std::cout << "│ 行动条: [";
        DrawProgressBar(displayPercent, 40, "▌", "─");
        std::cout << "] " << std::setw(3) << std::right << displayPercent << "% │" << std::endl;

        // 属性面板
        std::cout << "├─ 属性面板 ──────────────────────────────────────────────────┤" << std::endl;
        std::cout << "│ 攻击力 (ATK): " << std::setw(45) << std::left << currentCharacter->GetAttackPower() << "│" << std::endl;
        std::cout << "│ 防御力 (DEF): " << std::setw(45) << std::left << currentCharacter->GetDefend() << "│" << std::endl;
        std::cout << "│ 速度 (SPD):   " << std::setw(45) << std::left << currentCharacter->GetSpeed() << "│" << std::endl;
        std::cout << "│ 灵巧 (AGI):   " << std::setw(45) << std::left << currentCharacter->GetAgility() << "│" << std::endl;
        std::cout << "│ 等级 (LVL):   " << std::setw(45) << std::left << currentCharacter->GetLevel() << "│" << std::endl;
        if (player)
        {
            std::cout << "│ 金币 (GOLD):  " << std::setw(45) << std::left << player->GetGold() << "│" << std::endl;
            std::cout << "│ 可用属性点:   " << std::setw(45) << std::left << player->GetAvailableAP() << "│" << std::endl;
        }

        std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;
    }
    else
    {
        std::cout << "│ 没有可用的角色" << std::endl;
    }

    std::cout << std::endl;
}


// ================================================================
//  主菜单 - 显示所有可用的操作
// ================================================================
void GameManager::DisplayMainMenu() const
{
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                        主菜单                                  ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ 1. 探索 / 战斗 (遭遇怪物)                                      ║" << std::endl;
    std::cout << "║ 2. 查看角色列表                                                ║" << std::endl;
    std::cout << "║ 3. 使用技能                                                    ║" << std::endl;
    std::cout << "║ 4. 打开背包 (使用道具)                                         ║" << std::endl;
    std::cout << "║ 5. 进入商店                                                    ║" << std::endl;
    std::cout << "║ 6. 属性分配 (使用 AP 加点)                                      ║" << std::endl;
    std::cout << "║ 7. 查看详细信息                                                ║" << std::endl;
    std::cout << "║ S. 保存游戏                                                    ║" << std::endl;
    std::cout << "║ L. 加载游戏                                                    ║" << std::endl;
    std::cout << "║ 0. 返回标题画面                                                ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "请输入你的选择: ";
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
        // 当前选中的角色前面有 "→" 标记
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
//  处理操作 - 根据玩家的菜单选择执行对应的功能
//  返回值: true = 继续游戏, false = 返回标题画面
// ================================================================
bool GameManager::HandleAction()
{
    char choice;
    std::cin >> choice;

    if (!currentCharacter) return true;

    Player* player = dynamic_cast<Player*>(currentCharacter.get());

    switch (choice)
    {
    // ===== 1. 探索 / 战斗 =====
    case '1':
    {
        if (player)
        {
            BattleEncounter();
        }
        else
        {
            std::cout << "当前角色无法进行战斗！" << std::endl;
            WaitForKeyPress();
        }
    }
    break;

    // ===== 2. 查看角色列表 =====
    case '2':
    {
        DisplayCharacterList();
        int charIndex;
        if (std::cin >> charIndex)
        {
            SetCurrentCharacter(charIndex);
        }
        break;
    }

    // ===== 3. 使用技能 =====
    case '3':
    {
        if (player)
        {
            UseSkillInteractively(player);
        }
        else
        {
            std::cout << "当前角色不支持技能系统！" << std::endl;
            WaitForKeyPress();
        }
    }
    break;

    // ===== 4. 打开背包 =====
    case '4':
    {
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
            WaitForKeyPress();
        }
    }
    break;

    // ===== 5. 进入商店 =====
    case '5':
    {
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
                case 'B':
                case 'b':
                {
                    std::cout << "请输入要购买的道具序号: ";
                    int index;
                    std::cin >> index;
                    shop.BuyItem(*player, index);
                    WaitForKeyPress();
                }
                break;

                case 'S':
                case 's':
                {
                    std::cout << "请输入要出售的背包道具序号: ";
                    int index;
                    std::cin >> index;
                    shop.SellItem(*player, index);
                    WaitForKeyPress();
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
            WaitForKeyPress();
        }
    }
    break;

    // ===== 6. 属性分配 =====
    case '6':
    {
        if (!player)
        {
            std::cout << "当前角色不是玩家！" << std::endl;
            WaitForKeyPress();
            break;
        }

        bool allocating = true;
        while (allocating && player->GetAvailableAP() > 0)
        {
            ClearScreen();
            std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║                      属性分配界面                              ║" << std::endl;
            std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
            std::cout << "║  可用属性点 (AP): " << std::setw(46) << std::left << player->GetAvailableAP() << "║" << std::endl;
            std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
            std::cout << "║  当前属性:                                                      ║" << std::endl;
            std::cout << "║    HP: " << std::setw(8) << std::left << (std::to_string(player->GetHp()) + "/" + std::to_string(player->GetMaxHp()))
                      << "  MP: " << std::setw(8) << std::left << (std::to_string(player->GetMp()) + "/" + std::to_string(player->GetMaxMp()))
                      << "  ATK: " << player->GetAttackPower() << "  ║" << std::endl;
            std::cout << "║    DEF: " << std::setw(6) << std::left << player->GetDefend()
                      << "  SPD: " << std::setw(6) << std::left << player->GetSpeed()
                      << "  AGI: " << std::setw(6) << std::left << player->GetAgility()
                      << "  S.DEF: " << player->GetSpellDefense() << "  ║" << std::endl;

            Mage* mage = dynamic_cast<Mage*>(player);
            Braver* braver = dynamic_cast<Braver*>(player);
            Guardian* guardian = dynamic_cast<Guardian*>(player);

            if (mage)
            {
                std::cout << "║    法术强度: " << std::setw(50) << std::left << mage->GetSpellPower() << "║" << std::endl;
                std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
                std::cout << "║  每点 AP 效果 (法师 - 偏法术向):                                   ║" << std::endl;
                std::cout << "║    [1] HP+4   [2] MP+7   [3] ATK+1   [4] DEF+1                ║" << std::endl;
                std::cout << "║    [5] SPD+2  [6] AGI+2  [7] S.DEF+2 [8] 法术强度+3            ║" << std::endl;
            }
            else if (braver)
            {
                std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
                std::cout << "║  每点 AP 效果 (勇者 - 偏攻击向):                                   ║" << std::endl;
                std::cout << "║    [1] HP+6   [2] MP+4   [3] ATK+3   [4] DEF+1                ║" << std::endl;
                std::cout << "║    [5] SPD+2  [6] AGI+2  [7] S.DEF+1                          ║" << std::endl;
            }
            else if (guardian)
            {
                std::cout << "║    护盾值: " << std::setw(50) << std::left << guardian->GetShield() << "║" << std::endl;
                std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
                std::cout << "║  每点 AP 效果 (守护骑士 - 偏防御向):                                 ║" << std::endl;
                std::cout << "║    [1] HP+12  [2] MP+5   [3] ATK+1   [4] DEF+3                ║" << std::endl;
                std::cout << "║    [5] SPD+1  [6] AGI+1  [7] S.DEF+2                          ║" << std::endl;
            }
            else
            {
                std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
                std::cout << "║  每点 AP 效果 (战士 - 均衡型):                                     ║" << std::endl;
                std::cout << "║    [1] HP+8   [2] MP+5   [3] ATK+2   [4] DEF+1                ║" << std::endl;
                std::cout << "║    [5] SPD+1  [6] AGI+1  [7] S.DEF+1                          ║" << std::endl;
            }

            std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
            std::cout << "║  输入属性编号分配 1 点 AP，或输入 Q 退出分配                       ║" << std::endl;
            std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
            std::cout << "请输入你的选择: ";

            char attrChoice;
            std::cin >> attrChoice;

            if (attrChoice == 'Q' || attrChoice == 'q')
            {
                allocating = false;
                break;
            }

            int attrIndex = attrChoice - '0';
            if (attrIndex >= 1 && attrIndex <= 8)
            {
                player->AllocateAttribute(attrIndex);
            }
            else
            {
                std::cout << "无效的属性编号！" << std::endl;
            }

            if (player->GetAvailableAP() <= 0)
            {
                std::cout << "属性点已用完！" << std::endl;
                allocating = false;
            }

            WaitForKeyPress();
        }

        if (player->GetAvailableAP() <= 0)
        {
            std::cout << "当前没有可分配的属性点。升级后可获得更多 AP！" << std::endl;
            WaitForKeyPress();
        }
    }
    break;

    // ===== 7. 查看详细信息 =====
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

    // ===== S. 保存游戏 =====
    case 'S':
    case 's':
    {
        Player* player = dynamic_cast<Player*>(currentCharacter.get());
        if (player)
        {
            SaveLoadManager::SaveGame(*player);
        }
        else
        {
            std::cout << "当前角色无法保存！" << std::endl;
        }
        WaitForKeyPress();
    }
    break;

    // ===== L. 加载游戏 =====
    case 'L':
    case 'l':
    {
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
        WaitForKeyPress();
    }
    break;

    // ===== 0. 返回标题画面 =====
    case '0':
        std::cout << "返回标题画面..." << std::endl;
        WaitForKeyPress();
        return false;

    // ===== 无效输入 =====
    default:
        std::cout << "无效选择！请重新输入。" << std::endl;
        WaitForKeyPress();
    }
    return true;
}


// ================================================================
//  标题画面 - 游戏主界面
//  显示游戏 Logo，提供：开始游戏 / 加载存档 / 退出
// ================================================================
void GameManager::ShowTitleScreen()
{
    ClearScreen();
    std::cout << std::endl;
    std::cout << "                                                              " << std::endl;
    std::cout << "     ╔══════════════════════════════════════════════════╗     " << std::endl;
    std::cout << "     ║                                                ║     " << std::endl;
    std::cout << "     ║        ★ 幻 想 勇 者 物 语 ★                   ║     " << std::endl;
    std::cout << "     ║         Fantasy Brave Saga                     ║     " << std::endl;
    std::cout << "     ║                                                ║     " << std::endl;
    std::cout << "     ║         ATB 回合制战斗 RPG                      ║     " << std::endl;
    std::cout << "     ║         v1.0                                    ║     " << std::endl;
    std::cout << "     ║                                                ║     " << std::endl;
    std::cout << "     ╚══════════════════════════════════════════════════╝     " << std::endl;
    std::cout << std::endl;
    std::cout << "     ╔══════════════════════════════════════════════════╗     " << std::endl;
    std::cout << "     ║         1. 开始新游戏                             ║     " << std::endl;
    std::cout << "     ║         2. 加载存档                               ║     " << std::endl;
    std::cout << "     ║         0. 退出游戏                               ║     " << std::endl;
    std::cout << "     ╚══════════════════════════════════════════════════╝     " << std::endl;
    std::cout << std::endl;
    std::cout << "     请输入你的选择: ";

    char choice;
    std::cin >> choice;
    std::cin.ignore();

    switch (choice)
    {
    case '1':
        // 开始新游戏 → 创建角色
        CreateCharacterFlow();
        break;

    case '2':
    {
        // 加载存档
        ClearScreen();
        std::cout << std::endl;
        std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                      加载存档                                  ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;

        // 先创建一个临时角色用于加载
        std::cout << "║ 请输入存档文件名（默认 save.dat）: " << std::endl;
        std::cout << "║ ";
        std::string filename;
        std::getline(std::cin, filename);
        if (filename.empty()) filename = "save.dat";

        // 用 Braver 作为临时载体来加载
        auto tempPlayer = std::make_shared<Braver>("临时角色");
        if (SaveLoadManager::LoadGame(*tempPlayer, filename))
        {
            std::cout << "║" << std::endl;
            std::cout << "║ 存档加载成功！欢迎回来，" << tempPlayer->GetName() << "！" << std::endl;
            std::cout << "║" << std::endl;
            AddCharacter(tempPlayer);
            currentCharacter = tempPlayer;
            playerNickname = tempPlayer->GetName();

            // 给初始技能（如果存档里没有技能的话）
            if (tempPlayer->GetSkills().empty())
            {
                SkillLearningFlow(tempPlayer.get());
            }

            std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
            WaitForKeyPress();
            EnterGame();
        }
        else
        {
            std::cout << "║" << std::endl;
            std::cout << "║ 存档加载失败或文件不存在！" << std::endl;
            std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
            WaitForKeyPress();
            ShowTitleScreen();  // 返回标题画面
        }
    }
    break;

    case '0':
        std::cout << std::endl << "感谢游玩！再见！" << std::endl;
        exit(0);
        break;

    default:
        ShowTitleScreen();  // 无效输入，重新显示
        break;
    }
}


// ================================================================
//  创建角色流程
//  步骤：选择职业 → 输入昵称 → 确认创建 → 初始物品 → 技能学习
// ================================================================
void GameManager::CreateCharacterFlow()
{
    ClearScreen();

    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    创建你的角色                                ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║  ★ 第一步：选择职业                                           ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║  ┌───────────────────────────────────────────────────────┐    ║" << std::endl;
    std::cout << "║  │ [1] 勇者 (Braver)                                      │    ║" << std::endl;
    std::cout << "║  │     高攻击、中速度、高暴击率                            │    ║" << std::endl;
    std::cout << "║  │     HP:140  ATK:35  DEF:6  SPD:110  AGI:120            │    ║" << std::endl;
    std::cout << "║  │     水属性 | 怒气系统 | 真实伤害技能                     │    ║" << std::endl;
    std::cout << "║  │                                                       │    ║" << std::endl;
    std::cout << "║  │ [2] 法师 (Mage)                                        │    ║" << std::endl;
    std::cout << "║  │     高法术强度、高速度、高暴击                           │    ║" << std::endl;
    std::cout << "║  │     HP:90  ATK:12  DEF:5  SPD:120  AGI:110             │    ║" << std::endl;
    std::cout << "║  │     火属性 | 法术强度系统 | 元素魔法技能                  │    ║" << std::endl;
    std::cout << "║  │                                                       │    ║" << std::endl;
    std::cout << "║  │ [3] 守护骑士 (Guardian)                                 │    ║" << std::endl;
    std::cout << "║  │     高血量、高防御、低速（坦克）                          │    ║" << std::endl;
    std::cout << "║  │     HP:150  ATK:18  DEF:20  SPD:80  AGI:30             │    ║" << std::endl;
    std::cout << "║  │     地属性 | 护盾系统 | 守护/圣光技能                     │    ║" << std::endl;
    std::cout << "║  └───────────────────────────────────────────────────────┘    ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "  请选择职业 (1/2/3): ";

    int classChoice;
    if (!(std::cin >> classChoice) || classChoice < 1 || classChoice > 3)
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "  无效选择！请重新选择。" << std::endl;
        WaitForKeyPress();
        CreateCharacterFlow();
        return;
    }
    std::cin.ignore();

    // -------- 第二步：输入昵称 --------
    ClearScreen();
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    创建你的角色                                ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;

    std::string className;
    switch (classChoice)
    {
    case 1: className = "勇者 (Braver)"; break;
    case 2: className = "法师 (Mage)"; break;
    case 3: className = "守护骑士 (Guardian)"; break;
    }

    std::cout << "║  已选择职业: " << std::setw(47) << std::left << className << "║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║  ★ 第二步：输入角色昵称                                        ║" << std::endl;
    std::cout << "║     （最多 10 个字符，支持中文和英文）                           ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "  请输入角色昵称: ";

    std::string nickname;
    std::getline(std::cin, nickname);

    // 昵称不能为空
    if (nickname.empty())
    {
        std::cout << std::endl << "  昵称不能为空！请重新输入。" << std::endl;
        WaitForKeyPress();
        CreateCharacterFlow();
        return;
    }

    // 昵称长度限制
    if (nickname.length() > 10)
    {
        nickname = nickname.substr(0, 10);
        std::cout << "  昵称过长，已自动截断为: " << nickname << std::endl;
    }

    playerNickname = nickname;

    // -------- 第三步：确认并创建角色 --------
    ClearScreen();
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    创建你的角色                                ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║  ★ 确认角色信息                                               ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║     昵称: " << std::setw(47) << std::left << nickname << "║" << std::endl;
    std::cout << "║     职业: " << std::setw(47) << std::left << className << "║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;

    // 根据职业创建角色
    std::shared_ptr<Player> newPlayer;
    switch (classChoice)
    {
    case 1:
        newPlayer = std::make_shared<Braver>(nickname);
        newPlayer->SetPhysicalResistance(0.0);
        newPlayer->SetMagicalResistance(0.05);
        break;
    case 2:
        newPlayer = std::make_shared<Mage>(nickname, 35);
        newPlayer->SetPhysicalResistance(0.0);
        newPlayer->SetMagicalResistance(0.15);
        break;
    case 3:
        newPlayer = std::make_shared<Guardian>(nickname);
        newPlayer->SetPhysicalResistance(0.1);
        newPlayer->SetMagicalResistance(0.1);
        break;
    }

    // 显示创建后的初始属性
    std::cout << "║     ──── 初始属性 ────                                        ║" << std::endl;
    std::cout << "║     HP: " << std::setw(5) << std::left << newPlayer->GetMaxHp()
              << "  ATK: " << std::setw(5) << std::left << newPlayer->GetAttackPower()
              << "  DEF: " << std::setw(5) << std::left << newPlayer->GetDefend() << "     ║" << std::endl;
    std::cout << "║     SPD: " << std::setw(4) << std::left << newPlayer->GetSpeed()
              << "  AGI: " << std::setw(5) << std::left << newPlayer->GetAgility()
              << "  MP: " << std::setw(5) << std::left << newPlayer->GetMaxMp() << "     ║" << std::endl;
    std::cout << "║     金币: " << std::setw(47) << std::left << newPlayer->GetGold() << "║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║  确认创建这个角色吗？(Y/N): ";

    char confirm;
    std::cin >> confirm;
    std::cin.ignore();

    if (confirm != 'Y' && confirm != 'y')
    {
        std::cout << "║  已取消创建。返回标题画面..." << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
        WaitForKeyPress();
        ShowTitleScreen();
        return;
    }

    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║  角色创建成功！                                                ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    WaitForKeyPress();

    // -------- 第四步：初始物品给与 --------
    ClearScreen();
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    初始物品发放                                ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║  作为冒险的起始装备，你获得了以下物品：                           ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║    • 小型治疗药水 ×2  (恢复20HP)                               ║" << std::endl;
    std::cout << "║    • 中型治疗药水 ×1  (恢复50HP)                               ║" << std::endl;
    std::cout << "║    • 匕首 ×1          (ATK+3, AGI+8, SPD+3)                   ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;

    InitPlayerInventory(newPlayer.get());

    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    WaitForKeyPress();

    // -------- 第五步：技能学习 --------
    SkillLearningFlow(newPlayer.get());

    // -------- 第六步：将角色加入队伍并进入游戏 --------
    AddCharacter(newPlayer);
    currentCharacter = newPlayer;

    EnterGame();
}


// ================================================================
//  技能学习流程
//  根据角色职业，学习对应的初始技能
// ================================================================
void GameManager::SkillLearningFlow(Player* player)
{
    if (!player) return;

    ClearScreen();
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    技能学习                                    ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;

    // 根据职业判断
    if (dynamic_cast<Braver*>(player))
    {
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║  ★ 勇者初始技能                                               ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║    [1] 刺客之刃 (TrueSkill)                                   ║" << std::endl;
        std::cout << "║        真实伤害 30 + 攻击力×0.5                               ║" << std::endl;
        std::cout << "║        消耗蓝量: 20 | 无视防御和抗性                            ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║    [2] 强力挥斩 (PhysicalSkill)                                ║" << std::endl;
        std::cout << "║        物理伤害 1.5x 倍率                                      ║" << std::endl;
        std::cout << "║        消耗蓝量: 15 | 中规中矩的物理技能                         ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║  请选择要学习的技能 (1/2/0=全学): ";

        char skillChoice;
        std::cin >> skillChoice;
        std::cin.ignore();

        switch (skillChoice)
        {
        case '1':
            player->AddSkill(std::make_shared<TrueSkill>("刺客之刃", 30, 20));
            break;
        case '2':
            player->AddSkill(std::make_shared<PhysicalSkill>("强力挥斩", 1.5, 15, ElementType::Neutral));
            break;
        case '0':
            player->AddSkill(std::make_shared<TrueSkill>("刺客之刃", 30, 20));
            player->AddSkill(std::make_shared<PhysicalSkill>("强力挥斩", 1.5, 15, ElementType::Neutral));
            break;
        default:
            player->AddSkill(std::make_shared<TrueSkill>("刺客之刃", 30, 20));
            player->AddSkill(std::make_shared<PhysicalSkill>("强力挥斩", 1.5, 15, ElementType::Neutral));
            break;
        }
    }
    else if (dynamic_cast<Mage*>(player))
    {
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║  ★ 法师初始技能                                               ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║    [1] 火焰术 (MagicalSkill)                                  ║" << std::endl;
        std::cout << "║        火属性魔法 1.8x 倍率 | 法术强度: 35                     ║" << std::endl;
        std::cout << "║        消耗蓝量: 30 | 克制风属性                               ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║    [2] 冰霜术 (MagicalSkill)                                  ║" << std::endl;
        std::cout << "║        水属性魔法 1.6x 倍率 | 法术强度: 30                     ║" << std::endl;
        std::cout << "║        消耗蓝量: 25 | 克制火属性                               ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║  请选择要学习的技能 (1/2/0=全学): ";

        char skillChoice;
        std::cin >> skillChoice;
        std::cin.ignore();

        switch (skillChoice)
        {
        case '1':
            player->AddSkill(std::make_shared<MagicalSkill>("火焰术", 1.8, 30, 35, ElementType::Fire));
            break;
        case '2':
            player->AddSkill(std::make_shared<MagicalSkill>("冰霜术", 1.6, 25, 30, ElementType::Water));
            break;
        case '0':
            player->AddSkill(std::make_shared<MagicalSkill>("火焰术", 1.8, 30, 35, ElementType::Fire));
            player->AddSkill(std::make_shared<MagicalSkill>("冰霜术", 1.6, 25, 30, ElementType::Water));
            break;
        default:
            player->AddSkill(std::make_shared<MagicalSkill>("火焰术", 1.8, 30, 35, ElementType::Fire));
            player->AddSkill(std::make_shared<MagicalSkill>("冰霜术", 1.6, 25, 30, ElementType::Water));
            break;
        }
    }
    else if (dynamic_cast<Guardian*>(player))
    {
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║  ★ 守护骑士初始技能                                           ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║    [1] 守护姿态 (GuardStance)                                  ║" << std::endl;
        std::cout << "║        消耗蓝量: 15 | 获得 30 点护盾值                         ║" << std::endl;
        std::cout << "║        护盾优先吸收伤害                                        ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║    [2] 圣光斩 (HolySlash)                                      ║" << std::endl;
        std::cout << "║        消耗蓝量: 20 | 1.2x 物理伤害 + 吸血                     ║" << std::endl;
        std::cout << "║        回复攻击力×0.3 点血量                                   ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║    [3] 盾击 (PhysicalSkill)                                    ║" << std::endl;
        std::cout << "║        地属性物理 1.3x 倍率 | 消耗蓝量: 18                      ║" << std::endl;
        std::cout << "║        利用盾牌进行强力的撞击                                   ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║  请选择要学习的技能 (1/2/3/0=全学): ";

        char skillChoice;
        std::cin >> skillChoice;
        std::cin.ignore();

        switch (skillChoice)
        {
        case '1':
            // 守护姿态是内置技能，这里给它加个物理技能作为替代
            player->AddSkill(std::make_shared<PhysicalSkill>("盾击", 1.3, 18, ElementType::Earth));
            break;
        case '2':
            // 圣光斩也是内置技能
            player->AddSkill(std::make_shared<PhysicalSkill>("盾击", 1.3, 18, ElementType::Earth));
            break;
        case '3':
            player->AddSkill(std::make_shared<PhysicalSkill>("盾击", 1.3, 18, ElementType::Earth));
            break;
        case '0':
            player->AddSkill(std::make_shared<PhysicalSkill>("盾击", 1.3, 18, ElementType::Earth));
            break;
        default:
            player->AddSkill(std::make_shared<PhysicalSkill>("盾击", 1.3, 18, ElementType::Earth));
            break;
        }
    }

    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║  技能学习完成！                                                ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    WaitForKeyPress();
}


// ================================================================
//  进入游戏 - 过渡画面后进入主循环
// ================================================================
void GameManager::EnterGame()
{
    ClearScreen();

    // 显示进入游戏的过渡画面
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║                   ★ 冒险开始！★                               ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;

    if (currentCharacter)
    {
        std::cout << "║    勇者 " << std::setw(46) << std::left << currentCharacter->GetName() << "║" << std::endl;
        std::cout << "║    踏上了拯救世界的旅程...                                     ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║    提示：                                                      ║" << std::endl;
        std::cout << "║    • 按 1 探索野外 / 遭遇怪物战斗                               ║" << std::endl;
        std::cout << "║    • 按 4 打开背包使用道具                                     ║" << std::endl;
        std::cout << "║    • 按 5 进入 NPC 商店购买道具                                 ║" << std::endl;
        std::cout << "║    • 按 6 分配升级获得的属性点                                  ║" << std::endl;
        std::cout << "║    • 按 S 保存游戏进度                                         ║" << std::endl;
    }

    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;

    WaitForKeyPress();

    // 进入主游戏循环
    Run();
}


// ================================================================
//  游戏主循环
//  不断：检查存活 → 清屏 → 显示面板 → 显示菜单 → 处理操作 → 循环
// ================================================================
void GameManager::Run()
{
    while (true)
    {
        // -------- 死亡结算 --------
        // 当前角色死亡时，尝试切换到其他存活的角色
        if (currentCharacter && !currentCharacter->IsAlive())
        {
            std::cout << "\n[系统] " << currentCharacter->GetName() << " 已阵亡！" << std::endl;

            // 寻找第一个还活着的角色
            bool foundAlive = false;
            for (auto& ch : characters)
            {
                if (ch->IsAlive())
                {
                    currentCharacter = ch;
                    foundAlive = true;
                    std::cout << "[系统] 自动切换到 " << ch->GetName() << std::endl;
                    break;
                }
            }

            if (!foundAlive)
            {
                // 所有角色都死了 → 游戏结束
                ClearScreen();
                std::cout << "\n============================================================" << std::endl;
                std::cout << "                    全 员 阵 亡！" << std::endl;
                std::cout << "                    游 戏 结 束" << std::endl;
                std::cout << "============================================================" << std::endl;
                std::cout << "\n按 Enter 退出...";
                std::cin.get();
                return;
            }

            WaitForKeyPress();
        }

        UpdateDashboard();      // 刷新主控面板
        DisplayMainMenu();      // 显示菜单
        if (!HandleAction())    // 处理玩家选择，返回 false 则退出到标题画面
        {
            ClearScreen();
            // 清理当前游戏数据
            characters.clear();
            enemies.clear();
            currentCharacter = nullptr;
            playerNickname.clear();
            ShowTitleScreen();   // 返回标题画面
            return;              // 退出 Run() 循环
        }
    }
}

// ---------- 刷新面板 ----------
void GameManager::UpdateDashboard() const
{
    DisplayDashboard();
}


// ================================================================
//  伤害计算系统演示
//  展示三种伤害类型（物理/魔法/真实）的完整计算过程
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

    // 列出可选角色
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
    WaitForKeyPress();
}


// ================================================================
//  执行攻击演示
//  展示攻击者和防御者的属性，然后计算三种伤害
// ================================================================
void GameManager::PerformAttack(std::shared_ptr<Character> attacker, std::shared_ptr<Character> defender)
{
    DamageCalculator calculator;

    // 显示攻击者和防御者的信息
    std::cout << "攻击者信息:" << std::endl;
    attacker->DisplayInfo();
    std::cout << std::endl;

    std::cout << "防御者信息:" << std::endl;
    defender->DisplayInfo();
    std::cout << std::endl;

    // ===== 物理伤害计算 =====
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "【物理伤害计算】" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;

    DamageInfo physicalDamage = calculator.CalculatePhysicalDamage(
        attacker->GetAttackPower(),
        defender->GetDefend(),
        attacker->GetLevel(),
        defender->GetLevel(),
        1.5,                            // 技能倍率 1.5
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

    // ===== 魔法伤害计算 =====
    std::cout << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "【魔法伤害计算】" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;

    DamageInfo magicalDamage = calculator.CalculateMagicalDamage(
        50,                             // 假设法术攻击力 50
        defender->GetSpellDefense(),
        attacker->GetLevel(),
        defender->GetLevel(),
        1.2,                            // 技能倍率 1.2
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

    // ===== 真实伤害计算 =====
    std::cout << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "【真实伤害计算】" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;

    DamageInfo trueDamage = calculator.CalculateTrueDamage(
        20,                             // 固定伤害 20
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
//  史莱姆战斗测试
//  创建 3 个不同等级的史莱姆，测试攻击和分裂
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

    // 创建三个不同等级的史莱姆
    std::cout << "║ 生成史莱姆..." << std::endl;
    auto slime1 = std::make_shared<Slime>(1);   // 1 级史莱姆
    auto slime2 = std::make_shared<Slime>(3);   // 3 级史莱姆
    auto slime3 = std::make_shared<Slime>(5);   // 5 级史莱姆

    // 显示玩家和史莱姆信息
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

    // 测试 1：玩家攻击史莱姆
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ 测试1: 玩家普通攻击史莱姆1" << std::endl;
    Player* player = dynamic_cast<Player*>(currentCharacter.get());
    if (player)
    {
        player->NormalAttack(slime1.get());
    }

    // 测试 2：史莱姆攻击玩家
    std::cout << std::endl << "║ 测试2: 史莱姆1普通攻击玩家" << std::endl;
    slime1->PerformAction(currentCharacter.get());

    // 测试 3：史莱姆分裂（先把它打到半血以下）
    std::cout << std::endl << "║ 测试3: 史莱姆血量低时的分裂攻击" << std::endl;
    while (slime2->GetHp() > slime2->GetMaxHp() / 2)
    {
        slime2->TakeDamage(10);     // 每次打 10 血，直到低于半血
    }
    slime2->PerformAction(currentCharacter.get());  // 触发分裂！

    std::cout << std::endl << "║ 测试结束！" << std::endl;

    // 战斗胜利奖励
    if (player)
    {
        int expReward = 50;
        std::cout << "║ 战斗胜利！获得 " << expReward << " 经验值！" << std::endl;
        player->GainExperience(expReward);
    }

    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
}

// ================================================================
//  DrawSeparator - 画分隔线
// ================================================================
void GameManager::DrawSeparator(const std::string& title)
{
    std::cout << "\n============================================================\n";
    if (!title.empty())
        std::cout << "  " << title << "\n";
    std::cout << "============================================================\n";
}

// ================================================================
//  ATB 测试辅助 - 结构体和常量
// ================================================================

namespace ATBTest {
    const int TICK_MS = 50;
    const int MAX_PREDICT = 3;

    // ---------- 行动预测条目 ----------
    struct PredictEntry {
        std::string name;
        int speed;
        int framesToReady;
    };

    // ---------- 行动条目 ----------
    struct ActionEntry {
        bool isPlayer;
        int speed;
        size_t enemyIndex;
    };

    // ---------- 计算到满条所需帧数 ----------
    int FramesToReady(const Character* c) {
        int remaining = MAX_GAUGE - c->GetCurrentGauge();
        if (remaining <= 0) return 0;
        int perFrame = (c->GetSpeed() * TICK_MS) / 100;
        if (perFrame <= 0) return 99999;
        return (remaining + perFrame - 1) / perFrame;
    }

    // ---------- 预测接下来 N 个行动者 ----------
    std::vector<PredictEntry> PredictNextActors(
        const std::vector<Character*>& units, int count = MAX_PREDICT) {
        std::vector<PredictEntry> result;
        for (auto* u : units) {
            if (!u->IsAlive()) continue;
            result.push_back({ u->GetName(), u->GetSpeed(), FramesToReady(u) });
        }
        std::sort(result.begin(), result.end(),
            [](const PredictEntry& a, const PredictEntry& b) {
                if (a.framesToReady != b.framesToReady)
                    return a.framesToReady < b.framesToReady;
                return a.speed > b.speed;
            });
        if ((int)result.size() > count) result.resize(count);
        return result;
    }

    // ---------- 显示行动顺序 ----------
    void ShowActionOrder(const std::vector<PredictEntry>& predictions) {
        std::cout << "[行动顺序] ";
        for (size_t i = 0; i < predictions.size(); ++i) {
            if (i > 0) std::cout << " -> ";
            std::cout << predictions[i].name;
            if (predictions[i].framesToReady > 0)
                std::cout << "(" << predictions[i].framesToReady << "帧)";
            else
                std::cout << "(就绪)";
        }
        std::cout << "\n";
    }

    // ---------- 快进到下一个行动者满条 ----------
    int FastForwardToNextAction(
        Player* player,
        std::vector<std::shared_ptr<Enemy>>& enemies) {
        int framesPassed = 0;
        while (true) {
            if (player->IsAlive() && player->IsReadyToAct()) return framesPassed;
            for (auto& e : enemies)
                if (e->IsAlive() && e->IsReadyToAct()) return framesPassed;
            framesPassed++;
            if (player->IsAlive()) player->UpdateGauge(TICK_MS);
            for (auto& e : enemies)
                if (e->IsAlive()) e->UpdateGauge(TICK_MS);
        }
    }

    // ---------- 收集所有满条单位 ----------
    std::vector<ActionEntry> CollectReadyActors(
        Player* player,
        const std::vector<std::shared_ptr<Enemy>>& enemies) {
        std::vector<ActionEntry> queue;
        if (player->IsAlive() && player->IsReadyToAct())
            queue.push_back({ true, player->GetSpeed(), 0 });
        for (size_t i = 0; i < enemies.size(); ++i)
            if (enemies[i]->IsAlive() && enemies[i]->IsReadyToAct())
                queue.push_back({ false, enemies[i]->GetSpeed(), i });
        std::sort(queue.begin(), queue.end(),
            [](const ActionEntry& a, const ActionEntry& b) {
                return a.speed > b.speed;
            });
        return queue;
    }

    // ---------- 构建预测用单位列表 ----------
    std::vector<Character*> BuildUnitList(
        Player* player,
        const std::vector<std::shared_ptr<Enemy>>& enemies) {
        std::vector<Character*> units;
        if (player->IsAlive()) units.push_back(player);
        for (auto& e : enemies)
            if (e->IsAlive()) units.push_back(e.get());
        return units;
    }
}

// ================================================================
//  TestATB_1v1 - ATB 1v1 测试
// ================================================================
void GameManager::TestATB_1v1()
{
    ClearScreen();
    DrawSeparator("测试1：ATB战斗 - 1v1（无跑条动画，仅结算+行动顺序）");

    auto player = std::make_shared<Braver>("勇者·剑心");
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
    {
        std::cout << "\n【胜利！】\n";
        // 战斗奖励经验值
        int expReward = slime->GetExperienceReward();
        int goldReward = slime->GetGoldReward();
        std::cout << "获得经验: " << expReward << "  获得金币: " << goldReward << "\n";
        player->GainExperience(expReward);
        player->gold += goldReward;
    }
    else if (!player->IsAlive())
        std::cout << "\n【失败！】\n";

    std::cout << "\n按 Enter 继续...";
    std::cin.get();
}

// ================================================================
//  TestSlimeSplit - 史莱姆分裂验证
// ================================================================
void GameManager::TestSlimeSplit()
{
    ClearScreen();
    DrawSeparator("测试2：史莱姆分裂验证");

    auto player = std::make_shared<Braver>("测试勇者");
    player->SetAttackPower(30);

    auto slime = std::make_shared<Slime>(3);
    std::vector<std::shared_ptr<Enemy>> enemies;
    enemies.push_back(slime);

    std::cout << "玩家 SPD=" << player->GetSpeed() << " | 史莱姆 SPD=" << slime->GetSpeed() << "\n";
    std::cout << "分裂阈值: HP < " << slime->GetMaxHp() / 2 << "（低于这个血量就会分裂）\n\n";
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
//  TestElementSystemQuick - 元素克制全覆盖测试（自动 PASS/FAIL）
//
//  测试所有四条克制关系，每条正向/反向各一次 + 中立无克制
//  共 9 个子测试
// ================================================================
void GameManager::TestElementSystemQuick()
{
    ClearScreen();
    DrawSeparator("T1：属性克制全覆盖测试（9 项子测试）");

    // 固定属性的攻击者
    auto attacker = std::make_shared<Braver>("测试者");
    attacker->SetAttackPower(30);
    attacker->SetLevel(1);

    int totalTests = 0;
    int passCount = 0;

    // 辅助 Lambda：重置怪物血量到 200，避免死亡影响后续测试
    auto ResetMonster = [](std::shared_ptr<Enemy>& m) {
        m->SetMaxHp(200);
        m->SetHp(200);
    };

    // 辅助 Lambda：执行单次克制测试
    auto RunElementTest = [&](const std::string& testName,
                              ElementType atkElem, ElementType defElem,
                              double expectedBonus, const std::string& relation) {
        totalTests++;
        attacker->SetElementType(atkElem);

        auto monster = std::make_shared<Enemy>("测试怪物", 200, 8, 10, 1);
        monster->SetElementType(defElem);

        std::cout << "\n--- 子测试 " << totalTests << "：" << testName << " (" << relation << ") ---" << std::endl;
        std::cout << "  攻击者属性: " << (atkElem == ElementType::Fire ? "火" :
                                       atkElem == ElementType::Wind ? "风" :
                                       atkElem == ElementType::Earth ? "地" :
                                       atkElem == ElementType::Water ? "水" : "中立")
                  << "  防御者属性: " << (defElem == ElementType::Fire ? "火" :
                                       defElem == ElementType::Wind ? "风" :
                                       defElem == ElementType::Earth ? "地" :
                                       defElem == ElementType::Water ? "水" : "中立")
                  << "  期望 elementBonus = " << expectedBonus << "x" << std::endl;

        int hpBefore = monster->GetHp();
        auto d = attacker->NormalAttack(monster.get());
        int actualDmg = hpBefore - monster->GetHp();

        std::cout << "  elementBonus = " << d.elementBonus << "x"
                  << "  finalDamage = " << static_cast<int>(d.finalDamage)
                  << "  实际扣血 = " << actualDmg << std::endl;

        bool pass = false;
        if (expectedBonus >= 1.4) // 克制 1.5x
            pass = (d.elementBonus >= 1.4 && d.elementBonus <= 1.6);
        else if (expectedBonus <= 0.8) // 被克 0.75x
            pass = (d.elementBonus >= 0.7 && d.elementBonus <= 0.8);
        else // 中立 1.0x
            pass = (d.elementBonus >= 0.95 && d.elementBonus <= 1.05);

        if (pass) passCount++;
        std::cout << "  >> " << (pass ? "PASS" : "FAIL") << " <<" << std::endl;

        // 重置怪物血量
        ResetMonster(monster);
    };

    // === 9 个子测试 ===
    RunElementTest("火 → 风", ElementType::Fire, ElementType::Wind, 1.5, "火克风");
    RunElementTest("风 → 火", ElementType::Wind, ElementType::Fire, 0.75, "风被火克");
    RunElementTest("风 → 地", ElementType::Wind, ElementType::Earth, 1.5, "风克地");
    RunElementTest("地 → 风", ElementType::Earth, ElementType::Wind, 0.75, "地被风克");
    RunElementTest("地 → 水", ElementType::Earth, ElementType::Water, 1.5, "地克水");
    RunElementTest("水 → 地", ElementType::Water, ElementType::Earth, 0.75, "水被地克");
    RunElementTest("水 → 火", ElementType::Water, ElementType::Fire, 1.5, "水克火");
    RunElementTest("火 → 水", ElementType::Fire, ElementType::Water, 0.75, "火被水克");
    RunElementTest("中立 → 中立", ElementType::Neutral, ElementType::Neutral, 1.0, "无克制");

    // === 总结 ===
    std::cout << "\n============================================================" << std::endl;
    std::cout << "  汇总: " << passCount << "/" << totalTests << " 通过" << std::endl;
    std::cout << "  最终结论: " << (passCount == totalTests ? ">>> ALL PASS <<<" : ">>> SOME FAIL <<<") << std::endl;
    std::cout << "============================================================" << std::endl;

    std::cout << "\n按 Enter 继续...";
    std::cin.get();
}

// ================================================================
//  TestInventoryQuick - 背包与道具快速验证（自动 PASS/FAIL）
//
//  4 个步骤：
//    1. 验证背包道具数量
//    2. 使用小型治疗药水
//    3. 使用中型治疗药水
//    4. 装备匕首
// ================================================================
void GameManager::TestInventoryQuick()
{
    ClearScreen();
    DrawSeparator("T2：背包与道具验证（4 步骤）");

    auto player = std::make_shared<Braver>("测试玩家");
    int totalTests = 0;
    int passCount = 0;

    // === 步骤1：验证背包道具数量 ===
    std::cout << "\n--- 步骤1：初始化背包并验证道具数量 ---" << std::endl;
    InitPlayerInventory(player.get());
    int itemCount = player->GetInventory().GetItemCount();
    std::cout << "  背包道具数: " << itemCount << std::endl;
    player->GetInventory().ShowInventory();

    totalTests++;
    bool pass1 = (itemCount == 4);
    std::cout << "  >> " << (pass1 ? "PASS" : "FAIL") << " (期望 4 个道具) <<" << std::endl;
    if (pass1) passCount++;

    // === 步骤2：使用小型治疗药水 ===
    std::cout << "\n--- 步骤2：使用小型治疗药水（恢复20HP）---" << std::endl;
    // 先把玩家打到半血
    int halfHp = player->GetMaxHp() / 2;
    int damageToDeal = player->GetHp() - halfHp;
    player->TakeDamage(damageToDeal);
    std::cout << "  先造成 " << damageToDeal << " 点伤害" << std::endl;
    std::cout << "  使用前 HP: " << player->GetHp() << "/" << player->GetMaxHp() << std::endl;

    int hpBefore = player->GetHp();
    player->GetInventory().UseItem(0, player.get());   // 索引0 = 小型药水
    int hpAfter = player->GetHp();
    int healAmount = hpAfter - hpBefore;
    std::cout << "  使用后 HP: " << hpAfter << "/" << player->GetMaxHp() << " (+" << healAmount << ")" << std::endl;

    totalTests++;
    bool pass2 = (healAmount == 20);
    std::cout << "  >> " << (pass2 ? "PASS" : "FAIL") << " (期望恢复 20HP) <<" << std::endl;
    if (pass2) passCount++;

    // === 步骤3：使用中型治疗药水 ===
    std::cout << "\n--- 步骤3：使用中型治疗药水（恢复50HP）---" << std::endl;
    // 再次把玩家打到半血
    damageToDeal = player->GetHp() - halfHp;
    if (damageToDeal > 0)
    {
        player->TakeDamage(damageToDeal);
        std::cout << "  先造成 " << damageToDeal << " 点伤害" << std::endl;
    }
    std::cout << "  使用前 HP: " << player->GetHp() << "/" << player->GetMaxHp() << std::endl;

    hpBefore = player->GetHp();
    player->GetInventory().UseItem(0, player.get());   // 索引0 = 中型药水（小型已用掉）
    hpAfter = player->GetHp();
    healAmount = hpAfter - hpBefore;
    std::cout << "  使用后 HP: " << hpAfter << "/" << player->GetMaxHp() << " (+" << healAmount << ")" << std::endl;

    totalTests++;
    bool pass3 = (healAmount == 50);
    std::cout << "  >> " << (pass3 ? "PASS" : "FAIL") << " (期望恢复 50HP) <<" << std::endl;
    if (pass3) passCount++;

    // === 步骤4：装备匕首 ===
    std::cout << "\n--- 步骤4：装备匕首（ATK+3, AGI+8, SPD+3）---" << std::endl;
    int atkBefore = player->GetAttackPower();
    int agiBefore = player->GetAgility();
    int spdBefore = player->GetSpeed();
    std::cout << "  装备前: ATK=" << atkBefore << "  AGI=" << agiBefore << "  SPD=" << spdBefore << std::endl;

    // 添加匕首并使用（此时背包还剩小型药水1+匕首，匕首在索引1）
    player->GetInventory().AddItem(Weapon::CreateDagger());
    player->GetInventory().UseItem(0, player.get());   // 使用匕首（此时匕首在索引0，因为前面已经用掉两个药水了）

    int atkAfter = player->GetAttackPower();
    int agiAfter = player->GetAgility();
    int spdAfter = player->GetSpeed();
    std::cout << "  装备后: ATK=" << atkAfter << "  AGI=" << agiAfter << "  SPD=" << spdAfter << std::endl;
    std::cout << "  变化:   ATK+" << (atkAfter - atkBefore)
              << "  AGI+" << (agiAfter - agiBefore)
              << "  SPD+" << (spdAfter - spdBefore) << std::endl;

    bool passAtk = (atkAfter - atkBefore == 3);
    bool passAgi = (agiAfter - agiBefore == 8);
    bool passSpd = (spdAfter - spdBefore == 3);

    totalTests++;
    bool pass4 = (passAtk && passAgi && passSpd);
    std::cout << "  ATK >> " << (passAtk ? "PASS" : "FAIL") << " (期望 +3) <<" << std::endl;
    std::cout << "  AGI >> " << (passAgi ? "PASS" : "FAIL") << " (期望 +8) <<" << std::endl;
    std::cout << "  SPD >> " << (passSpd ? "PASS" : "FAIL") << " (期望 +3) <<" << std::endl;
    std::cout << "  装备测试 >> " << (pass4 ? "PASS" : "FAIL") << " <<" << std::endl;
    if (pass4) passCount++;

    // === 最终背包状态 ===
    std::cout << "\n--- 最终背包状态 ---" << std::endl;
    std::cout << "剩余道具数: " << player->GetInventory().GetItemCount() << std::endl;
    player->GetInventory().ShowInventory();

    // === 总结 ===
    std::cout << "\n============================================================" << std::endl;
    std::cout << "  汇总: " << passCount << "/" << totalTests << " 通过" << std::endl;
    std::cout << "  最终结论: " << (passCount == totalTests ? ">>> ALL PASS <<<" : ">>> SOME FAIL <<<") << std::endl;
    std::cout << "============================================================" << std::endl;

    std::cout << "\n按 Enter 继续...";
    std::cin.get();
}

// ================================================================
//  TestFullBattleQuick - 三场自动战斗测试
//
//  场景1：Braver vs FireLizard（火蜥蜴）
//  场景2：Guardian vs Slime（验证护盾机制）
//  场景3：Mage vs WindSprite（风精灵）
//
//  全部自动战斗（无需玩家操作），最多 15 回合
// ================================================================
void GameManager::TestFullBattleQuick()
{
    ClearScreen();
    DrawSeparator("T3：自动战斗测试（3 个场景）");

    // ================================================================
    //  场景1：Braver vs FireLizard
    // ================================================================
    {
        std::cout << "\n========================================" << std::endl;
        std::cout << "  场景1：勇者 vs 火蜥蜴 (Braver vs FireLizard)" << std::endl;
        std::cout << "========================================" << std::endl;

        auto braver = std::make_shared<Braver>("勇者·测试");
        braver->SetPhysicalResistance(0.0);
        braver->SetMagicalResistance(0.05);
        braver->AddSkill(std::make_shared<TrueSkill>("刺客之刃", 30, 20));

        auto fireLizard = std::make_shared<FireLizard>(3);
        std::vector<std::shared_ptr<Enemy>> enemies;
        enemies.push_back(fireLizard);

        std::cout << "勇者 SPD=" << braver->GetSpeed()
                  << " | 火蜥蜴 SPD=" << fireLizard->GetSpeed() << std::endl;
        braver->DisplayInfo();
        fireLizard->DisplayInfo();

        int round = 0;
        const int MAX_ROUND = 15;
        while (braver->IsAlive() && fireLizard->IsAlive() && round < MAX_ROUND)
        {
            ATBTest::FastForwardToNextAction(braver.get(), enemies);
            auto queue = ATBTest::CollectReadyActors(braver.get(), enemies);
            round++;

            for (auto& entry : queue)
            {
                if (entry.isPlayer)
                {
                    braver->ResetGauge();
                    std::cout << "\n[回合" << round << "] 勇者行动" << std::endl;
                    // 自动策略：有蓝就用技能，没蓝就平A
                    if (braver->HasEnoughMp(20))
                        braver->UseSkill(0, fireLizard.get());
                    else
                        braver->NormalAttack(fireLizard.get());
                    std::cout << "  勇者 HP:" << braver->GetHp() << "/" << braver->GetMaxHp()
                              << " MP:" << braver->GetMp() << "/" << braver->GetMaxMp()
                              << "  火蜥蜴 HP:" << fireLizard->GetHp() << "/" << fireLizard->GetMaxHp() << std::endl;
                }
                else
                {
                    auto& enemy = enemies[entry.enemyIndex];
                    if (!enemy->IsAlive()) continue;
                    enemy->ResetGauge();
                    std::cout << "\n[回合" << round << "] 火蜥蜴行动" << std::endl;
                    enemy->PerformAction(braver.get());
                    std::cout << "  勇者 HP:" << braver->GetHp() << "/" << braver->GetMaxHp()
                              << "  火蜥蜴 HP:" << fireLizard->GetHp() << "/" << fireLizard->GetMaxHp() << std::endl;
                }
                if (!braver->IsAlive() || !fireLizard->IsAlive()) break;
            }
        }

        if (!fireLizard->IsAlive())
            std::cout << "\n>>> 场景1：勇者 胜利！火蜥蜴被击败！" << std::endl;
        else if (!braver->IsAlive())
            std::cout << "\n>>> 场景1：勇者 失败！" << std::endl;
        else
            std::cout << "\n>>> 场景1：达到最大回合数，战斗中断。" << std::endl;
    }

    std::cout << "\n按 Enter 进入场景2...";
    std::cin.get();

    // ================================================================
    //  场景2：Guardian vs Slime（验证护盾机制）
    // ================================================================
    {
        ClearScreen();
        std::cout << "========================================" << std::endl;
        std::cout << "  场景2：守护骑士 vs 史莱姆 (Guardian vs Slime)" << std::endl;
        std::cout << "  验证护盾机制：GuardStance 获得护盾 → 护盾先吸收伤害" << std::endl;
        std::cout << "========================================" << std::endl;

        auto guardian = std::make_shared<Guardian>("骑士·守护");
        guardian->SetPhysicalResistance(0.1);
        guardian->SetMagicalResistance(0.1);

        auto slime = std::make_shared<Slime>(3);
        std::vector<std::shared_ptr<Enemy>> enemies;
        enemies.push_back(slime);

        std::cout << "守护骑士 SPD=" << guardian->GetSpeed()
                  << " | 史莱姆 SPD=" << slime->GetSpeed() << std::endl;
        guardian->DisplayInfo();
        slime->DisplayInfo();

        std::cout << "\n>>> 第1回合：守护骑士使用【守护姿态】获得护盾 <<<" << std::endl;
        guardian->GuardStance();
        std::cout << "  护盾值: " << guardian->GetShield() << "  当前HP: "
                  << guardian->GetHp() << "/" << guardian->GetMaxHp() << std::endl;

        int round = 0;
        const int MAX_ROUND = 15;
        bool shieldVerified = false;
        while (guardian->IsAlive() && slime->IsAlive() && round < MAX_ROUND)
        {
            ATBTest::FastForwardToNextAction(guardian.get(), enemies);
            auto queue = ATBTest::CollectReadyActors(guardian.get(), enemies);
            round++;

            for (auto& entry : queue)
            {
                if (entry.isPlayer)
                {
                    guardian->ResetGauge();
                    std::cout << "\n[回合" << round << "] 守护骑士行动" << std::endl;
                    // 自动策略：优先保持护盾，然后输出
                    if (guardian->GetShield() < 10 && guardian->HasEnoughMp(15))
                    {
                        std::cout << "  → 使用【守护姿态】补充护盾" << std::endl;
                        guardian->GuardStance();
                    }
                    else if (guardian->HasEnoughMp(20))
                    {
                        std::cout << "  → 使用【圣光斩】" << std::endl;
                        guardian->HolySlash(slime.get());
                    }
                    else
                    {
                        std::cout << "  → 普通攻击" << std::endl;
                        guardian->NormalAttack(slime.get());
                    }
                    std::cout << "  护盾: " << guardian->GetShield()
                              << "  HP:" << guardian->GetHp() << "/" << guardian->GetMaxHp()
                              << "  MP:" << guardian->GetMp() << "/" << guardian->GetMaxMp()
                              << "  史莱姆 HP:" << slime->GetHp() << "/" << slime->GetMaxHp() << std::endl;
                }
                else
                {
                    auto& enemy = enemies[entry.enemyIndex];
                    if (!enemy->IsAlive()) continue;
                    enemy->ResetGauge();

                    int shieldBefore = guardian->GetShield();
                    int hpBefore = guardian->GetHp();
                    std::cout << "\n[回合" << round << "] 史莱姆攻击！（护盾前: " << shieldBefore << "）" << std::endl;
                    enemy->PerformAction(guardian.get());
                    int shieldAfter = guardian->GetShield();
                    int hpAfter = guardian->GetHp();

                    if (!shieldVerified && shieldBefore > 0 && shieldAfter < shieldBefore)
                    {
                        shieldVerified = true;
                        std::cout << "  ✓ 护盾机制验证通过！护盾 " << shieldBefore
                                  << " → " << shieldAfter << "（优先吸收伤害）" << std::endl;
                    }
                    std::cout << "  护盾: " << shieldAfter
                              << "  HP:" << hpAfter << "/" << guardian->GetMaxHp()
                              << "  史莱姆 HP:" << slime->GetHp() << "/" << slime->GetMaxHp() << std::endl;
                }
                if (!guardian->IsAlive() || !slime->IsAlive()) break;
            }
        }

        if (!slime->IsAlive())
            std::cout << "\n>>> 场景2：守护骑士 胜利！史莱姆被击败！" << std::endl;
        else if (!guardian->IsAlive())
            std::cout << "\n>>> 场景2：守护骑士 失败！" << std::endl;
        else
            std::cout << "\n>>> 场景2：达到最大回合数，战斗中断。" << std::endl;

        if (shieldVerified)
            std::cout << "  [护盾机制] PASS" << std::endl;
        else
            std::cout << "  [护盾机制] 未触发（可能未受到攻击或护盾未被消耗）" << std::endl;
    }

    std::cout << "\n按 Enter 进入场景3...";
    std::cin.get();

    // ================================================================
    //  场景3：Mage vs WindSprite
    // ================================================================
    {
        ClearScreen();
        std::cout << "========================================" << std::endl;
        std::cout << "  场景3：法师 vs 风精灵 (Mage vs WindSprite)" << std::endl;
        std::cout << "========================================" << std::endl;

        auto mage = std::make_shared<Mage>("法师·火焰", 35);
        mage->SetPhysicalResistance(0.0);
        mage->SetMagicalResistance(0.15);
        mage->AddSkill(std::make_shared<MagicalSkill>("火焰术", 1.8, 30, 35, ElementType::Fire));

        auto windSprite = std::make_shared<WindSprite>(3);
        std::vector<std::shared_ptr<Enemy>> enemies;
        enemies.push_back(windSprite);

        std::cout << "法师 SPD=" << mage->GetSpeed()
                  << " | 风精灵 SPD=" << windSprite->GetSpeed() << std::endl;
        std::cout << "法师 属性: 火  |  风精灵 属性: 风" << std::endl;
        std::cout << "注意：火克风！火焰术将造成 1.5x 伤害！" << std::endl;
        mage->DisplayInfo();
        windSprite->DisplayInfo();

        int round = 0;
        const int MAX_ROUND = 15;
        while (mage->IsAlive() && windSprite->IsAlive() && round < MAX_ROUND)
        {
            ATBTest::FastForwardToNextAction(mage.get(), enemies);
            auto queue = ATBTest::CollectReadyActors(mage.get(), enemies);
            round++;

            for (auto& entry : queue)
            {
                if (entry.isPlayer)
                {
                    mage->ResetGauge();
                    std::cout << "\n[回合" << round << "] 法师行动" << std::endl;
                    // 自动策略：优先使用火焰术（火克风，高效输出），没蓝就平A
                    if (mage->HasEnoughMp(30))
                    {
                        std::cout << "  → 使用【火焰术】（火克风，1.5x 伤害！）" << std::endl;
                        mage->UseSkill(0, windSprite.get());
                    }
                    else
                    {
                        std::cout << "  → 普通攻击" << std::endl;
                        mage->NormalAttack(windSprite.get());
                    }
                    std::cout << "  法师 HP:" << mage->GetHp() << "/" << mage->GetMaxHp()
                              << " MP:" << mage->GetMp() << "/" << mage->GetMaxMp()
                              << "  风精灵 HP:" << windSprite->GetHp() << "/" << windSprite->GetMaxHp() << std::endl;
                }
                else
                {
                    auto& enemy = enemies[entry.enemyIndex];
                    if (!enemy->IsAlive()) continue;
                    enemy->ResetGauge();
                    std::cout << "\n[回合" << round << "] 风精灵行动" << std::endl;
                    enemy->PerformAction(mage.get());
                    std::cout << "  法师 HP:" << mage->GetHp() << "/" << mage->GetMaxHp()
                              << "  风精灵 HP:" << windSprite->GetHp() << "/" << windSprite->GetMaxHp() << std::endl;
                }
                if (!mage->IsAlive() || !windSprite->IsAlive()) break;
            }
        }

        if (!windSprite->IsAlive())
            std::cout << "\n>>> 场景3：法师 胜利！风精灵被击败！" << std::endl;
        else if (!mage->IsAlive())
            std::cout << "\n>>> 场景3：法师 失败！" << std::endl;
        else
            std::cout << "\n>>> 场景3：达到最大回合数，战斗中断。" << std::endl;
    }

    std::cout << "\n============================================================" << std::endl;
    std::cout << "  三场自动战斗测试全部结束！" << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << "\n按 Enter 返回...";
    std::cin.get();
}

// ================================================================
//  BattleEncounter - 怪物遭遇战
//
//  根据玩家等级随机生成 1~3 只怪物，进入 ATB 战斗
//  胜利后获得经验和金币奖励
// ================================================================
void GameManager::BattleEncounter()
{
    Player* player = dynamic_cast<Player*>(currentCharacter.get());
    if (!player) return;

    ClearScreen();
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                      野外探索                                  ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║  你踏上了冒险的旅程...                                          ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;

    // 根据玩家等级决定怪物等级（±2 级浮动）
    int playerLevel = player->GetLevel();
    int monsterLevel = playerLevel + (rand() % 5 - 2);  // -2 ~ +2
    if (monsterLevel < 1) monsterLevel = 1;

    // 随机怪物数量 1~3
    int monsterCount = (rand() % 3) + 1;

    // 清空之前的怪物
    enemies.clear();

    // 随机生成怪物
    for (int i = 0; i < monsterCount; ++i)
    {
        int monsterType = rand() % 3;  // 0=史莱姆, 1=火焰蜥蜴, 2=风精灵
        int mLevel = monsterLevel + (rand() % 3 - 1);  // 等级微调
        if (mLevel < 1) mLevel = 1;

        std::shared_ptr<Enemy> enemy;
        switch (monsterType)
        {
        case 0:
            enemy = std::make_shared<Slime>(mLevel);
            break;
        case 1:
            enemy = std::make_shared<FireLizard>(mLevel);
            break;
        case 2:
            enemy = std::make_shared<WindSprite>(mLevel);
            break;
        }
        enemies.push_back(enemy);
    }

    // 显示遭遇信息
    std::cout << "║  突然，前方出现了 " << enemies.size() << " 只怪物！                               ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    for (size_t i = 0; i < enemies.size(); ++i)
    {
        std::cout << "║  [" << (i + 1) << "] ";
        enemies[i]->DisplayInfo();
        std::cout << "║" << std::endl;
    }

    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║  准备战斗！(按 Enter)                                          ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cin.ignore();
    std::cin.get();

    // 进入战斗
    bool victory = RunBattle(enemies);

    if (victory)
    {
        // 战斗胜利！发放奖励
        ClearScreen();
        std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                      ★ 战斗胜利！★                             ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;

        int totalExp = 0;
        int totalGold = 0;
        for (auto& enemy : enemies)
        {
            totalExp += enemy->GetExperienceReward();
            totalGold += enemy->GetGoldReward();
        }

        std::cout << "║  获得经验: " << std::setw(48) << std::left << totalExp << "║" << std::endl;
        std::cout << "║  获得金币: " << std::setw(48) << std::left << totalGold << "║" << std::endl;

        player->GainExperience(totalExp);
        player->gold += totalGold;

        // 检查升级
        if (player->GetAvailableAP() > 0)
        {
            std::cout << "║                                                               ║" << std::endl;
            std::cout << "║  升级了！获得 " << player->GetAvailableAP() << " 点属性点！                                ║" << std::endl;
        }

        std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
        WaitForKeyPress();
    }
    else
    {
        // 战斗失败
        ClearScreen();
        std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                      战斗失败...                               ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║  你被击败了，但顽强地活了下来。                                  ║" << std::endl;
        std::cout << "║  回到城镇休息后恢复了 50% 的生命值。                             ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;

        // 回复一半血量
        if (currentCharacter && currentCharacter->IsAlive())
        {
            int healAmount = currentCharacter->GetMaxHp() / 2;
            currentCharacter->Heal(healAmount);
        }
        WaitForKeyPress();
    }

    // 清理怪物
    enemies.clear();
}

// ================================================================
//  RunBattle - ATB 战斗主循环
//
//  玩家 vs 怪物列表，ATB 回合制战斗
//  返回 true = 胜利，false = 失败
// ================================================================
bool GameManager::RunBattle(std::vector<std::shared_ptr<Enemy>>& enemies)
{
    Player* player = dynamic_cast<Player*>(currentCharacter.get());
    if (!player) return false;

    // 重置所有人的行动条
    player->ResetGauge();
    for (auto& e : enemies)
    {
        e->ResetGauge();
        e->SetHp(e->GetMaxHp());  // 确保怪物满血
    }

    bool quit = false;
    int round = 0;

    while (!quit && player->IsAlive())
    {
        // 检查所有怪物是否死亡
        bool allEnemiesDead = true;
        for (auto& e : enemies)
        {
            if (e->IsAlive())
            {
                allEnemiesDead = false;
                break;
            }
        }
        if (allEnemiesDead)
        {
            return true;  // 胜利！
        }

        // 快进到下一个行动者
        int frames = ATBTest::FastForwardToNextAction(player, enemies);

        // 收集所有满条的单位
        auto queue = ATBTest::CollectReadyActors(player, enemies);

        for (auto& entry : queue)
        {
            if (entry.isPlayer)
            {
                player->ResetGauge();
                round++;

                // 清屏显示战斗状态
                ClearScreen();
                std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
                std::cout << "║                    ATB 战斗 - 第 " << std::setw(2) << std::left << round << " 回合                       ║" << std::endl;
                std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;

                // 显示玩家状态
                std::cout << "║  " << std::setw(10) << std::left << player->GetName()
                          << "  HP: " << std::setw(5) << std::left << (std::to_string(player->GetHp()) + "/" + std::to_string(player->GetMaxHp()))
                          << "  MP: " << std::setw(5) << std::left << (std::to_string(player->GetMp()) + "/" + std::to_string(player->GetMaxMp()))
                          << "                   ║" << std::endl;

                // 显示怪物状态
                std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
                for (size_t i = 0; i < enemies.size(); ++i)
                {
                    if (!enemies[i]->IsAlive())
                    {
                        std::cout << "║  [" << (i + 1) << "] " << std::setw(20) << std::left << enemies[i]->GetName()
                                  << " [已阵亡]                                              ║" << std::endl;
                    }
                    else
                    {
                        std::cout << "║  [" << (i + 1) << "] " << std::setw(20) << std::left << enemies[i]->GetName()
                                  << " HP: " << std::setw(5) << std::left << (std::to_string(enemies[i]->GetHp()) + "/" + std::to_string(enemies[i]->GetMaxHp()))
                                  << "                    ║" << std::endl;
                    }
                }

                std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
                std::cout << "║  >>> 你的回合！请选择行动：                                      ║" << std::endl;
                std::cout << "║    [1] 普通攻击    [2] 使用技能    [3] 使用道具    [Q] 逃跑      ║" << std::endl;
                std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
                std::cout << "请输入: ";

                char ch;
                std::cin >> ch;
                std::cin.ignore();

                if (ch == 'Q' || ch == 'q')
                {
                    quit = true;
                    break;
                }

                switch (ch)
                {
                case '1':
                {
                    // 普通攻击：选择目标
                    std::cout << "请选择攻击目标: ";
                    int targetIdx;
                    if (std::cin >> targetIdx)
                    {
                        targetIdx--;  // 转为 0-based
                        if (targetIdx >= 0 && targetIdx < static_cast<int>(enemies.size()) && enemies[targetIdx]->IsAlive())
                        {
                            player->NormalAttack(enemies[targetIdx].get());
                        }
                        else
                        {
                            std::cout << "无效目标！" << std::endl;
                        }
                    }
                }
                break;

                case '2':
                {
                    // 使用技能
                    const auto& skills = player->GetSkills();
                    if (skills.empty())
                    {
                        std::cout << "没有可用的技能！" << std::endl;
                    }
                    else
                    {
                        std::cout << "可用技能:" << std::endl;
                        for (size_t i = 0; i < skills.size(); ++i)
                        {
                            std::cout << "  [" << (i + 1) << "] " << skills[i]->GetName()
                                      << " (消耗 " << skills[i]->GetMpCost() << " MP)" << std::endl;
                        }
                        std::cout << "请选择技能 (0=取消): ";
                        int skillIdx;
                        if (std::cin >> skillIdx)
                        {
                            if (skillIdx > 0 && skillIdx <= static_cast<int>(skills.size()))
                            {
                                std::cout << "请选择攻击目标: ";
                                int targetIdx;
                                if (std::cin >> targetIdx)
                                {
                                    targetIdx--;
                                    if (targetIdx >= 0 && targetIdx < static_cast<int>(enemies.size()) && enemies[targetIdx]->IsAlive())
                                    {
                                        player->UseSkill(skillIdx - 1, enemies[targetIdx].get());
                                    }
                                    else
                                    {
                                        std::cout << "无效目标！" << std::endl;
                                    }
                                }
                            }
                        }
                    }
                }
                break;

                case '3':
                {
                    // 使用道具
                    player->GetInventory().ShowInventory();
                    std::cout << "请输入道具序号 (0=取消): ";
                    int itemIdx;
                    if (std::cin >> itemIdx && itemIdx > 0)
                    {
                        player->GetInventory().UseItem(itemIdx, player);
                    }
                }
                break;

                default:
                    std::cout << "无效选择！" << std::endl;
                    break;
                }

                std::cout << std::endl << "按 Enter 继续...";
                std::cin.get();
            }
            else
            {
                // 怪物行动
                auto& enemy = enemies[entry.enemyIndex];
                if (!enemy->IsAlive()) continue;

                enemy->ResetGauge();

                std::cout << std::endl << ">>> " << enemy->GetName() << " 开始行动！" << std::endl;
                auto spawned = enemy->PerformAction(player);

                // 处理分裂
                if (!spawned.empty())
                {
                    for (auto& newEnemy : spawned)
                    {
                        enemies.push_back(newEnemy);
                        std::cout << "  (新怪物加入了战场！)" << std::endl;
                    }
                }

                std::cout << "按 Enter 继续...";
                std::cin.get();
            }

            // 检查玩家是否死亡
            if (!player->IsAlive())
            {
                return false;  // 失败
            }
        }
    }

    return false;  // 逃跑也算失败
}

// ================================================================
//  UseSkillInteractively - 交互式使用技能
//
//  在非战斗状态下使用技能（用于菜单中的"使用技能"选项）
// ================================================================
void GameManager::UseSkillInteractively(Player* player)
{
    if (!player) return;

    const auto& skills = player->GetSkills();
    if (skills.empty())
    {
        std::cout << "当前角色没有可用的技能！" << std::endl;
        WaitForKeyPress();
        return;
    }

    ClearScreen();
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                      技能列表                                  ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║  MP: " << std::setw(5) << std::left << (std::to_string(player->GetMp()) + "/" + std::to_string(player->GetMaxMp()))
              << "                                                     ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;

    for (size_t i = 0; i < skills.size(); ++i)
    {
        std::cout << "║  [" << (i + 1) << "] " << std::setw(30) << std::left << skills[i]->GetName()
                  << "  消耗: " << std::setw(3) << std::left << skills[i]->GetMpCost() << " MP              ║" << std::endl;
    }

    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;

    // 显示可攻击目标
    std::cout << "║  可攻击目标:                                                    ║" << std::endl;
    if (enemies.empty())
    {
        std::cout << "║    (当前没有怪物，技能只能对自己释放)                              ║" << std::endl;
    }
    else
    {
        for (size_t i = 0; i < enemies.size(); ++i)
        {
            if (enemies[i]->IsAlive())
            {
                std::cout << "║    [" << (i + 1) << "] " << std::setw(20) << std::left << enemies[i]->GetName()
                          << " HP: " << enemies[i]->GetHp() << "/" << enemies[i]->GetMaxHp() << "               ║" << std::endl;
            }
        }
    }

    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║  输入技能序号和目标编号 (例如: 1 1)，或输入 0 取消:               ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "请输入: ";

    int skillIdx;
    std::cin >> skillIdx;

    if (skillIdx <= 0 || skillIdx > static_cast<int>(skills.size()))
    {
        return;
    }

    int targetIdx;
    std::cin >> targetIdx;

    Character* target = nullptr;
    if (enemies.empty() || targetIdx <= 0 || targetIdx > static_cast<int>(enemies.size()) || !enemies[targetIdx - 1]->IsAlive())
    {
        std::cout << "没有有效目标！技能未释放。" << std::endl;
        WaitForKeyPress();
        return;
    }

    target = enemies[targetIdx - 1].get();
    player->UseSkill(skillIdx - 1, target);
    WaitForKeyPress();
}

// ================================================================
//  InitPlayerInventory - 初始化玩家背包
//
//  给玩家加上初始道具：
//    - 2 个小型治疗药水
//    - 1 个中型治疗药水
//    - 1 个匕首
// ================================================================
void GameManager::InitPlayerInventory(Player* player)
{
    if (!player) return;

    player->GetInventory().AddItem(Potion::CreateSmallHealthPotion());
    player->GetInventory().AddItem(Potion::CreateSmallHealthPotion());
    player->GetInventory().AddItem(Potion::CreateMediumHealthPotion());
    player->GetInventory().AddItem(Weapon::CreateDagger());

    std::cout << "[系统] 已为 " << player->GetName() << " 添加初始道具！" << std::endl;
}

// ================================================================
//  TestFullBattle - 完整 ATB 多对多战斗
// ================================================================
void GameManager::TestFullBattle()
{
    ClearScreen();
    DrawSeparator("测试3：完整ATB战斗 - 多敌人+技能+分裂");

    auto player = std::make_shared<Braver>("勇者·剑心");
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
            // 计算总经验奖励（所有被杀怪物的经验之和）
            int totalExp = 0, totalGold = 0;
            // 从原始敌人列表获取（在清除前）
            totalExp = 20 + 1 * 5 + 20 + 2 * 5 + 20 + 4 * 5;  // 3个史莱姆的经验
            totalGold = 10 + 1 * 3 + 10 + 2 * 3 + 10 + 4 * 3;  // 3个史莱姆的金币
            std::cout << "获得总经验: " << totalExp << "  获得总金币: " << totalGold << "\n";
            player->GainExperience(totalExp);
            player->gold += totalGold;
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
//  TestAllClasses - 三种职业对比
// ================================================================
void GameManager::TestAllClasses()
{
    ClearScreen();
    DrawSeparator("测试4：三种职业 vs 史莱姆（ATB速度对比）");

    auto braver = std::make_shared<Braver>("勇者·破晓");
    braver->SetPhysicalResistance(0.0);
    braver->SetMagicalResistance(0.05);
    braver->AddSkill(std::make_shared<TrueSkill>("刺客之刃", 30, 20));

    auto mage = std::make_shared<Mage>("法师·火焰", 35);
    mage->SetPhysicalResistance(0.0);
    mage->SetMagicalResistance(0.15);
    mage->AddSkill(std::make_shared<MagicalSkill>("火焰术", 1.8, 30, 35, ElementType::Fire));

    auto guardian = std::make_shared<Guardian>("骑士·守护");
    guardian->SetPhysicalResistance(0.1);
    guardian->SetMagicalResistance(0.1);

    struct TestCase { std::shared_ptr<Player> player; std::string className; };
    std::vector<TestCase> cases = {
        { braver,   "勇者" },
        { mage,     "法师" },
        { guardian, "守护骑士" }
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

        // 如果击败了史莱姆，获得经验奖励
        if (tc.player->IsAlive() && !slime->IsAlive())
        {
            int expReward = slime->GetExperienceReward();
            int goldReward = slime->GetGoldReward();
            std::cout << tc.className << " 获得经验: " << expReward << "  获得金币: " << goldReward << "\n";
            tc.player->GainExperience(expReward);
            tc.player->gold += goldReward;
        }
    }

    std::cout << "\n按 Enter 继续...";
    std::cin.get();
}
