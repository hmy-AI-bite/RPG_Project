#include "Monster.h"
#include <iostream>
#include <random>
#include <cmath>

// ===== Enemy 基类实现 =====
Enemy::Enemy(const std::string& name, int maxHp, int attackPower, int defend, int level)
    : Character(name, maxHp, attackPower, defend, level),
      experienceReward(0), goldReward(0), dropItem(""), dropRate(0.0)
{
}

Enemy::~Enemy()
{
}

DamageInfo Enemy::PerformAction(Character* target)
{
    if (!target)
    {
        DamageInfo empty;
        empty.finalDamage = 0;
        return empty;
    }

    // 基础怪物的默认行为：普通攻击
    DamageCalculator calculator;
    DamageInfo damageInfo = calculator.CalculatePhysicalDamage(
        attackPower,
        target->GetDefend(),
        level,
        target->GetLevel(),
        1.0,
        elementType,
        agility,
        target->GetResistance()
    );

    std::cout << name << " 发动了普通攻击！";
    target->TakeDamage(damageInfo);

    return damageInfo;
}

void Enemy::DisplayInfo() const
{
    std::cout << "[怪物] " << name << " | 生命值: " << hp << "/" << maxHp << ", 攻击: " << attackPower 
              << ", 防御: " << defend << ", 速度: " << speed << ", 等级: " << level 
              << " | 奖励经验: " << experienceReward << ", 金币: " << goldReward << std::endl;
}

// ===== Slime 史莱姆实现 =====
Slime::Slime(int level)
    : Enemy("史莱姆", 50, 8, 2, level), splitHealth(25)
{
    // 史莱姆属性配置
    this->maxHp = 30 + level * 5;      // 生命值随等级增长
    this->hp = this->maxHp;
    this->attackPower = 5 + level * 2; // 攻击力随等级增长
    this->defend = 1 + level;           // 防御力随等级增长
    this->agility = 20 + level * 2;     // 灵巧
    this->speed = 80;                  // 速度较慢
    this->spellDefense = 2;
    this->elementType = ElementType::Water;  // 水属性

    // 分裂血量阈值
    splitHealth = this->maxHp / 2;

    // 奖励设置
    this->experienceReward = 20 + level * 5;  // 经验奖励
    this->goldReward = 10 + level * 3;        // 金币奖励
    this->SetDropItem("史莱姆胶体", 0.3);     // 30%掉落概率
}

Slime::~Slime()
{
}

DamageInfo Slime::PerformAction(Character* target)
{
    if (!target)
    {
        DamageInfo empty;
        empty.finalDamage = 0;
        return empty;
    }

    DamageCalculator calculator;

    // 史莱姆AI逻辑：
    // 血量较低时(<50%)使用分裂攻击，否则普通攻击
    if (hp < splitHealth)
    {
        std::cout << name << " 使出分裂攻击！";
        SplitAttack();
        return DamageInfo();
    }
    else
    {
        DamageInfo damageInfo = calculator.CalculatePhysicalDamage(
            attackPower,
            target->GetDefend(),
            level,
            target->GetLevel(),
            1.2,  // 史莱姆攻击倍率1.2
            elementType,
            agility,
            target->GetResistance()
        );

        std::cout << name << " 弹跳攻击！";
        target->TakeDamage(damageInfo);

        return damageInfo;
    }
}

void Slime::SplitAttack()
{
    std::cout << "会发出粘液喷射！" << std::endl;
    // 分裂攻击会溅射伤害，这里先作为文字输出
    // 实际游戏可能会对所有敌人或玩家造成范围伤害
}

void Slime::DisplayInfo() const
{
    std::cout << "[史莱姆] " << name << " Lv." << level << " | 生命值: " << hp << "/" << maxHp 
              << ", 攻击: " << attackPower << ", 防御: " << defend << ", 速度: " << speed
              << " | 经验奖励: " << experienceReward << ", 金币: " << goldReward 
              << " | 掉落: " << dropItem << " (" << (dropRate * 100) << "%)" << std::endl;
}
