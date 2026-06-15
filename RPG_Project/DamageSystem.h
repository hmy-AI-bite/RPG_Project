#pragma once
#include <random>
#include <cmath>
#include <string>

// ================================================================
//  元素/属性类型
//  就像"石头剪刀布"一样，属性之间有克制关系：
//    火 → 风 → 地 → 水 → 火 （循环克制）
//    用克制属性打敌人 = 1.5 倍伤害！
//    用被克制属性打敌人 = 0.75 倍伤害...
// ================================================================
enum class ElementType
{
    Fire,       // 火 - 克制风，被水克制
    Wind,       // 风 - 克制地，被火克制
    Earth,      // 地 - 克制水，被风克制
    Water,      // 水 - 克制火，被地克制
    Neutral     // 中立 - 不克制任何人，也不被任何人克制
};

// ================================================================
//  伤害类型
// ================================================================
enum class DamageType
{
    Physical,   // 物理伤害 - 会被防御力和物理抗性减免
    Magical,    // 魔法伤害 - 会被法术防御和魔法抗性减免
    True        // 真实伤害 - 无视一切防御和抗性！非常厉害
};

// ================================================================
//  抗性结构
//  抗性 = 减免伤害的百分比
//  比如 physicalResistance = 0.1 就是减免 10% 的物理伤害
// ================================================================
struct ResistanceProfile
{
    double physicalResistance = 0.0;    // 物理抗性（0.0 ~ 1.0）
    double magicalResistance = 0.0;     // 魔法抗性（0.0 ~ 1.0）
};

// ================================================================
//  伤害信息结构
//  一次攻击产生的完整伤害数据都装在这里
// ================================================================
struct DamageInfo
{
    double baseDamage;              // 基础伤害（未经任何加成/减免的原始伤害）
    double finalDamage;             // 最终伤害（经过所有加成和减免后，实际扣的血）
    bool isCritical;                // 是否暴击（暴击 = 1.5 倍伤害！）
    double elementBonus;            // 属性克制倍数（克制 = 1.5x，被克 = 0.75x）
    double resistanceReduction;     // 抗性减免倍数（比如 0.9 就是减免了 10%）
    std::string description;        // 伤害描述文字
};

// ================================================================
//  伤害计算器
//  这是整个游戏伤害系统的"大脑"！
//  负责计算：物理伤害、魔法伤害、真实伤害
//
//  伤害计算流程（按顺序）：
//    1. 计算基础伤害（攻击力 × 2 - 防御力 × 1.5）
//    2. × 技能倍率（普通攻击 = 1.0，技能可能是 1.5、1.8 等）
//    3. × 随机浮动（0.9 ~ 1.1，让伤害不是固定值）
//    4. × 等级差加成（等级高的打等级低的更疼）
//    5. × 属性克制（克制 = 1.5x，被克 = 0.75x）
//    6. × 暴击判定（暴击 = 1.5x，暴击率 = 灵巧 ÷ 1000）
//    7. × 抗性减免（物理抗性/魔法抗性）
//    8. 最终伤害至少为 1（保证每次攻击都有意义）
// ================================================================
class DamageCalculator
{
public:
    DamageCalculator();

    // ========== 物理伤害计算 ==========
    // 用于：普通攻击、物理技能
    // 公式：基础伤害 = max(攻击力 × 2 - 防御力 × 1.5, 攻击力 × 0.2)
    //       保证不会因为防御太高而完全打不动
    DamageInfo CalculatePhysicalDamage(
        double attackerAttack,          // 攻击者的攻击力
        double defenderDefense,         // 防御者的防御力
        int attackerLevel,              // 攻击者等级
        int defenderLevel,              // 防御者等级
        double skillRatio = 1.0,        // 技能倍率（默认 1.0 = 普通攻击）
        ElementType elementType = ElementType::Neutral,  // 攻击属性
        int agility = 0,                // 攻击者灵巧（影响暴击率）
        ResistanceProfile resistance = ResistanceProfile()  // 防御者抗性
    );

    // ========== 魔法伤害计算 ==========
    // 用于：魔法技能
    // 公式：基础伤害 = 法术攻击力 × 伤害系数
    //       伤害系数 = 1.6 - (防御方法防 ÷ (防御方法防 + 200))
    //       法防越高，魔法伤害越低
    DamageInfo CalculateMagicalDamage(
        double attackerSpellPower,      // 攻击者的法术攻击力
        double defenderSpellDefense,    // 防御者的法术防御力
        int attackerLevel,              // 攻击者等级
        int defenderLevel,              // 防御者等级
        double skillRatio = 1.0,        // 技能倍率
        ElementType elementType = ElementType::Neutral,  // 攻击属性
        int agility = 0,                // 攻击者灵巧（影响暴击率）
        ResistanceProfile resistance = ResistanceProfile()  // 防御者抗性
    );

    // ========== 真实伤害计算 ==========
    // 用于：真实伤害技能（如勇者的"刺客之刃"）
    // 特点：无视防御力、无视抗性！
    // 公式：真实伤害 = 固定值 + 攻击力 × 0.5
    DamageInfo CalculateTrueDamage(
        double baseTrueDamage,          // 固定伤害值
        double attackerAttack,          // 攻击者的攻击力
        int attackerLevel,              // 攻击者等级
        int defenderLevel               // 防御者等级
    );

private:
    // 随机数生成器（C++ 的随机工具）
    std::random_device rd;      // 真随机种子
    std::mt19937 gen;           // 随机数生成引擎

    // ========== 内部工具方法 ==========

    // 获取随机伤害浮动倍数（0.9 ~ 1.1）
    // 让每次攻击的伤害都有点不一样，更有趣
    double GetRandomDamageMultiplier();

    // 获取等级差加成
    // 等级每高 1 级，伤害 +2.5%
    double GetLevelDifferenceBonus(int attackerLevel, int defenderLevel);

    // 获取属性克制倍数
    // 克制 = 1.5x，被克制 = 0.75x，中立 = 1.0x
    double GetElementBonus(ElementType attackerElement, ElementType defenderElement);

    // 计算暴击率
    // 暴击率 = 灵巧 ÷ 1000，最高 30%
    double GetCriticalChance(int agility);

    // 判断这次攻击是否暴击（掷骰子！）
    bool TriggerCritical(int agility);

    // 生成伤害描述文字
    std::string GetDamageDescription(const DamageInfo& info,
        const std::string& attackerName, const std::string& defenderName);

    // 公共后处理：把技能倍率、随机浮动、等级差、属性克制、暴击、抗性
    // 全部应用到基础伤害上，得到最终伤害
    void ApplyDamageModifiers(DamageInfo& info, double& baseDamage,
        double skillRatio, int attackerLevel, int defenderLevel,
        ElementType elementType, int agility, double resistanceValue);
};
