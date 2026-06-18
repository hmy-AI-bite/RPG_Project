#include "BattleSystem.h"
#include "ATBTest.h"
#include "Skill.h"
#include "DamageSystem.h"
#include "GameConfig.h"
#include <iostream>
#include <iomanip>

// ================================================================
//  战斗 UI 辅助
// ================================================================

void BattleSystem::ShowBattleStatus(Player* player,
    const std::vector<std::shared_ptr<Enemy>>& enemies, int round)
{
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    ATB 战斗 - 第 " << std::setw(2) << std::left << round << " 回合                       ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;

    std::cout << "║  " << std::setw(10) << std::left << player->GetName()
              << "  HP: " << std::setw(5) << std::left << (std::to_string(player->GetHp()) + "/" + std::to_string(player->GetMaxHp()))
              << "  MP: " << std::setw(5) << std::left << (std::to_string(player->GetMp()) + "/" + std::to_string(player->GetMaxMp()))
              << "                   ║" << std::endl;

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
}

// ================================================================
//  处理玩家战斗输入
// ================================================================
bool BattleSystem::HandleBattleInput(Player* player,
    std::vector<std::shared_ptr<Enemy>>& enemies,
    bool& quit)
{
    char ch;
    std::cin >> ch;
    std::cin.ignore();

    if (ch == 'Q' || ch == 'q')
    {
        quit = true;
        return false;
    }

    switch (ch)
    {
    case '1':
    {
        std::cout << "请选择攻击目标: ";
        int targetIdx;
        if (std::cin >> targetIdx)
        {
            targetIdx--;
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
        player->GetInventory().ShowInventory();
        std::cout << "请输入道具序号 (0=取消): ";
        int itemIdx;
        if (std::cin >> itemIdx && itemIdx > 0)
        {
            player->GetInventory().UseItem(itemIdx - 1, player);
        }
    }
    break;

    default:
        std::cout << "无效选择！" << std::endl;
        break;
    }

    return true;
}

// ================================================================
//  处理怪物行动
// ================================================================
void BattleSystem::HandleEnemyAction(Enemy* enemy, Player* player,
    std::vector<std::shared_ptr<Enemy>>& enemies)
{
    std::cout << std::endl << ">>> " << enemy->GetName() << " 开始行动！" << std::endl;
    auto spawned = enemy->PerformAction(player);

    if (!spawned.empty())
    {
        for (auto& newEnemy : spawned)
        {
            enemies.push_back(newEnemy);
            std::cout << "  (新怪物加入了战场！)" << std::endl;
        }
    }
}

// ================================================================
//  发放战斗奖励
// ================================================================
void BattleSystem::GiveBattleReward(Player* player,
    const std::vector<std::shared_ptr<Enemy>>& enemies)
{
    int totalExp = 0;
    int totalGold = 0;
    for (auto& enemy : enemies)
    {
        totalExp += enemy->GetExperienceReward();
        totalGold += enemy->GetGoldReward();
    }

    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                      ★ 战斗胜利！★                             ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║  获得经验: " << std::setw(48) << std::left << totalExp << "║" << std::endl;
    std::cout << "║  获得金币: " << std::setw(48) << std::left << totalGold << "║" << std::endl;

    player->GainExperience(totalExp);
    player->gold += totalGold;

    if (player->GetAvailableAP() > 0)
    {
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║  升级了！获得 " << player->GetAvailableAP() << " 点属性点！                                ║" << std::endl;
    }

    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
}

// ================================================================
//  怪物遭遇战
// ================================================================
bool BattleSystem::BattleEncounter(Player* player,
    std::vector<std::shared_ptr<Enemy>>& enemies)
{
    if (!player) return false;

    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                      野外探索                                  ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║  你踏上了冒险的旅程...                                          ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;

    int playerLevel = player->GetLevel();
    int monsterLevel = playerLevel + (rand() % 5 - 2);
    if (monsterLevel < 1) monsterLevel = 1;

    int monsterCount = (rand() % 3) + 1;
    enemies.clear();

    for (int i = 0; i < monsterCount; ++i)
    {
        int monsterType = rand() % 3;
        int mLevel = monsterLevel + (rand() % 3 - 1);
        if (mLevel < 1) mLevel = 1;

        std::shared_ptr<Enemy> enemy;
        switch (monsterType)
        {
        case 0: enemy = std::make_shared<Slime>(mLevel); break;
        case 1: enemy = std::make_shared<FireLizard>(mLevel); break;
        case 2: enemy = std::make_shared<WindSprite>(mLevel); break;
        }
        enemies.push_back(enemy);
    }

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

    bool victory = RunBattle(player, enemies);

    if (victory)
    {
        GiveBattleReward(player, enemies);
        std::cout << "按任意键继续...";
        std::cin.get();
    }
    else
    {
        std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                      战斗失败...                               ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║  你被击败了，但顽强地活了下来。                                  ║" << std::endl;
        std::cout << "║  回到城镇休息后恢复了 50% 的生命值。                             ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;

        if (player->IsAlive())
        {
            int healAmount = player->GetMaxHp() / 2;
            player->Heal(healAmount);
        }
        std::cout << "按任意键继续...";
        std::cin.get();
    }

    return victory;
}

// ================================================================
//  ATB 战斗主循环
// ================================================================
bool BattleSystem::RunBattle(Player* player,
    std::vector<std::shared_ptr<Enemy>>& enemies)
{
    if (!player) return false;

    player->ResetGauge();
    for (auto& e : enemies)
    {
        e->ResetGauge();
        e->SetHp(e->GetMaxHp());
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
        if (allEnemiesDead) return true;

        int frames = ATBTest::FastForwardToNextAction(player, enemies);
        auto queue = ATBTest::CollectReadyActors(player, enemies);

        for (auto& entry : queue)
        {
            if (entry.isPlayer)
            {
                player->ResetGauge();
                round++;

                system("cls");
                ShowBattleStatus(player, enemies, round);

                HandleBattleInput(player, enemies, quit);

                std::cout << std::endl << "按 Enter 继续...";
                std::cin.get();
            }
            else
            {
                auto& enemy = enemies[entry.enemyIndex];
                if (!enemy->IsAlive()) continue;

                enemy->ResetGauge();
                HandleEnemyAction(enemy.get(), player, enemies);

                std::cout << "按 Enter 继续...";
                std::cin.get();
            }

            if (!player->IsAlive()) return false;
        }
    }

    return false;
}

// ================================================================
//  交互式使用技能（非战斗状态）
// ================================================================
void BattleSystem::UseSkillInteractively(Player* player,
    const std::vector<std::shared_ptr<Enemy>>& enemies)
{
    if (!player) return;

    const auto& skills = player->GetSkills();
    if (skills.empty())
    {
        std::cout << "当前角色没有可用的技能！" << std::endl;
        std::cout << "按任意键继续...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    system("cls");
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
        return;

    int targetIdx;
    std::cin >> targetIdx;

    if (enemies.empty() || targetIdx <= 0 || targetIdx > static_cast<int>(enemies.size()) || !enemies[targetIdx - 1]->IsAlive())
    {
        std::cout << "没有有效目标！技能未释放。" << std::endl;
        std::cout << "按任意键继续...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    player->UseSkill(skillIdx - 1, enemies[targetIdx - 1].get());
    std::cout << "按任意键继续...";
    std::cin.ignore();
    std::cin.get();
}

// ================================================================
//  伤害演示
// ================================================================
void BattleSystem::PerformAttack(std::shared_ptr<Character> attacker,
    std::shared_ptr<Character> defender)
{
    DamageCalculator calculator;

    std::cout << "攻击者信息:" << std::endl;
    attacker->DisplayInfo();
    std::cout << std::endl;

    std::cout << "防御者信息:" << std::endl;
    defender->DisplayInfo();
    std::cout << std::endl;

    // 物理伤害
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

    // 魔法伤害
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

    // 真实伤害
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
