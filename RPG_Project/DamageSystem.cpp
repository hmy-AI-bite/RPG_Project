#include "DamageSystem.h"
#include <iostream>
#include <sstream>

DamageCalculator::DamageCalculator() : gen(rd())
{
}

double DamageCalculator::GetRandomDamageMultiplier()
{
    std::uniform_real_distribution<> dis(0.9, 1.1);
    return dis(gen);
}

double DamageCalculator::GetLevelDifferenceBonus(int attackerLevel, int defenderLevel)
{
    int levelDiff = attackerLevel - defenderLevel;
    return 1.0 + (levelDiff * 0.025);
}

double DamageCalculator::GetElementBonus(ElementType attackerElement, ElementType defenderElement)
{
    // 属性克制关系：火→风→地→水→火
    if (attackerElement == ElementType::Neutral || defenderElement == ElementType::Neutral)
        return 1.0;

    // 克制关系
    if ((attackerElement == ElementType::Fire && defenderElement == ElementType::Wind) ||
        (attackerElement == ElementType::Wind && defenderElement == ElementType::Earth) ||
        (attackerElement == ElementType::Earth && defenderElement == ElementType::Water) ||
        (attackerElement == ElementType::Water && defenderElement == ElementType::Fire))
    {
        return 1.5;  // 克制伤害提升
    }

    // 被克制关系
    if ((attackerElement == ElementType::Wind && defenderElement == ElementType::Fire) ||
        (attackerElement == ElementType::Earth && defenderElement == ElementType::Wind) ||
        (attackerElement == ElementType::Water && defenderElement == ElementType::Earth) ||
        (attackerElement == ElementType::Fire && defenderElement == ElementType::Water))
    {
        return 0.75;  // 被克制伤害降低
    }

    return 1.0;
}

double DamageCalculator::GetCriticalChance(int agility)
{
    // 暴击率 = 灵巧 / 1000，最高30%
    return std::min(0.30, agility / 1000.0);
}

bool DamageCalculator::TriggerCritical(int agility)
{
    double critChance = GetCriticalChance(agility);
    std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen) < critChance;
}

DamageInfo DamageCalculator::CalculatePhysicalDamage(
    double attackerAttack,
    double defenderDefense,
    int attackerLevel,
    int defenderLevel,
    double skillRatio,
    ElementType elementType,
    int agility,
    ResistanceProfile resistance)
{
    DamageInfo info;

    // 基础伤害计算
    double baseDamage = std::max(
        (attackerAttack * 2.0 - defenderDefense * 1.5),
        attackerAttack * 0.2
    );

    // 应用技能倍率
    baseDamage *= skillRatio;

    // 随机浮动 (0.9-1.1)
    baseDamage *= GetRandomDamageMultiplier();

    // 等级差加成
    baseDamage *= GetLevelDifferenceBonus(attackerLevel, defenderLevel);

    info.baseDamage = baseDamage;

    // 属性克制
    double elementBonus = GetElementBonus(elementType, ElementType::Neutral);
    info.elementBonus = elementBonus;
    baseDamage *= elementBonus;

    // 暴击判定
    info.isCritical = TriggerCritical(agility);
    if (info.isCritical)
    {
        baseDamage *= 1.5;
    }

    // 抗性计算
    double resistanceReduction = 1.0 - resistance.physicalResistance;
    info.resistanceReduction = resistanceReduction;
    baseDamage *= resistanceReduction;

    // 最终伤害
    info.finalDamage = std::max(1.0, baseDamage);  // 至少造成1点伤害

    return info;
}

DamageInfo DamageCalculator::CalculateMagicalDamage(
    double attackerSpellPower,
    double defenderSpellDefense,
    int attackerLevel,
    int defenderLevel,
    double skillRatio,
    ElementType elementType,
    int agility,
    ResistanceProfile resistance)
{
    DamageInfo info;

    // 攻击方有效法术攻击
    double effectiveSpellPower = attackerSpellPower;

    // 防御方法术减伤比
    double magicDamageReduction = defenderSpellDefense / (defenderSpellDefense + 200.0);

    // 伤害系数
    double damageCoefficient = 1.6 - magicDamageReduction;

    // 基础伤害
    double baseDamage = effectiveSpellPower * damageCoefficient;

    // 应用最小伤害限制
    baseDamage = std::max(baseDamage, effectiveSpellPower * 0.2);

    info.baseDamage = baseDamage;

    // 应用技能倍率
    baseDamage *= skillRatio;

    // 随机浮动 (0.9-1.1)
    baseDamage *= GetRandomDamageMultiplier();

    // 等级差加成
    baseDamage *= GetLevelDifferenceBonus(attackerLevel, defenderLevel);

    // 属性克制
    double elementBonus = GetElementBonus(elementType, ElementType::Neutral);
    info.elementBonus = elementBonus;
    baseDamage *= elementBonus;

    // 暴击判定
    info.isCritical = TriggerCritical(agility);
    if (info.isCritical)
    {
        baseDamage *= 1.5;
    }

    // 抗性计算
    double resistanceReduction = 1.0 - resistance.magicalResistance;
    info.resistanceReduction = resistanceReduction;
    baseDamage *= resistanceReduction;

    // 最终伤害
    info.finalDamage = std::max(1.0, baseDamage);  // 至少造成1点伤害

    return info;
}

DamageInfo DamageCalculator::CalculateTrueDamage(
    double baseTrueDamage,
    double attackerAttack,
    int attackerLevel,
    int defenderLevel)
{
    DamageInfo info;

    // 真实伤害 = 固定值 + 攻击力 × 0.5
    // 无视防御和抗性
    double trueDamage = baseTrueDamage + attackerAttack * 0.5;

    // 随机浮动 (0.9-1.1)
    trueDamage *= GetRandomDamageMultiplier();

    // 等级差加成
    trueDamage *= GetLevelDifferenceBonus(attackerLevel, defenderLevel);

    info.baseDamage = trueDamage;
    info.finalDamage = std::max(1.0, trueDamage);
    info.isCritical = false;
    info.elementBonus = 1.0;
    info.resistanceReduction = 1.0;

    return info;
}
