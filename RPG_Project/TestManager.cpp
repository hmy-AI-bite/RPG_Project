#include "TestManager.h"
#include "ATBTest.h"
#include "Skill.h"
#include "Monster.h"
#include "DamageSystem.h"
#include "Item.h"
#include "UIManager.h"
#include "GameConfig.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

// ================================================================
//  辅助方法
// ================================================================

void TestManager::ExecuteAttack(std::shared_ptr<Character> attacker,
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

// ================================================================
//  InitPlayerInventory
// ================================================================
void TestManager::InitPlayerInventory(Player* player)
{
    if (!player) return;

    player->GetInventory().AddItem(Potion::CreateSmallHealthPotion());
    player->GetInventory().AddItem(Potion::CreateSmallHealthPotion());
    player->GetInventory().AddItem(Potion::CreateMediumHealthPotion());
    player->GetInventory().AddItem(Weapon::CreateDagger());

    std::cout << "[系统] 已为 " << player->GetName() << " 添加初始道具！" << std::endl;
}

// ================================================================
//  TestElementSystemQuick
// ================================================================
void TestManager::TestElementSystemQuick()
{
    UIManager::ClearScreen();
    UIManager::DrawSeparator("T1：属性克制全覆盖测试（9 项子测试）");

    auto attacker = std::make_shared<Braver>("测试者");
    attacker->SetAttackPower(30);
    attacker->SetLevel(1);

    int totalTests = 0;
    int passCount = 0;

    auto ResetMonster = [](std::shared_ptr<Enemy>& m) {
        m->SetMaxHp(200);
        m->SetHp(200);
    };

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
        if (expectedBonus >= 1.4)
            pass = (d.elementBonus >= 1.4 && d.elementBonus <= 1.6);
        else if (expectedBonus <= 0.8)
            pass = (d.elementBonus >= 0.7 && d.elementBonus <= 0.8);
        else
            pass = (d.elementBonus >= 0.95 && d.elementBonus <= 1.05);

        if (pass) passCount++;
        std::cout << "  >> " << (pass ? "PASS" : "FAIL") << " <<" << std::endl;

        ResetMonster(monster);
    };

    RunElementTest("火 → 风", ElementType::Fire, ElementType::Wind, 1.5, "火克风");
    RunElementTest("风 → 火", ElementType::Wind, ElementType::Fire, 0.75, "风被火克");
    RunElementTest("风 → 地", ElementType::Wind, ElementType::Earth, 1.5, "风克地");
    RunElementTest("地 → 风", ElementType::Earth, ElementType::Wind, 0.75, "地被风克");
    RunElementTest("地 → 水", ElementType::Earth, ElementType::Water, 1.5, "地克水");
    RunElementTest("水 → 地", ElementType::Water, ElementType::Earth, 0.75, "水被地克");
    RunElementTest("水 → 火", ElementType::Water, ElementType::Fire, 1.5, "水克火");
    RunElementTest("火 → 水", ElementType::Fire, ElementType::Water, 0.75, "火被水克");
    RunElementTest("中立 → 中立", ElementType::Neutral, ElementType::Neutral, 1.0, "无克制");

    std::cout << "\n============================================================" << std::endl;
    std::cout << "  汇总: " << passCount << "/" << totalTests << " 通过" << std::endl;
    std::cout << "  最终结论: " << (passCount == totalTests ? ">>> ALL PASS <<<" : ">>> SOME FAIL <<<") << std::endl;
    std::cout << "============================================================" << std::endl;

    std::cout << "\n按 Enter 继续...";
    std::cin.get();
}

// ================================================================
//  TestInventoryQuick
// ================================================================
void TestManager::TestInventoryQuick()
{
    UIManager::ClearScreen();
    UIManager::DrawSeparator("T2：背包与道具验证（4 步骤）");

    auto player = std::make_shared<Braver>("测试玩家");
    int totalTests = 0;
    int passCount = 0;

    // 步骤1
    std::cout << "\n--- 步骤1：初始化背包并验证道具数量 ---" << std::endl;
    InitPlayerInventory(player.get());
    int itemCount = static_cast<int>(player->GetInventory().GetItemCount());
    std::cout << "  背包道具数: " << itemCount << std::endl;
    player->GetInventory().ShowInventory();

    totalTests++;
    bool pass1 = (itemCount == 4);
    std::cout << "  >> " << (pass1 ? "PASS" : "FAIL") << " (期望 4 个道具) <<" << std::endl;
    if (pass1) passCount++;

    // 步骤2
    std::cout << "\n--- 步骤2：使用小型治疗药水（恢复20HP）---" << std::endl;
    int halfHp = player->GetMaxHp() / 2;
    int damageToDeal = player->GetHp() - halfHp;
    player->TakeDamage(damageToDeal);
    std::cout << "  先造成 " << damageToDeal << " 点伤害" << std::endl;
    std::cout << "  使用前 HP: " << player->GetHp() << "/" << player->GetMaxHp() << std::endl;

    int hpBefore = player->GetHp();
    player->GetInventory().UseItem(0, player.get());
    int hpAfter = player->GetHp();
    int healAmount = hpAfter - hpBefore;
    std::cout << "  使用后 HP: " << hpAfter << "/" << player->GetMaxHp() << " (+" << healAmount << ")" << std::endl;

    totalTests++;
    bool pass2 = (healAmount == 20);
    std::cout << "  >> " << (pass2 ? "PASS" : "FAIL") << " (期望恢复 20HP) <<" << std::endl;
    if (pass2) passCount++;

    // 步骤3
    std::cout << "\n--- 步骤3：使用中型治疗药水（恢复50HP）---" << std::endl;
    damageToDeal = player->GetHp() - halfHp;
    if (damageToDeal > 0)
    {
        player->TakeDamage(damageToDeal);
        std::cout << "  先造成 " << damageToDeal << " 点伤害" << std::endl;
    }
    std::cout << "  使用前 HP: " << player->GetHp() << "/" << player->GetMaxHp() << std::endl;

    hpBefore = player->GetHp();
    player->GetInventory().UseItem(0, player.get());
    hpAfter = player->GetHp();
    healAmount = hpAfter - hpBefore;
    std::cout << "  使用后 HP: " << hpAfter << "/" << player->GetMaxHp() << " (+" << healAmount << ")" << std::endl;

    totalTests++;
    bool pass3 = (healAmount == 50);
    std::cout << "  >> " << (pass3 ? "PASS" : "FAIL") << " (期望恢复 50HP) <<" << std::endl;
    if (pass3) passCount++;

    // 步骤4
    std::cout << "\n--- 步骤4：装备匕首（ATK+3, AGI+8, SPD+3）---" << std::endl;
    int atkBefore = player->GetAttackPower();
    int agiBefore = player->GetAgility();
    int spdBefore = player->GetSpeed();
    std::cout << "  装备前: ATK=" << atkBefore << "  AGI=" << agiBefore << "  SPD=" << spdBefore << std::endl;

    player->GetInventory().AddItem(Weapon::CreateDagger());
    player->GetInventory().UseItem(0, player.get());

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

    std::cout << "\n--- 最终背包状态 ---" << std::endl;
    std::cout << "剩余道具数: " << player->GetInventory().GetItemCount() << std::endl;
    player->GetInventory().ShowInventory();

    std::cout << "\n============================================================" << std::endl;
    std::cout << "  汇总: " << passCount << "/" << totalTests << " 通过" << std::endl;
    std::cout << "  最终结论: " << (passCount == totalTests ? ">>> ALL PASS <<<" : ">>> SOME FAIL <<<") << std::endl;
    std::cout << "============================================================" << std::endl;

    std::cout << "\n按 Enter 继续...";
    std::cin.get();
}

// ================================================================
//  TestFullBattleQuick
// ================================================================
void TestManager::TestFullBattleQuick()
{
    UIManager::ClearScreen();
    UIManager::DrawSeparator("T3：自动战斗测试（3 个场景）");

    // 场景1：Braver vs FireLizard
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

    // 场景2：Guardian vs Slime
    {
        UIManager::ClearScreen();
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
                    std::cout << "\n[回合" << round << "] 史莱姆攻击！（护盾前: " << shieldBefore << "）" << std::endl;
                    enemy->PerformAction(guardian.get());
                    int shieldAfter = guardian->GetShield();

                    if (!shieldVerified && shieldBefore > 0 && shieldAfter < shieldBefore)
                    {
                        shieldVerified = true;
                        std::cout << "  ✓ 护盾机制验证通过！护盾 " << shieldBefore
                                  << " → " << shieldAfter << "（优先吸收伤害）" << std::endl;
                    }
                    std::cout << "  护盾: " << shieldAfter
                              << "  HP:" << guardian->GetHp() << "/" << guardian->GetMaxHp()
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

    // 场景3：Mage vs WindSprite
    {
        UIManager::ClearScreen();
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
//  TestSlime
// ================================================================
void TestManager::TestSlime()
{
    UIManager::ClearScreen();
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    史莱姆战斗测试                             ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;

    // 直接创建测试玩家
    auto player = std::make_shared<Braver>("测试勇者");
    std::cout << "║ 生成史莱姆..." << std::endl;
    auto slime1 = std::make_shared<Slime>(1);
    auto slime2 = std::make_shared<Slime>(3);
    auto slime3 = std::make_shared<Slime>(5);

    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ 玩家信息:" << std::endl;
    std::cout << "║ ";
    player->DisplayInfo();
    std::cout << " ║" << std::endl;

    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ 史莱姆敌人信息:" << std::endl;
    std::cout << "║ "; slime1->DisplayInfo(); std::cout << " ║" << std::endl;
    std::cout << "║ "; slime2->DisplayInfo(); std::cout << " ║" << std::endl;
    std::cout << "║ "; slime3->DisplayInfo(); std::cout << " ║" << std::endl;

    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ 测试1: 玩家普通攻击史莱姆1" << std::endl;
    player->NormalAttack(slime1.get());

    std::cout << std::endl << "║ 测试2: 史莱姆1普通攻击玩家" << std::endl;
    slime1->PerformAction(player.get());

    std::cout << std::endl << "║ 测试3: 史莱姆血量低时的分裂攻击" << std::endl;
    while (slime2->GetHp() > slime2->GetMaxHp() / 2)
        slime2->TakeDamage(10);
    slime2->PerformAction(player.get());

    std::cout << std::endl << "║ 测试结束！" << std::endl;

    int expReward = 50;
    std::cout << "║ 战斗胜利！获得 " << expReward << " 经验值！" << std::endl;
    player->GainExperience(expReward);

    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
}

// ================================================================
//  TestATB_1v1
// ================================================================
void TestManager::TestATB_1v1()
{
    UIManager::ClearScreen();
    UIManager::DrawSeparator("测试1：ATB战斗 - 1v1（无跑条动画，仅结算+行动顺序）");

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
        UIManager::DrawSeparator("第 " + std::to_string(++round) + " 次行动 (推进 " + std::to_string(frames) + " 帧)");
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
//  TestSlimeSplit
// ================================================================
void TestManager::TestSlimeSplit()
{
    UIManager::ClearScreen();
    UIManager::DrawSeparator("测试2：史莱姆分裂验证");

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

        UIManager::DrawSeparator("第 " + std::to_string(round) + " 次行动 (推进 " + std::to_string(frames) + " 帧)");
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
void TestManager::TestFullBattle()
{
    UIManager::ClearScreen();
    UIManager::DrawSeparator("测试3：完整ATB战斗 - 多敌人+技能+分裂");

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
        UIManager::DrawSeparator("行动 #" + std::to_string(++round) + " (推进 " + std::to_string(frames) + " 帧)");
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
            int totalExp = 20 + 1 * 5 + 20 + 2 * 5 + 20 + 4 * 5;
            int totalGold = 10 + 1 * 3 + 10 + 2 * 3 + 10 + 4 * 3;
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
//  TestAllClasses
// ================================================================
void TestManager::TestAllClasses()
{
    UIManager::ClearScreen();
    UIManager::DrawSeparator("测试4：三种职业 vs 史莱姆（ATB速度对比）");

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

// ================================================================
//  DemonstrateDamageSystem
// ================================================================
void TestManager::DemonstrateDamageSystem()
{
    UIManager::ClearScreen();
    std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    伤害系统演示                                ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;

    // 创建两个测试角色
    auto attacker = std::make_shared<Braver>("攻击者·测试");
    auto defender = std::make_shared<Guardian>("防御者·测试");

    std::cout << "║ [0] " << std::setw(58) << std::left << attacker->GetName() << "║" << std::endl;
    std::cout << "║ [1] " << std::setw(58) << std::left << defender->GetName() << "║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "自动选择攻击者[0] vs 防御者[1]" << std::endl;

    std::cout << std::endl;
    ExecuteAttack(attacker, defender);

    std::cout << std::endl;
    std::cout << "按任意键继续...";
    std::cin.ignore();
    std::cin.get();
}
