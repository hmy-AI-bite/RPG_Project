#pragma once
#include "Character.h"
#include <string>

// 怪物基类
class Enemy : public Character
{
protected:
    int experienceReward;   // 击杀奖励经验值
    int goldReward;         // 击杀奖励金币
    std::string dropItem;   // 掉落物品
    double dropRate;        // 掉落率 (0.0 ~ 1.0)

public:
    Enemy(const std::string& name, int maxHp, int attackPower, int defend, int level);
    virtual ~Enemy();

    // getter
    int GetExperienceReward() const { return experienceReward; }
    int GetGoldReward() const { return goldReward; }
    std::string GetDropItem() const { return dropItem; }
    double GetDropRate() const { return dropRate; }

    // setter
    void SetExperienceReward(int exp) { experienceReward = exp; }
    void SetGoldReward(int gold) { goldReward = gold; }
    void SetDropItem(const std::string& item, double rate) { dropItem = item; dropRate = rate; }

    // AI行为
    virtual DamageInfo PerformAction(Character* target);
    virtual void DisplayInfo() const override;
};

// 史莱姆 - 基础怪物
class Slime : public Enemy
{
private:
    int splitHealth;  // 分裂时的血量阈值

public:
    Slime(int level = 1);
    virtual ~Slime();

    // 史莱姆特有行为
    virtual DamageInfo PerformAction(Character* target) override;
    void SplitAttack();  // 分裂攻击
    virtual void DisplayInfo() const override;
};
