#pragma once
#include "Character.h"
#include <string>
#include <memory>
#include <vector>

// ================================================================
//  Enemy 类 - 怪物基类（继承自 Character）
//  比普通角色多了：击杀奖励（经验、金币）、掉落物品
//  怪物由 AI 控制，不需要玩家手动操作
// ================================================================
class Enemy : public Character
{
protected:
    int experienceReward;       // 击杀这只怪物能获得多少经验值
    int goldReward;             // 击杀这只怪物能获得多少金币
    std::string dropItem;       // 掉落物品的名字（比如"史莱姆胶体"）
    double dropRate;            // 掉落概率（0.0 ~ 1.0，比如 0.3 就是 30% 概率掉落）

public:
    // 构造函数：创建一个怪物
    Enemy(const std::string& name, int maxHp, int attackPower, int defend, int level);
    virtual ~Enemy();

    // ========== Getter ==========
    int GetExperienceReward() const { return experienceReward; }
    int GetGoldReward() const { return goldReward; }
    std::string GetDropItem() const { return dropItem; }
    double GetDropRate() const { return dropRate; }

    // ========== Setter ==========
    void SetExperienceReward(int exp) { experienceReward = exp; }
    void SetGoldReward(int gold) { goldReward = gold; }
    void SetDropItem(const std::string& item, double rate) { dropItem = item; dropRate = rate; }

    // ========== AI 行为 ==========

    // 怪物行动：对目标执行 AI 决定的动作（攻击、分裂等）
    // 返回值：本次行动新生成的怪物列表（比如史莱姆分裂出的新史莱姆）
    //         如果没生成新怪物，返回空列表 {}
    virtual std::vector<std::shared_ptr<Enemy>> PerformAction(Character* target);

    // 显示怪物信息（包括经验、金币、掉落等额外信息）
    virtual void DisplayInfo() const override;
};

// ================================================================
//  Slime 类 - 史莱姆（继承自 Enemy）
//  最基础的小怪物，特点是：
//  1. 血量低于一半时会分裂！一个变两个，非常讨厌
//  2. 分裂出的子史莱姆继承父史莱姆的当前属性
//  3. 分裂后父史莱姆会回复 1/4 最大血量
//  4. 血量高于一半时用"弹跳攻击"（1.2 倍伤害）
// ================================================================
class Slime : public Enemy
{
private:
    int splitHealth;    // 分裂阈值：当前血量低于这个值就会触发分裂
                        // 默认 = 最大血量的一半

public:
    // 构造函数：level 是等级，等级越高史莱姆越强
    Slime(int level = 1);
    virtual ~Slime();

    // 史莱姆的 AI 行为：
    //   血量 < splitHealth → 分裂（生成一个新史莱姆，自己回血）
    //   血量 >= splitHealth → 弹跳攻击（1.2 倍伤害）
    virtual std::vector<std::shared_ptr<Enemy>> PerformAction(Character* target) override;

    // 显示史莱姆详细信息
    virtual void DisplayInfo() const override;
};
