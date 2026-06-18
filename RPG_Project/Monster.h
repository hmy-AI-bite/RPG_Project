#pragma once
#include "Character.h"
#include "Item.h"
#include <memory>
#include <vector>
#include <string>

// ================================================================
//  Enemy 基类 - 所有怪物的父类
// ================================================================
class Enemy : public Character
{
protected:
    int experienceReward;
    int goldReward;
    std::vector<std::shared_ptr<Skill>> skills;  // 怪物技能列表（新增）

public:
    Enemy(const std::string& name, int maxHp, int attackPower, int defend, int level);
    virtual ~Enemy() = default;

    // 奖励
    int GetExperienceReward() const { return experienceReward; }
    int GetGoldReward() const { return goldReward; }

    // 掉落物（返回动态分配的道具，调用者负责加入背包）
    virtual std::vector<Item*> GenerateDrops() const { return {}; }

    // AI 行动（每个怪物必须实现）
    virtual std::vector<std::shared_ptr<Enemy>> PerformAction(Character* target) = 0;

    // 添加技能
    void AddSkill(std::shared_ptr<Skill> skill) { skills.push_back(skill); }

    // 显示信息
    virtual void DisplayInfo() const override;
};

// ================================================================
//  Slime 史莱姆（原有，增加掉落生成）
// ================================================================
class Slime : public Enemy
{
private:
    int splitHealth;
public:
    Slime(int level = 1);
    std::vector<std::shared_ptr<Enemy>> PerformAction(Character* target) override;
    std::vector<Item*> GenerateDrops() const override;
    void DisplayInfo() const override;
};

// ================================================================
//  Goblin 哥布林
// ================================================================
class Goblin : public Enemy
{
public:
    Goblin(int level = 1);
    std::vector<std::shared_ptr<Enemy>> PerformAction(Character* target) override;
    std::vector<Item*> GenerateDrops() const override;
    void DisplayInfo() const override;
};

// ================================================================
//  Skeleton 骷髅兵
// ================================================================
class Skeleton : public Enemy
{
public:
    Skeleton(int level = 1);
    std::vector<std::shared_ptr<Enemy>> PerformAction(Character* target) override;
    std::vector<Item*> GenerateDrops() const override;
    void DisplayInfo() const override;
};

// ================================================================
//  DarkMage 暗影法师
// ================================================================
class DarkMage : public Enemy
{
public:
    DarkMage(int level = 1);
    std::vector<std::shared_ptr<Enemy>> PerformAction(Character* target) override;
    std::vector<Item*> GenerateDrops() const override;
    void DisplayInfo() const override;
};

// ================================================================
//  Boss 巨龙
// ================================================================
class Boss : public Enemy
{
public:
    Boss(int level = 5);
    std::vector<std::shared_ptr<Enemy>> PerformAction(Character* target) override;
    std::vector<Item*> GenerateDrops() const override;
    void DisplayInfo() const override;
};
