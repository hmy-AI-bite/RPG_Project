#include "Character.h"
#include "Inventory.h"
#include <iostream>

// ===== Character 类实现 =====
Character::Character(const std::string& name, int maxHp, int attackPower, int defend, int level)
    : name(name), maxHp(maxHp), hp(maxHp), attackPower(attackPower), defend(defend), level(level),
      agility(10), speed(100), spellDefense(5), elementType(ElementType::Neutral), currentGauge(0)
{
}

Character::~Character()
{
}

void Character::TakeDamage(int damage)
{
    if (damage <= 0) return;
    hp -= damage;
    if (hp < 0) hp = 0;
    std::cout << name << " 受到了 " << damage << " 点伤害！生命值: " << hp << "/" << maxHp << std::endl;
}

void Character::TakeDamage(const DamageInfo& damageInfo)
{
    int finalDamage = static_cast<int>(damageInfo.finalDamage);
    hp -= finalDamage;
    if (hp < 0) hp = 0;

    std::string criticalText = damageInfo.isCritical ? "【暴击】" : "";
    std::cout << name << " 受到了 " << finalDamage << " 点伤害 " << criticalText << "！生命值: " << hp << "/" << maxHp << std::endl;
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

void Character::UpdateGauge(int deltaTime)
{
    // 根据速度值每帧增加行动条
    // deltaTime是毫秒，转换为速度增长
    currentGauge += (speed * deltaTime) / 100;
    if (currentGauge > MAX_GAUGE * 2)  // 防止溢出过多
    {
        currentGauge = MAX_GAUGE * 2;
    }
}

bool Character::IsReadyToAct() const
{
    return currentGauge >= MAX_GAUGE;
}

void Character::ResetGauge()
{
    currentGauge -= MAX_GAUGE;
    if (currentGauge < 0)
    {
        currentGauge = 0;
    }
}

int Character::GetGaugePercent() const
{
    return (currentGauge * 100) / MAX_GAUGE;
}

void Character::DisplayInfo() const
{
    std::cout << "[" << name << "] 生命值: " << hp << "/" << maxHp << ", 攻击: " << attackPower 
              << ", 防御: " << defend << ", 法防: " << spellDefense << ", 灵巧: " << agility 
              << ", 速度: " << speed << ", 等级: " << level << std::endl;
}

// ===== Player 类实现 =====
Player::Player(const std::string& name, int maxHp, int attackPower, int defend, int level)
    : Character(name, maxHp, attackPower, defend, level), experience(0), maxMp(50), mp(maxMp), gold(100)
{
    // 战士属性设定：均衡型，能抗能打
    this->maxHp = 120;  // 生命值中等
    this->hp = this->maxHp;
    this->defend = 10;  // 防御力较高，能抗
    this->attackPower = 20;  // 攻击力中等
    this->agility = 60;  // 灵巧60 -> 暴击率6%
    this->spellDefense = 8;
    this->speed = 100;  // 速度中等
    
    // 蓝量中等
    maxMp = 50;
    mp = maxMp;
    
    // 设置属性为中立
    elementType = ElementType::Neutral;
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

DamageInfo Player::NormalAttack(Character* defender)
{
    if (!defender)
    {
        DamageInfo empty;
        empty.finalDamage = 0;
        return empty;
    }

    DamageCalculator calculator;
    
    // 普通攻击：倍率1.0，无特殊属性
    DamageInfo damageInfo = calculator.CalculatePhysicalDamage(
        attackPower,
        defender->GetDefend(),
        level,
        defender->GetLevel(),
        1.0,  // 普通攻击倍率为1.0
        elementType,
        agility,
        defender->GetResistance()
    );

    std::cout << name << " 使出普通攻击！";
    defender->TakeDamage(damageInfo);
    
    return damageInfo;
}

void Player::DisplayInfo() const
{
    std::cout << "[战士] " << name << " | 生命值: " << hp << "/" << maxHp << ", 蓝量: " << mp << "/" << maxMp 
              << ", 攻击: " << attackPower << ", 防御: " << defend << ", 灵巧: " << agility
              << ", 速度: " << speed << ", 等级: " << level << ", 经验: " << experience << std::endl;
}

// ===== Braver 类实现 =====
Braver::Braver(const std::string& name, int maxHp, int attackPower, int defend, int level)
    : Player(name, maxHp, attackPower, defend, level), rage(0)
{
    // 勇者属性设定：高攻击力、低防御、高暴击率、真实伤害
    this->maxHp = 140;  // 生命值较高
    this->hp = this->maxHp;
    this->defend = 6;  // 防御力较低，追求输出
    this->attackPower = 35;  // 攻击力很高
    this->agility = 120;  // 灵巧120 -> 暴击率12%
    this->spellDefense = 5;
    this->speed = 110;  // 速度较快
    
    // 蓝量设置
    maxMp = 60;
    mp = maxMp;
    
    // 设置属性为水属性
    elementType = ElementType::Water;
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

void Braver::ExecuteTrueAttack()
{
    int trueDamage = static_cast<int>(30 + attackPower * 0.5);
    std::cout << name << " 使出刺客之刃！造成真实伤害: " << trueDamage << " (无视防御!)" << std::endl;
}

void Braver::DisplayInfo() const
{
    std::cout << "[勇敢者] " << name << " | 生命值: " << hp << "/" << maxHp << ", 蓝量: " << mp << "/" << maxMp 
              << ", 攻击: " << attackPower << ", 防御: " << defend << ", 灵巧: " << agility 
              << ", 速度: " << speed << ", 等级: " << level << ", 经验: " << experience << ", 怒气: " << rage << "/100" << std::endl;
}

// ===== Mage 类实现 =====
Mage::Mage(const std::string& name, int maxHp, int attackPower, int defend, int level, int spellPower)
    : Player(name, maxHp, attackPower, defend, level), spellPower(spellPower)
{
    // 法师属性设定：低生命值、低防御、高攻击力、超高暴击率
    this->maxHp = 80;  // 生命值较低
    this->hp = this->maxHp;
    this->defend = 3;  // 防御力很低
    this->attackPower = 28;  // 攻击力较高
    this->agility = 150;  // 灵巧150 -> 暴击率15%
    this->spellDefense = 8;  // 法术防御中等
    this->speed = 120;  // 速度较快
    
    // 法师的蓝量更多
    maxMp = 100;
    mp = maxMp;
    
    // 设置属性为火属性
    elementType = ElementType::Fire;
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
    std::cout << name << " 施放了火焰术！" << std::endl;
}

void Mage::DisplayInfo() const
{
    std::cout << "[法师] " << name << " | 生命值: " << hp << "/" << maxHp << ", 蓝量: " << mp << "/" << maxMp 
              << ", 攻击: " << attackPower << ", 法力: " << spellPower << ", 防御: " << defend 
              << ", 灵巧: " << agility << ", 速度: " << speed << ", 等级: " << level << ", 经验: " << experience << std::endl;
}