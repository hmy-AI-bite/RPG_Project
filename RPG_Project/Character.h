#pragma once
#include <string>
#include "DamageSystem.h"

// 行动条相关常数
const int MAX_GAUGE = 500;

class Character
{
protected:
    std::string name;
    int hp;
    int level;
    int defend;
    int maxHp;
    int attackPower;
    int agility;        // 灵巧（用于暴击率）
    int speed;          // 速度（决定行动条填充速率）
    int spellDefense;   // 法术防御力
    ElementType elementType;  // 属性类型
    ResistanceProfile resistance;  // 抗性
    
    // 行动条系统
    int currentGauge;   // 当前行动条值 (0 ~ MAX_GAUGE)

public:
    Character(const std::string& name, int maxHp, int attackPower, int defend, int level);
    virtual ~Character();

    // 基本操作
    void TakeDamage(int damage);
    void TakeDamage(const DamageInfo& damageInfo);  // 新的伤害方法
    void Heal(int amount);
    bool IsAlive() const;
    
    // 行动条相关
    void UpdateGauge(int deltaTime);  // 根据速度更新行动条
    bool IsReadyToAct() const;        // 判断是否满条
    void ResetGauge();                // 行动后重置行动条
    int GetGaugePercent() const;      // 获取行动条百分比

    // 显示信息
    virtual void DisplayInfo() const;

    // getter
    std::string GetName() const { return name; }
    int GetHp() const { return hp; }
    int GetMaxHp() const { return maxHp; }
    int GetAttackPower() const { return attackPower; }
    int GetLevel() const { return level; }
    int GetDefend() const { return defend; }
    int GetAgility() const { return agility; }
    int GetSpeed() const { return speed; }
    int GetSpellDefense() const { return spellDefense; }
    int GetCurrentGauge() const { return currentGauge; }
    ElementType GetElementType() const { return elementType; }
    ResistanceProfile GetResistance() const { return resistance; }

    // setter
    void SetAgility(int value) { agility = value; }
    void SetSpeed(int value) { speed = value; }
    void SetSpellDefense(int value) { spellDefense = value; }
    void SetElementType(ElementType type) { elementType = type; }
    void SetPhysicalResistance(double value) { resistance.physicalResistance = value; }
    void SetMagicalResistance(double value) { resistance.magicalResistance = value; }
};

class Player : public Character
{
protected:
    int experience;
    int mp;
    int maxMp;

public:
    Player(const std::string& name, int maxHp, int attackPower, int defend, int level);
    virtual ~Player();
    
    void GainExperience(int exp);
    void LevelUp();
    virtual void DisplayInfo() const override;
    
    // 蓝量相关
    void ConsumeMp(int amount);
    void RestoreMp(int amount);
    bool HasEnoughMp(int amount) const { return mp >= amount; }
    
    // 普通攻击
    DamageInfo NormalAttack(Character* defender);
    
    int GetExperience() const { return experience; }
    int GetMp() const { return mp; }
    int GetMaxMp() const { return maxMp; }
};

class Braver : public Player
{
private:
    int rage;

public:
    Braver(const std::string& name, int maxHp, int attackPower, int defend, int level);
    virtual ~Braver();
    
    void RageAttack();
    void IncreaseRage(int amount);
    void ExecuteTrueAttack();  // 真实伤害技能
    virtual void DisplayInfo() const override;
    
    int GetRage() const { return rage; }
};

class Mage : public Player
{
private:
    int spellPower;

public:
    Mage(const std::string& name, int maxHp, int attackPower, int defend, int level, int spellPower);
    virtual ~Mage();
    
    void CastSpell(int mpCost);
    virtual void DisplayInfo() const override;
    
    int GetSpellPower() const { return spellPower; }
};