#pragma once
#include <string>

class Character
{
protected:
    std::string name;
    int hp;
    int level;
    int defend;
    int maxHp;
    int attackPower;

public:
    Character(const std::string& name, int maxHp, int attackPower, int defend, int level);
    virtual ~Character();

    // 基本操作
    void TakeDamage(int damage);
    void Heal(int amount);
    bool IsAlive() const;

    // 显示信息
    virtual void DisplayInfo() const;

    // getter
    std::string GetName() const { return name; }
    int GetHp() const { return hp; }
    int GetMaxHp() const { return maxHp; }
    int GetAttackPower() const { return attackPower; }
    int GetLevel() const { return level; }
    int GetDefend() const { return defend; }
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
    virtual void DisplayInfo() const override;
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