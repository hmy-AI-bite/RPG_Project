#include "GameManager.h"
#include "UIManager.h"
#include "BattleSystem.h"
#include "TestManager.h"
#include "SaveLoadManager.h"
#include "Shop.h"
#include "Item.h"

#include <iostream>
#include <iomanip>

// ================================================================
//  工具方法
// ================================================================

void GameManager::WaitForKeyPress()
{
    UIManager::WaitForKeyPress();
}

// ================================================================
//  构造函数 / 析构函数
// ================================================================

GameManager::GameManager() : currentCharacter(nullptr)
{
}

GameManager::~GameManager()
{
}

// ================================================================
//  角色管理
// ================================================================

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

// ================================================================
//  怪物管理
// ================================================================

void GameManager::AddEnemy(std::shared_ptr<Enemy> enemy)
{
    if (enemy)
    {
        enemies.push_back(enemy);
    }
}

void GameManager::DisplayEnemies() const
{
    UIManager::DisplayEnemies(enemies);
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

            // 技能学习
            UIManager::SkillLearningFlow(newPlayer.get());

            // 加入队伍并进入游戏
            AddCharacter(newPlayer);
            currentCharacter = newPlayer;
            playerNickname = newPlayer->GetName();

            EnterGame();
            return;
        }

        case '2':
        {
            // 加载存档
            UIManager::ClearScreen();
            std::cout << std::endl;
            std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║                      加载存档                                  ║" << std::endl;
            std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
            std::cout << "║ 请输入存档文件名（默认 save.dat）: " << std::endl;
            std::cout << "║ ";

            std::string filename;
            std::getline(std::cin, filename);
            if (filename.empty()) filename = "save.dat";

            auto tempPlayer = std::make_shared<Braver>("临时角色");
            if (SaveLoadManager::LoadGame(*tempPlayer, filename))
            {
                std::cout << "║" << std::endl;
                std::cout << "║ 存档加载成功！欢迎回来，" << tempPlayer->GetName() << "！" << std::endl;
                std::cout << "║" << std::endl;
                AddCharacter(tempPlayer);
                currentCharacter = tempPlayer;
                playerNickname = tempPlayer->GetName();

                if (tempPlayer->GetSkills().empty())
                {
                    UIManager::SkillLearningFlow(tempPlayer.get());
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
            }
            break;
        }

        case '0':
            std::cout << std::endl << "感谢游玩！再见！" << std::endl;
            exit(0);
            break;

        default:
            break;
        }
    }
}

// ================================================================
//  进入游戏
// ================================================================

void GameManager::EnterGame()
{
    UIManager::EnterGameScreen(currentCharacter);
    Run();
}

// ================================================================
//  游戏主循环
// ================================================================

void GameManager::Run()
{
    while (true)
    {
        // 死亡结算
        if (currentCharacter && !currentCharacter->IsAlive())
        {
            std::cout << "\n[系统] " << currentCharacter->GetName() << " 已阵亡！" << std::endl;

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
                UIManager::ClearScreen();
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

        // 刷新面板 + 显示菜单
        UIManager::DisplayDashboard(currentCharacter);
        UIManager::DisplayMainMenu();

        if (!HandleAction())
        {
            // 返回标题画面
            UIManager::ClearScreen();
            characters.clear();
            enemies.clear();
            currentCharacter = nullptr;
            playerNickname.clear();
            ShowTitleScreen();
            return;
        }
    }
}

// ================================================================
//  处理操作（菜单路由分发）
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
            BattleSystem::BattleEncounter(player, enemies);
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
        UIManager::DisplayCharacterList(characters, currentCharacter);
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
            BattleSystem::UseSkillInteractively(player, enemies);
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
            OpenInventory(player);
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
            OpenShop(player);
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
        AllocateAttributes(player);
    }
    break;

    // ===== 7. 查看详细信息 =====
    case '7':
        ShowCharacterDetail();
        break;

    // ===== S. 保存游戏 =====
    case 'S':
    case 's':
    {
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

    default:
        std::cout << "无效选择！请重新输入。" << std::endl;
        WaitForKeyPress();
    }
    return true;
}

// ================================================================
//  背包界面
// ================================================================

void GameManager::OpenInventory(Player* player)
{
    UIManager::ClearScreen();
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

// ================================================================
//  商店界面
// ================================================================

void GameManager::OpenShop(Player* player)
{
    Shop shop;
    bool inShop = true;

    while (inShop)
    {
        UIManager::ClearScreen();
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

// ================================================================
//  属性分配界面
// ================================================================

void GameManager::AllocateAttributes(Player* player)
{
    bool allocating = true;
    while (allocating && player->GetAvailableAP() > 0)
    {
        UIManager::ClearScreen();
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

        // 职业特有信息（多态调用）
        currentCharacter->DisplayClassSpecificInfo();

        Mage* mage = dynamic_cast<Mage*>(player);
        Braver* braver = dynamic_cast<Braver*>(player);
        Guardian* guardian = dynamic_cast<Guardian*>(player);

        if (mage)
        {
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

// ================================================================
//  角色详情
// ================================================================

void GameManager::ShowCharacterDetail() const
{
    UIManager::ClearScreen();
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                      角色详细信息                             ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ ";
    currentCharacter->DisplayInfo();
    std::cout << "║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    WaitForKeyPress();
}

// ================================================================
//  测试入口（委托给 TestManager）
// ================================================================

void GameManager::TestElementSystemQuick()
{
    TestManager::TestElementSystemQuick();
}

void GameManager::TestInventoryQuick()
{
    TestManager::TestInventoryQuick();
}

void GameManager::TestFullBattleQuick()
{
    TestManager::TestFullBattleQuick();
}
