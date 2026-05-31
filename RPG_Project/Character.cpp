#include "Character.h"
#include <iostream>

// ===== Character 类实现 =====
Character::Character(const std::string& name, int maxHp, int attackPower, int defend, int level)
    : name(name), maxHp(maxHp), hp(maxHp), attackPower(attackPower), defend(defend), level(level)
{
}

Character::~Character()
{
}

void Character::TakeDamage(int damage)
{
    if (damage <= 0) return;
    hp -= damage * (damage / (damage + defend));
    if (hp < 0) hp = 0;
    std::cout << name << " 受到了 " << damage << " 点伤害！生命值: " << hp << "/" << maxHp << std::endl;
}

void Character::Heal(int amount)
{
    if (amount <= 0) return;
    hp += amount;
    if (hp > maxHp) hp = maxHp;
    std::cout << name << " 恢复了 " << amount << " 点生命值。生命值: " << hp << "/" << maxHp << std::endl;
}

bool Character::IsAlive() const
{
    return hp > 0;
}

void Character::DisplayInfo() const
{
    std::cout << "[" << name << "] 生命值: " << hp << "/" << maxHp << ", 攻击: " << attackPower << ", 防御: " << defend << ", 等级: " << level << std::endl;
}

// ===== Player 类实现 =====
Player::Player(const std::string& name, int maxHp, int attackPower, int defend, int level)
    : Character(name, maxHp, attackPower, defend, level), experience(0), maxMp(50), mp(maxMp)
{
}

Player::~Player()
{
}

void Player::GainExperience(int exp)
{
    if (exp <= 0) return;
    experience += exp;
    std::cout << name << " 获得了 " << exp << " 点经验值！总经验: " << experience << std::endl;
    
    // 每100经验值升一级
    if (experience >= 100)
    {
        LevelUp();
    }
}

void Player::LevelUp()
{
    if (experience >= 100)
    {
        experience -= 100;
        level++;
        maxHp += 10;
        hp = maxHp;
        maxMp += 10;
        mp = maxMp;
        attackPower += 5;
        defend += 2;
        std::cout << name << " 升级到 " << level << " 级！属性已增强！" << std::endl;
    }
}

void Player::ConsumeMp(int amount)
{
    if (amount <= 0) return;
    mp -= amount;
    if (mp < 0) mp = 0;
    std::cout << name << " 消耗了 " << amount << " 点蓝量。当前蓝量: " << mp << "/" << maxMp << std::endl;
}

void Player::RestoreMp(int amount)
{
    if (amount <= 0) return;
    mp += amount;
    if (mp > maxMp) mp = maxMp;
    std::cout << name << " 恢复了 " << amount << " 点蓝量。当前蓝量: " << mp << "/" << maxMp << std::endl;
}

void Player::DisplayInfo() const
{
    std::cout << "[玩家] " << name << " | 生命值: " << hp << "/" << maxHp << ", 蓝量: " << mp << "/" << maxMp 
              << ", 攻击: " << attackPower << ", 防御: " << defend << ", 等级: " << level << ", 经验: " << experience << std::endl;
}

// ===== Braver 类实现 =====
Braver::Braver(const std::string& name, int maxHp, int attackPower, int defend, int level)
    : Player(name, maxHp, attackPower, defend, level), rage(0)
{
}

Braver::~Braver()
{
}

void Braver::RageAttack()
{
    if (rage < 50)
    {
        std::cout << name << " 怒气不足！当前怒气: " << rage << "/100" << std::endl;
        return;
    }
    
    int damage = attackPower * 2 + rage / 10;
    std::cout << name << " 使出怒气斩击！伤害: " << damage << std::endl;
    rage = 0;
}

void Braver::IncreaseRage(int amount)
{
    if (amount <= 0) return;
    rage += amount;
    if (rage > 100) rage = 100;
    std::cout << name << " 的怒气增加到 " << rage << "/100" << std::endl;
}

void Braver::DisplayInfo() const
{
    std::cout << "[勇敢者] " << name << " | 生命值: " << hp << "/" << maxHp << ", 蓝量: " << mp << "/" << maxMp 
              << ", 攻击: " << attackPower << ", 防御: " << defend << ", 等级: " << level << ", 经验: " << experience 
              << ", 怒气: " << rage << "/100" << std::endl;
}

// ===== Mage 类实现 =====
Mage::Mage(const std::string& name, int maxHp, int attackPower, int defend, int level, int spellPower)
    : Player(name, maxHp, attackPower, defend, level), spellPower(spellPower)
{
    // 法师的蓝量更多
    maxMp = 100;
    mp = maxMp;
}

Mage::~Mage()
{
}

void Mage::CastSpell(int mpCost)
{
    if (mpCost <= 0)
    {
        std::cout << "技能消耗无效！" << std::endl;
        return;
    }
    
    if (!HasEnoughMp(mpCost))
    {
        std::cout << name << " 蓝量不足！需要 " << mpCost << " 点蓝量，当前蓝量: " << mp << std::endl;
        return;
    }
    
    ConsumeMp(mpCost);
    std::cout << name << " 施放了一个技能！" << std::endl;
}

void Mage::DisplayInfo() const
{
    std::cout << "[法师] " << name << " | 生命值: " << hp << "/" << maxHp << ", 蓝量: " << mp << "/" << maxMp 
              << ", 攻击: " << attackPower << ", 法力: " << spellPower << ", 防御: " << defend 
              << ", 等级: " << level << ", 经验: " << experience << std::endl;
}