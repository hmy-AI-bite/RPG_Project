#pragma once
#pragma once
#include <string>

class Character
{
protected:
    std::string name;
    int hp;
    int maxHp;
    int attackPower;

public:
    Character(const std::string& name, int maxHp, int attackPower);
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
};