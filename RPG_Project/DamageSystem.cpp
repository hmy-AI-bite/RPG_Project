#include "DamageSystem.h"
#include <iostream>
#include <sstream>

// ================================================================
//  DamageCalculator - 伤害计算器
//  游戏里所有伤害计算的"大脑"，就在这里！
//
//  伤害计算分三大类：
//    1. 物理伤害 - 看攻击力和防御力
//    2. 魔法伤害 - 看法术强度和法术防御
//    3. 真实伤害 - 无视一切防御！
// ================================================================

// ---------- 构造函数 ----------
// 初始化随机数生成器（用于伤害浮动和暴击判定）
DamageCalculator::DamageCalculator() : gen(rd())
{
}

// ================================================================
//  工具方法
// ================================================================

// ---------- 随机伤害浮动 ----------
// 返回 0.9 ~ 1.1 之间的随机数
// 作用：让每次攻击的伤害不是固定值，而是有小幅度波动
//       比如原本伤害 100，可能变成 90~110 之间
double DamageCalculator::GetRandomDamageMultiplier()
{
    std::uniform_real_distribution<> dis(0.9, 1.1);
    return dis(gen);
}

// ---------- 等级差加成 ----------
// 等级高的打等级低的更疼！
// 公式：加成 = 1.0 + (等级差 × 0.025)
// 比如你 10 级打 5 级怪：加成 = 1.0 + (5 × 0.025) = 1.125（伤害 +12.5%）
double DamageCalculator::GetLevelDifferenceBonus(int attackerLevel, int defenderLevel)
{
    int levelDiff = attackerLevel - defenderLevel;
    return 1.0 + (levelDiff * 0.025);
}

// ---------- 属性克制 ----------
// 属性克制关系（像"石头剪刀布"）：
//   火 → 风 → 地 → 水 → 火 （循环克制）
//
// 规则：
//   如果你克制对方 → 返回 1.5（1.5 倍伤害！）
//   如果你被对方克制 → 返回 0.75（只有 75% 伤害...）
//   如果是中立属性或无克制关系 → 返回 1.0（正常伤害）
double DamageCalculator::GetElementBonus(ElementType attackerElement, ElementType defenderElement)
{
    // 如果任意一方是中立属性，没有克制关系
    if (attackerElement == ElementType::Neutral || defenderElement == ElementType::Neutral)
        return 1.0;

    // 检查：攻击者是否克制防御者？
    // 火克风、风克地、地克水、水克火
    if ((attackerElement == ElementType::Fire && defenderElement == ElementType::Wind) ||
        (attackerElement == ElementType::Wind && defenderElement == ElementType::Earth) ||
        (attackerElement == ElementType::Earth && defenderElement == ElementType::Water) ||
        (attackerElement == ElementType::Water && defenderElement == ElementType::Fire))
    {
        return 1.5;     // 克制！伤害 ×1.5
    }

    // 检查：攻击者是否被防御者克制？
    // 风被火克、地被风克、水被地克、火被水克
    if ((attackerElement == ElementType::Wind && defenderElement == ElementType::Fire) ||
        (attackerElement == ElementType::Earth && defenderElement == ElementType::Wind) ||
        (attackerElement == ElementType::Water && defenderElement == ElementType::Earth) ||
        (attackerElement == ElementType::Fire && defenderElement == ElementType::Water))
    {
        return 0.75;    // 被克制！伤害 ×0.75
    }

    return 1.0;         // 没有克制关系，正常伤害
}

// ---------- 暴击率计算 ----------
// 公式：暴击率 = 灵巧 ÷ 1000
// 比如灵巧 100 → 暴击率 10%
// 最高暴击率 30%（防止太变态）
double DamageCalculator::GetCriticalChance(int agility)
{
    return std::min(0.30, agility / 1000.0);
}

// ---------- 暴击判定（掷骰子！）----------
// 生成一个 0~1 的随机数，如果小于暴击率就暴击
// 比如暴击率 10%（0.1），随机到 0.05 就暴击，随机到 0.5 就不暴击
bool DamageCalculator::TriggerCritical(int agility)
{
    double critChance = GetCriticalChance(agility);
    std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen) < critChance;
}

// ================================================================
//  伤害修正应用（ApplyDamageModifiers）
//
//  这是伤害计算的"流水线"！
//  基础伤害经过这条流水线后，变成最终伤害：
//
//  基础伤害
//    → × 技能倍率（普通攻击 1.0，技能 1.5~1.8）
//    → × 随机浮动（0.9~1.1，让伤害有波动）
//    → × 等级差加成（高等级打低等级更疼）
//    → × 属性克制（克制 1.5x，被克 0.75x）
//    → × 暴击（如果暴击，×1.5）
//    → × 抗性减免（物理抗性/魔法抗性，比如 10% 抗性 = ×0.9）
//    → 最终伤害（至少 1 点）
// ================================================================
void DamageCalculator::ApplyDamageModifiers(DamageInfo& info, double& baseDamage,
    double skillRatio, int attackerLevel, int defenderLevel,
    ElementType elementType, int agility, double resistanceValue)
{
    // 第 1 步：应用技能倍率
    baseDamage *= skillRatio;

    // 第 2 步：随机浮动（0.9~1.1）
    baseDamage *= GetRandomDamageMultiplier();

    // 第 3 步：等级差加成
    baseDamage *= GetLevelDifferenceBonus(attackerLevel, defenderLevel);

    // 记录基础伤害（经过前三步后的值）
    info.baseDamage = baseDamage;

    // 第 4 步：属性克制
    double elementBonus = GetElementBonus(elementType, ElementType::Neutral);
    info.elementBonus = elementBonus;
    baseDamage *= elementBonus;

    // 第 5 步：暴击判定
    info.isCritical = TriggerCritical(agility);
    if (info.isCritical)
    {
        baseDamage *= 1.5;      // 暴击 = 1.5 倍伤害！
    }

    // 第 6 步：抗性减免
    // 比如抗性 0.1（10%），减免后 = 1.0 - 0.1 = 0.9（保留 90% 伤害）
    double resistanceReduction = 1.0 - resistanceValue;
    info.resistanceReduction = resistanceReduction;
    baseDamage *= resistanceReduction;

    // 第 7 步：最终伤害至少为 1
    // 防止伤害太低变成 0，保证每次攻击都有意义
    info.finalDamage = std::max(1.0, baseDamage);
}


// ================================================================
//  物理伤害计算
//
//  公式：基础伤害 = max(攻击力 × 2 - 防御力 × 1.5, 攻击力 × 0.2)
//
//  解释：
//    - 攻击力 × 2：你的攻击力翻倍
//    - 防御力 × 1.5：对方防御力的 1.5 倍用来减免
//    - max(..., 攻击力 × 0.2)：保证至少有 20% 攻击力的伤害
//      防止防御太高导致完全打不动的情况
//
//  例子：攻击力 30，防御力 10
//    基础伤害 = max(30×2 - 10×1.5, 30×0.2)
//             = max(60 - 15, 6)
//             = max(45, 6)
//             = 45
//===============================================================
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

    // 计算基础伤害
    double baseDamage = std::max(
        (attackerAttack * 2.0 - defenderDefense * 1.5),   // 正常伤害公式
        attackerAttack * 0.2                               // 保底伤害（至少 20% 攻击力）
    );

    // 经过流水线处理，得到最终伤害
    ApplyDamageModifiers(info, baseDamage, skillRatio,
        attackerLevel, defenderLevel, elementType, agility,
        resistance.physicalResistance);     // 使用物理抗性

    return info;
}


// ================================================================
//  魔法伤害计算
//
//  公式：
//    法术减伤比 = 防御方法防 ÷ (防御方法防 + 200)
//    伤害系数 = 1.6 - 法术减伤比
//    基础伤害 = max(法术攻击力 × 伤害系数, 法术攻击力 × 0.2)
//
//  解释：
//    - 法防越高，减伤越多
//    - 比如法防 200：减伤比 = 200÷400 = 0.5，伤害系数 = 1.6-0.5 = 1.1
//    - 比如法防 0：  减伤比 = 0÷200 = 0，伤害系数 = 1.6
//    - 保底 20% 法术攻击力
// ================================================================
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

    // 计算防御方的法术减伤比
    // 法防 0   → 减伤 0%（完全没减免）
    // 法防 100 → 减伤 33%（减免三分之一）
    // 法防 200 → 减伤 50%（减免一半）
    double magicDamageReduction = defenderSpellDefense / (defenderSpellDefense + 200.0);

    // 伤害系数 = 1.6 - 减伤比
    double damageCoefficient = 1.6 - magicDamageReduction;

    // 基础伤害 = 法术攻击力 × 伤害系数（至少保留 20%）
    double baseDamage = std::max(
        attackerSpellPower * damageCoefficient,
        attackerSpellPower * 0.2     // 保底伤害
    );

    // 经过流水线处理，得到最终伤害
    ApplyDamageModifiers(info, baseDamage, skillRatio,
        attackerLevel, defenderLevel, elementType, agility,
        resistance.magicalResistance);      // 使用魔法抗性

    return info;
}


// ================================================================
//  真实伤害计算
//
//  公式：真实伤害 = 固定值 + 攻击力 × 0.5
//
//  特点：无视防御力、无视抗性！
//  只受随机浮动和等级差影响（不受属性克制、暴击、抗性影响）
//
//  例子：固定值 30，攻击力 35
//    真实伤害 = 30 + 35×0.5 = 30 + 17.5 = 47.5
// ================================================================
DamageInfo DamageCalculator::CalculateTrueDamage(
    double baseTrueDamage,
    double attackerAttack,
    int attackerLevel,
    int defenderLevel)
{
    DamageInfo info;

    // 真实伤害 = 固定值 + 攻击力的一半
    double trueDamage = baseTrueDamage + attackerAttack * 0.5;

    // 随机浮动（0.9~1.1）
    trueDamage *= GetRandomDamageMultiplier();

    // 等级差加成
    trueDamage *= GetLevelDifferenceBonus(attackerLevel, defenderLevel);

    // 记录伤害信息
    info.baseDamage = trueDamage;
    info.finalDamage = std::max(1.0, trueDamage);   // 至少 1 点伤害
    info.isCritical = false;        // 真实伤害不会暴击
    info.elementBonus = 1.0;        // 不受属性克制影响
    info.resistanceReduction = 1.0; // 不受抗性影响（100% 穿透）

    return info;
}
