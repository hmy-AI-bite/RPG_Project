#include "Skill.h"
#include "Character.h"
#include <iostream>

// ===== Skill 基础类实现 =====
Skill::Skill(const std::string& name, DamageType type, double ratio, int mpCost, ElementType element)
    : skillName(name), damageType(type), skillRatio(ratio), mpCost(mpCost), elementType(element)
{
}

Skill::~Skill()
{
}

DamageInfo Skill::ExecuteSkill(Character* attacker, Character* defender)
{
    DamageInfo info;
    info.finalDamage = 0;
    return info;
}

// ===== PhysicalSkill 物理技能实现 =====
PhysicalSkill::PhysicalSkill(const std::string& name, double ratio, int mpCost, ElementType element)
    : Skill(name, DamageType::Physical, ratio, mpCost, element)
{
}

DamageInfo PhysicalSkill::ExecuteSkill(Character* attacker, Character* defender)
{
    if (!attacker || !defender) return DamageInfo();

    DamageInfo info = calculator.CalculatePhysicalDamage(
        attacker->GetAttackPower(),
        defender->GetDefend(),
        attacker->GetLevel(),
        defender->GetLevel(),
        skillRatio,
        elementType,
        defender->GetElementType(),
        attacker->GetAgility(),
        defender->GetResistance()
    );

    std::cout << attacker->GetName() << " 使用了 [" << skillName << "] 对 " << defender->GetName() << " 造成伤害！" << std::endl;
    return info;
}

// ===== MagicalSkill 魔法技能实现 =====
MagicalSkill::MagicalSkill(const std::string& name, double ratio, int mpCost, int basePower, ElementType element)
    : Skill(name, DamageType::Magical, ratio, mpCost, element), spellPower(basePower)
{
}

DamageInfo MagicalSkill::ExecuteSkill(Character* attacker, Character* defender)
{
    if (!attacker || !defender) return DamageInfo();

    DamageInfo info = calculator.CalculateMagicalDamage(
        spellPower,
        defender->GetSpellDefense(),
        attacker->GetLevel(),
        defender->GetLevel(),
        skillRatio,
        elementType,
        defender->GetElementType(),
        attacker->GetAgility(),
        defender->GetResistance()
    );

    std::cout << attacker->GetName() << " 使用了 [" << skillName << "] 对 " << defender->GetName() << " 造成魔法伤害！" << std::endl;
    return info;
}

// ===== TrueSkill 真实伤害技能实现 =====
TrueSkill::TrueSkill(const std::string& name, double baseDamage, int mpCost)
    : Skill(name, DamageType::True, 1.0, mpCost), baseTrueDamage(baseDamage)
{
}

DamageInfo TrueSkill::ExecuteSkill(Character* attacker, Character* defender)
{
    if (!attacker || !defender) return DamageInfo();

    DamageInfo info = calculator.CalculateTrueDamage(
        baseTrueDamage,
        attacker->GetAttackPower(),
        attacker->GetLevel(),
        defender->GetLevel()
    );

    std::cout << attacker->GetName() << " 使用了 [" << skillName << "] 对 " << defender->GetName() << " 造成真实伤害！" << std::endl;
    return info;
}
