#include "UIManager.h"
#include "TestManager.h"
#include "Skill.h"
#include "Item.h"
#include "Monster.h"
#include <iostream>
#include <iomanip>

// ================================================================
//  UI 工具方法
// ================================================================

void UIManager::WaitForKeyPress()
{
    std::cout << "按任意键继续...";
    std::cin.ignore();
    std::cin.get();
}

void UIManager::ClearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void UIManager::DrawProgressBar(int percent, int barLength,
    const std::string& filledChar, const std::string& emptyChar)
{
    int filled = (percent * barLength) / 100;
    for (int i = 0; i < barLength; ++i)
    {
        std::cout << (i < filled ? filledChar : emptyChar);
    }
}

void UIManager::DrawSeparator(const std::string& title)
{
    std::cout << "\n============================================================\n";
    if (!title.empty())
        std::cout << "  " << title << "\n";
    std::cout << "============================================================\n";
}

// ================================================================
//  主控面板
// ================================================================
void UIManager::DisplayDashboard(const std::shared_ptr<Character>& currentCharacter)
{
    ClearScreen();

    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    RPG 游戏 - 主控面板                          ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;

    if (currentCharacter)
    {
        std::cout << "┌─ 当前角色信息 ─────────────────────────────────────────────┐" << std::endl;

        std::cout << "│ " << std::setw(60) << std::left
                  << currentCharacter->GetName() + " [LV." + std::to_string(currentCharacter->GetLevel()) + "]"
                  << "│" << std::endl;

        // HP 血条
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

            // 经验条
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

        // 行动条
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
            Weapon* eqWeapon = player->GetEquippedWeapon();
            if (eqWeapon)
                std::cout << "│ 装备武器:     " << std::setw(45) << std::left << eqWeapon->GetName() << "│" << std::endl;
        }

        // 职业特有信息
        currentCharacter->DisplayClassSpecificInfo();

        std::cout << "└────────────────────────────────────────────────────────────────┘" << std::endl;
    }
    else
    {
        std::cout << "│ 没有可用的角色" << std::endl;
    }

    std::cout << std::endl;
}

// ================================================================
//  主菜单
// ================================================================
void UIManager::DisplayMainMenu()
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
void UIManager::DisplayCharacterList(
    const std::vector<std::shared_ptr<Character>>& characters,
    const std::shared_ptr<Character>& currentCharacter)
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
void UIManager::DisplayEnemies(const std::vector<std::shared_ptr<Enemy>>& enemies)
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
//  标题画面
// ================================================================
char UIManager::ShowTitleScreen()
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
    return choice;
}

// ================================================================
//  创建角色流程
// ================================================================
std::shared_ptr<Player> UIManager::CreateCharacterFlow()
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
        return nullptr;  // 调用方应重新调用
    }
    std::cin.ignore();

    // 第二步：输入昵称
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

    if (nickname.empty())
    {
        std::cout << std::endl << "  昵称不能为空！请重新输入。" << std::endl;
        WaitForKeyPress();
        return nullptr;
    }

    if (nickname.length() > 10)
    {
        nickname = nickname.substr(0, 10);
        std::cout << "  昵称过长，已自动截断为: " << nickname << std::endl;
    }

    // 第三步：确认并创建角色
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
        return nullptr;
    }

    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║  角色创建成功！                                                ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    WaitForKeyPress();

    // 第四步：初始物品发放
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

    TestManager::InitPlayerInventory(newPlayer.get());

    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    WaitForKeyPress();

    return newPlayer;
}

// ================================================================
//  技能学习流程
// ================================================================
void UIManager::SkillLearningFlow(Player* player)
{
    if (!player) return;

    ClearScreen();
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    技能学习                                    ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;

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

        // 守护姿态和圣光斩是内置技能，这里给物理技能
        player->AddSkill(std::make_shared<PhysicalSkill>("盾击", 1.3, 18, ElementType::Earth));
    }

    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║  技能学习完成！                                                ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    WaitForKeyPress();
}

// ================================================================
//  进入游戏过渡画面
// ================================================================
void UIManager::EnterGameScreen(const std::shared_ptr<Character>& currentCharacter)
{
    ClearScreen();

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
}
