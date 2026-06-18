#include "Monster.h"
#include "Skill.h"
#include "Item.h"
#include <iostream>
#include <cstdlib>

// ========== Enemy 基类 ==========
Enemy::Enemy(const std::string& name, int maxHp, int attackPower, int defend, int level)
    : Character(name, maxHp, attackPower, defend, level),
    experienceReward(0), goldReward(0)
{
}

void Enemy::DisplayInfo() const
{
    std::cout << "[怪物] " << name
        << " | HP:" << hp << "/" << maxHp
        << " ATK:" << attackPower << " DEF:" << defend
        << " SPD:" << speed << " LV:" << level
        << " | EXP:" << experienceReward << " GOLD:" << goldReward << std::endl;
}

// ========== Slime 史莱姆 ==========
Slime::Slime(int level)
    : Enemy("史莱姆", 50, 8, 2, level), splitHealth(25)
{
    maxHp = 30 + level * 5;
    hp = maxHp;
    attackPower = 5 + level * 2;
    defend = 1 + level;
    agility = 20 + level * 2;
    speed = 80;
    spellDefense = 2;
    elementType = ElementType::Water;
    splitHealth = maxHp / 2;
    experienceReward = 20 + level * 5;
    goldReward = 10 + level * 3;
}

std::vector<std::shared_ptr<Enemy>> Slime::PerformAction(Character* target)
{
    if (hp < splitHealth && hp > 0)
    {
        std::cout << name << " 开始分裂！" << std::endl;
        auto newSlime = std::make_shared<Slime>(level);
        newSlime->SetMaxHp(maxHp);
        newSlime->SetHp(hp);
        newSlime->SetAttackPower(attackPower);
        newSlime->SetDefend(defend);
        newSlime->SetLevel(level);
        newSlime->SetAgility(agility);
        newSlime->SetSpeed(speed);
        newSlime->SetSpellDefense(spellDefense);
        newSlime->SetElementType(elementType);
        newSlime->SetPhysicalResistance(resistance.physicalResistance);
        newSlime->SetMagicalResistance(resistance.magicalResistance);
        newSlime->experienceReward = experienceReward;
        newSlime->goldReward = goldReward;

        int healAmount = maxHp / 4;
        hp += healAmount;
        if (hp > maxHp) hp = maxHp;

        std::cout << "  → 分裂出新的 " << newSlime->GetName()
            << " (HP: " << newSlime->GetHp() << "/" << newSlime->GetMaxHp() << ")" << std::endl;
        std::cout << "  → " << name << " 恢复了 " << healAmount << " 点生命值！" << std::endl;
        return { newSlime };
    }
    else
    {
        PerformPhysicalAttack(target, 1.2, " 弹跳攻击！");
        return {};
    }
}

std::vector<Item*> Slime::GenerateDrops() const
{
    std::vector<Item*> drops;
    if (rand() % 100 < 30)
        drops.push_back(Potion::CreateSmallHealthPotion());
    return drops;
}

void Slime::DisplayInfo() const
{
    std::cout << "[史莱姆] " << name << " Lv." << level
        << " | HP:" << hp << "/" << maxHp
        << " ATK:" << attackPower << " DEF:" << defend
        << " SPD:" << speed
        << " | EXP:" << experienceReward << " GOLD:" << goldReward << std::endl;
}

// ========== Goblin 哥布林 ==========
Goblin::Goblin(int level)
    : Enemy("哥布林", 25 + level * 4, 6 + level * 2, 2 + level, level)
{
    agility = 15 + level * 2;
    speed = 90;
    spellDefense = 3;
    elementType = ElementType::Earth;
    experienceReward = 15 + level * 4;
    goldReward = 8 + level * 2;
    AddSkill(std::make_shared<PhysicalSkill>("飞刀投掷", 1.3, 5, ElementType::Neutral));
}

std::vector<std::shared_ptr<Enemy>> Goblin::PerformAction(Character* target)
{
    if (!skills.empty() && (rand() % 2 == 0))   // 50% 概率使用技能
    {
        DamageInfo info = skills[0]->ExecuteSkill(this, target);
        target->TakeDamage(info);
    }
    else
    {
        PerformPhysicalAttack(target, 1.0, " 挥动小刀！");
    }
    return {};
}

std::vector<Item*> Goblin::GenerateDrops() const
{
    std::vector<Item*> drops;
    if (rand() % 100 < 20)
        drops.push_back(Potion::CreateSmallHealthPotion());
    return drops;
}

void Goblin::DisplayInfo() const
{
    std::cout << "[哥布林] " << name << " Lv." << level
        << " HP:" << hp << "/" << maxHp
        << " ATK:" << attackPower << " DEF:" << defend
        << " EXP:" << experienceReward << " GOLD:" << goldReward << std::endl;
}

// ========== Skeleton 骷髅兵 ==========
Skeleton::Skeleton(int level)
    : Enemy("骷髅兵", 35 + level * 5, 10 + level * 2, 5 + level, level)
{
    agility = 10 + level;
    speed = 70;
    spellDefense = 6;
    elementType = ElementType::Neutral;
    SetPhysicalResistance(0.15);
    experienceReward = 25 + level * 5;
    goldReward = 12 + level * 3;
    AddSkill(std::make_shared<PhysicalSkill>("骨刺", 1.5, 10, ElementType::Neutral));
}

std::vector<std::shared_ptr<Enemy>> Skeleton::PerformAction(Character* target)
{
    if (!skills.empty() && (rand() % 3 != 0))   // 2/3 概率使用技能
    {
        DamageInfo info = skills[0]->ExecuteSkill(this, target);
        target->TakeDamage(info);
    }
    else
    {
        PerformPhysicalAttack(target, 1.0, " 用骨头砸！");
    }
    return {};
}

std::vector<Item*> Skeleton::GenerateDrops() const
{
    std::vector<Item*> drops;
    if (rand() % 100 < 25)
        drops.push_back(Potion::CreateMediumHealthPotion());
    if (rand() % 100 < 10)
        drops.push_back(Potion::CreateStrengthPotion());
    return drops;
}

void Skeleton::DisplayInfo() const
{
    std::cout << "[骷髅兵] " << name << " Lv." << level
        << " HP:" << hp << "/" << maxHp
        << " ATK:" << attackPower << " DEF:" << defend
        << " PHY-RES:" << resistance.physicalResistance * 100 << "%"
        << " EXP:" << experienceReward << " GOLD:" << goldReward << std::endl;
}

// ========== DarkMage 暗影法师 ==========
DarkMage::DarkMage(int level)
    : Enemy("暗影法师", 20 + level * 3, 4 + level, 1 + level / 2, level)
{
    agility = 20 + level * 3;
    speed = 100;
    spellDefense = 15;
    elementType = ElementType::Fire;
    SetMagicalResistance(0.2);
    experienceReward = 30 + level * 6;
    goldReward = 15 + level * 4;
    AddSkill(std::make_shared<MagicalSkill>("暗影弹", 1.6, 12, 40 + level * 5, ElementType::Fire));
}

std::vector<std::shared_ptr<Enemy>> DarkMage::PerformAction(Character* target)
{
    if (!skills.empty() && (rand() % 2 == 0))   // 50% 概率使用技能
    {
        DamageInfo info = skills[0]->ExecuteSkill(this, target);
        target->TakeDamage(info);
    }
    else
    {
        PerformPhysicalAttack(target, 1.0, " 用杖敲击！");
    }
    return {};
}

std::vector<Item*> DarkMage::GenerateDrops() const
{
    std::vector<Item*> drops;
    if (rand() % 100 < 30) drops.push_back(Potion::CreateAgilityPotion());
    if (rand() % 100 < 15) drops.push_back(Weapon::CreateStaff());
    return drops;
}

void DarkMage::DisplayInfo() const
{
    std::cout << "[暗影法师] " << name << " Lv." << level
        << " HP:" << hp << "/" << maxHp
        << " MAG-RES:" << resistance.magicalResistance * 100 << "%"
        << " EXP:" << experienceReward << " GOLD:" << goldReward << std::endl;
}

// ========== Boss 巨龙 ==========
Boss::Boss(int level)
    : Enemy("巨龙", 80 + level * 10, 20 + level * 3, 10 + level, level)
{
    agility = 15 + level * 2;
    speed = 60;
    spellDefense = 12;
    elementType = ElementType::Fire;
    SetPhysicalResistance(0.25);
    SetMagicalResistance(0.25);
    experienceReward = 100 + level * 20;
    goldReward = 50 + level * 10;
    AddSkill(std::make_shared<TrueSkill>("龙息", 50, 25));
    AddSkill(std::make_shared<PhysicalSkill>("撕咬", 2.0, 15, ElementType::Neutral));
}

std::vector<std::shared_ptr<Enemy>> Boss::PerformAction(Character* target)
{
    // 优先使用真实伤害技能，其次物理技能，否则普通攻击
    if (!skills.empty() && (rand() % 3 != 0))   // 2/3 概率用第一个技能
    {
        DamageInfo info = skills[0]->ExecuteSkill(this, target);
        target->TakeDamage(info);
    }
    else if (skills.size() > 1 && (rand() % 2 == 0))   // 剩余情况用第二个技能
    {
        DamageInfo info = skills[1]->ExecuteSkill(this, target);
        target->TakeDamage(info);
    }
    else
    {
        PerformPhysicalAttack(target, 1.2, " 龙爪猛击！");
    }
    return {};
}

std::vector<Item*> Boss::GenerateDrops() const
{
    std::vector<Item*> drops;
    drops.push_back(Weapon::CreateGreatSword());   // 必掉巨剑
    drops.push_back(Potion::CreateDefensePotion()); // 必掉护盾药水
    if (rand() % 100 < 50) drops.push_back(Weapon::CreateShield());
    return drops;
}

void Boss::DisplayInfo() const
{
    std::cout << "[BOSS] " << name << " Lv." << level
        << " HP:" << hp << "/" << maxHp
        << " ATK:" << attackPower << " DEF:" << defend
        << " PHY-RES:" << resistance.physicalResistance * 100
        << "% MAG-RES:" << resistance.magicalResistance * 100 << "%"
        << " EXP:" << experienceReward << " GOLD:" << goldReward << std::endl;
}