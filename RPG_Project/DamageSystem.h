#pragma once
#include <random>
#include <cmath>

// 属性类型枚举
enum class ElementType
{
    Fire,       // 火
    Wind,       // 风
    Earth,      // 地
    Water,      // 水
    Neutral     // 中立
};

// 伤害类型枚举
enum class DamageType
{
    Physical,   // 物理伤害
    Magical,    // 魔法伤害
    True        // 真实伤害
};

// 抗性结构
struct ResistanceProfile
{
    double physicalResistance = 0.0;  // 物理抗性 0-1
    double magicalResistance = 0.0;   // 魔法抗性 0-1
};

// 伤害信息结构
struct DamageInfo
{
    double baseDamage;      // 基础伤害
    double finalDamage;     // 最终伤害
    bool isCritical;        // 是否暴击
    double elementBonus;    // 属性克制倍数
    double resistanceReduction;  // 抗性减伤倍数
    std::string description;     // 伤害描述
};

class DamageCalculator
{
public:
    DamageCalculator();

    // 物理伤害计算
    DamageInfo CalculatePhysicalDamage(
        double attackerAttack,      // 攻击者攻击力
        double defenderDefense,     // 防御者防御力
        int attackerLevel,          // 攻击者等级
        int defenderLevel,          // 防御者等级
        double skillRatio = 1.0,    // 技能倍率
        ElementType elementType = ElementType::Neutral,  // 属性
        int agility = 0,            // 灵巧（用于暴击率）
        ResistanceProfile resistance = ResistanceProfile()  // 防御者抗性
    );

    // 魔法伤害计算
    DamageInfo CalculateMagicalDamage(
        double attackerSpellPower,      // 攻击者法术攻击力
        double defenderSpellDefense,    // 防御者法术防御力
        int attackerLevel,              // 攻击者等级
        int defenderLevel,              // 防御者等级
        double skillRatio = 1.0,        // 技能倍率
        ElementType elementType = ElementType::Neutral,  // 属性
        int agility = 0,                // 灵巧（用于暴击率）
        ResistanceProfile resistance = ResistanceProfile()  // 防御者抗性
    );

    // 真实伤害计算
    DamageInfo CalculateTrueDamage(
        double baseTrueDamage,      // 固定值
        double attackerAttack,      // 攻击者攻击力
        int attackerLevel,          // 攻击者等级
        int defenderLevel           // 防御者等级
    );

private:
    // 随机数生成器
    std::random_device rd;
    std::mt19937 gen;

    // 工具方法
    double GetRandomDamageMultiplier();  // 获取 0.9-1.1 的随机倍数
    double GetLevelDifferenceBonus(int attackerLevel, int defenderLevel);  // 获取等级差加成
    double GetElementBonus(ElementType attackerElement, ElementType defenderElement);  // 获取属性克制倍数
    double GetCriticalChance(int agility);  // 获取暴击率
    bool TriggerCritical(int agility);  // 判断是否触发暴击
    std::string GetDamageDescription(const DamageInfo& info, const std::string& attackerName, const std::string& defenderName);
};
