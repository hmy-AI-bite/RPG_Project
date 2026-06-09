#pragma once
#include "DamageSystem.h"
#include <string>

class Character;

// 技能类
class Skill
{
protected:
    std::string skillName;
    DamageType damageType;
    double skillRatio;      // 技能倍率
    int mpCost;             // 蓝量消耗
    ElementType elementType;    // 技能属性

public:
    Skill(const std::string& name, DamageType type, double ratio, int mpCost, ElementType element = ElementType::Neutral);
    virtual ~Skill();

    // getter
    std::string GetName() const { return skillName; }
    DamageType GetDamageType() const { return damageType; }
    double GetSkillRatio() const { return skillRatio; }
    int GetMpCost() const { return mpCost; }
    ElementType GetElementType() const { return elementType; }

    // 执行技能（返回伤害信息）
    virtual DamageInfo ExecuteSkill(Character* attacker, Character* defender);

protected:
    DamageCalculator calculator;
};

// 物理技能
class PhysicalSkill : public Skill
{
public:
    PhysicalSkill(const std::string& name, double ratio, int mpCost, ElementType element = ElementType::Neutral);
    virtual DamageInfo ExecuteSkill(Character* attacker, Character* defender) override;
};

// 魔法技能
class MagicalSkill : public Skill
{
private:
    int spellPower;  // 魔法技能的基础法术攻击力

public:
    MagicalSkill(const std::string& name, double ratio, int mpCost, int basePower, ElementType element = ElementType::Neutral);
    virtual DamageInfo ExecuteSkill(Character* attacker, Character* defender) override;
};

// 真实伤害技能（刺客技能）
class TrueSkill : public Skill
{
private:
    double baseTrueDamage;  // 固定伤害值

public:
    TrueSkill(const std::string& name, double baseDamage, int mpCost);
    virtual DamageInfo ExecuteSkill(Character* attacker, Character* defender) override;
};
