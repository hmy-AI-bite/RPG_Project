#include "Monster.h"
#include <iostream>
#include <random>
#include <cmath>
#include <sstream>

// ================================================================
//  Enemy 基类 - 所有怪物的"爸爸"
// ================================================================

// ---------- 构造函数 ----------
// 创建一个怪物，默认没有奖励（经验和金币都是 0，不掉落物品）
Enemy::Enemy(const std::string& name, int maxHp, int attackPower, int defend, int level)
    : Character(name, maxHp, attackPower, defend, level),
      experienceReward(0), goldReward(0), dropItem(""), dropRate(0.0)
{
}

Enemy::~Enemy()
{
}

// ---------- 怪物默认行动：普通攻击 ----------
// 大多数怪物就是简单地普通攻击（倍率 1.0）
// 返回空列表 {} 表示没有生成新怪物
std::vector<std::shared_ptr<Enemy>> Enemy::PerformAction(Character* target)
{
    PerformPhysicalAttack(target, 1.0, " 发动了普通攻击！");
    return {};
}

// ---------- 显示怪物信息 ----------
// 比普通角色多了：经验奖励、金币奖励
void Enemy::DisplayInfo() const
{
    std::cout << "[怪物] " << name
              << " | 生命值: " << hp << "/" << maxHp
              << ", 攻击: " << attackPower
              << ", 防御: " << defend
              << ", 速度: " << speed
              << ", 等级: " << level
              << " | 奖励经验: " << experienceReward
              << ", 金币: " << goldReward << std::endl;
}


// ================================================================
//  Slime 史莱姆 - 最基础的小怪物
//
//  史莱姆的行为规则：
//    如果当前血量 < 最大血量的一半 → 分裂！
//      - 生成一个新的史莱姆，属性完全继承当前史莱姆的当前属性
//      - 分裂后，自己恢复 1/4 最大血量
//    如果当前血量 >= 最大血量的一半 → 弹跳攻击（1.2 倍伤害）
// ================================================================

// ---------- 构造函数 ----------
// 根据等级计算史莱姆的各项属性
// 公式：
//   最大血量 = 30 + 等级 × 5      （比如 3 级 = 45 血）
//   攻击力   = 5  + 等级 × 2      （比如 3 级 = 11 攻击）
//   防御力   = 1  + 等级          （比如 3 级 = 4 防御）
//   灵巧     = 20 + 等级 × 2      （比如 3 级 = 26 灵巧）
//   速度     = 80                 （固定，比较慢）
Slime::Slime(int level)
    : Enemy("史莱姆", 50, 8, 2, level), splitHealth(25)
{
    // 根据等级重新计算属性
    this->maxHp = 30 + level * 5;       // 生命值随等级增长
    this->hp = this->maxHp;             // 初始满血
    this->attackPower = 5 + level * 2;  // 攻击力随等级增长
    this->defend = 1 + level;           // 防御力随等级增长
    this->agility = 20 + level * 2;     // 灵巧随等级增长
    this->speed = 80;                   // 速度固定 80（比较慢）
    this->spellDefense = 2;             // 法防很低
    this->elementType = ElementType::Water;  // 史莱姆是水属性的！

    // 分裂阈值 = 最大血量的一半
    // 比如 3 级史莱姆 maxHp = 45，低于 22 血就会分裂
    splitHealth = this->maxHp / 2;

    // 击杀奖励设置
    this->experienceReward = 20 + level * 5;   // 经验奖励
    this->goldReward = 10 + level * 3;         // 金币奖励
    this->SetDropItem("史莱姆胶体", 0.3);      // 30% 概率掉落"史莱姆胶体"
}

Slime::~Slime()
{
}

// ---------- 史莱姆行动（AI 决策）----------
//
//  决策逻辑（像一个简单的小机器人）：
//    1. 先看自己的血量
//    2. 如果血量 < 分裂阈值（最大血量的一半）→ 分裂！
//       - 创建一个新史莱姆，属性 = 当前所有属性（不是初始属性！）
//       - 自己恢复 1/4 最大血量
//    3. 如果血量 >= 分裂阈值 → 弹跳攻击（1.2 倍伤害）
//
//  返回值：如果分裂了，返回新史莱姆的列表；否则返回空列表
std::vector<std::shared_ptr<Enemy>> Slime::PerformAction(Character* target)
{
    // 判断：血量低于分裂阈值了吗？
    if (hp < splitHealth && hp > 0)
    {
        std::cout << name << " 开始分裂！" << std::endl;

        // ----- 分裂！创建一个新史莱姆 -----
        // 注意：这里先按等级构造（会用等级计算属性），
        // 然后立刻用 setter 把属性全部覆盖为当前值
        auto newSlime = std::make_shared<Slime>(level);

        // 复制当前史莱姆的全部属性到新史莱姆
        // 重要：继承的是"当前属性"，不是"初始最大属性"！
        // 比如当前史莱姆被打到只有 15/45 血，子史莱姆也是 15/45
        newSlime->SetMaxHp(maxHp);                  // 最大血量 = 父体当前最大血量
        newSlime->SetHp(hp);                        // 当前血量 = 父体当前血量
        newSlime->SetAttackPower(attackPower);      // 攻击力 = 父体当前攻击力
        newSlime->SetDefend(defend);                // 防御力 = 父体当前防御力
        newSlime->SetLevel(level);                  // 等级 = 父体当前等级
        newSlime->SetAgility(agility);              // 灵巧 = 父体当前灵巧
        newSlime->SetSpeed(speed);                  // 速度 = 父体当前速度
        newSlime->SetSpellDefense(spellDefense);    // 法防 = 父体当前法防
        newSlime->SetElementType(elementType);      // 属性 = 父体当前属性
        newSlime->SetPhysicalResistance(resistance.physicalResistance);   // 物理抗性
        newSlime->SetMagicalResistance(resistance.magicalResistance);     // 魔法抗性
        newSlime->SetExperienceReward(experienceReward);   // 经验奖励
        newSlime->SetGoldReward(goldReward);               // 金币奖励
        newSlime->SetDropItem(dropItem, dropRate);         // 掉落物品和概率

        // ----- 分裂后，父史莱姆恢复 1/4 最大血量 -----
        // 比如 maxHp = 45，恢复 45/4 = 11 点血
        int healAmount = maxHp / 4;
        hp += healAmount;
        if (hp > maxHp) hp = maxHp;     // 不能超过最大血量

        // 打印分裂结果
        std::cout << "  → 分裂出新的 " << newSlime->GetName()
                  << " (HP: " << newSlime->GetHp() << "/" << newSlime->GetMaxHp() << ")！" << std::endl;
        std::cout << "  → " << name << " 恢复了 " << healAmount << " 点生命值！" << std::endl;

        // 返回新生成的史莱姆（调用者会把它加入战场）
        return { newSlime };
    }
    else
    {
        // 血量还够，用弹跳攻击（1.2 倍伤害）
        PerformPhysicalAttack(target, 1.2, " 弹跳攻击！");
        return {};
    }
}

// ---------- 显示史莱姆信息 ----------
void Slime::DisplayInfo() const
{
    std::cout << "[史莱姆] " << name << " Lv." << level
              << " | 生命值: " << hp << "/" << maxHp
              << ", 攻击: " << attackPower
              << ", 防御: " << defend
              << ", 速度: " << speed
              << " | 经验奖励: " << experienceReward
              << ", 金币: " << goldReward
              << " | 掉落: " << dropItem
              << " (" << (dropRate * 100) << "%)" << std::endl;
}


// ================================================================
//  FireLizard 火焰蜥蜴 - 火属性怪物
//
//  属性：
//    - 攻击力较高（比史莱姆高），防御力较低
//    - 火属性（克制风，被水克制）
//    - 攻击方式：火焰吐息（1.3 倍伤害）
// ================================================================

FireLizard::FireLizard(int level)
    : Enemy("火焰蜥蜴", 45, 10, 3, level)
{
    // 根据等级重新计算属性
    this->maxHp = 35 + level * 4;       // 生命值：比史莱姆少
    this->hp = this->maxHp;
    this->attackPower = 7 + level * 3;  // 攻击力：比史莱姆高！
    this->defend = 1 + level;           // 防御力
    this->agility = 15 + level * 2;     // 灵巧
    this->speed = 90;                   // 速度中等
    this->spellDefense = 3;             // 法防一般
    this->elementType = ElementType::Fire;  // 火属性！

    // 击杀奖励
    this->experienceReward = 25 + level * 5;
    this->goldReward = 15 + level * 3;
    this->SetDropItem("火焰鳞片", 0.25);    // 25% 概率掉落
}

FireLizard::~FireLizard()
{
}

std::vector<std::shared_ptr<Enemy>> FireLizard::PerformAction(Character* target)
{
    // 火焰吐息：1.3 倍伤害
    PerformPhysicalAttack(target, 1.3, " 火焰吐息！");
    return {};
}

void FireLizard::DisplayInfo() const
{
    std::cout << "[火焰蜥蜴] " << name << " Lv." << level
              << " | 生命值: " << hp << "/" << maxHp
              << ", 攻击: " << attackPower
              << ", 防御: " << defend
              << ", 速度: " << speed
              << " | 经验奖励: " << experienceReward
              << ", 金币: " << goldReward
              << " | 属性: 火"
              << " | 掉落: " << dropItem
              << " (" << (dropRate * 100) << "%)" << std::endl;
}


// ================================================================
//  WindSprite 风精灵 - 风属性怪物
//
//  属性：
//    - 速度很快，攻击力中等
//    - 风属性（克制地，被火克制）
//    - 攻击方式：风刃（1.2 倍伤害）
// ================================================================

WindSprite::WindSprite(int level)
    : Enemy("风精灵", 40, 9, 2, level)
{
    // 根据等级重新计算属性
    this->maxHp = 28 + level * 3;       // 生命值：偏少（脆皮）
    this->hp = this->maxHp;
    this->attackPower = 6 + level * 2;  // 攻击力中等
    this->defend = 1 + level;           // 防御力一般
    this->agility = 25 + level * 3;     // 灵巧高
    this->speed = 130;                  // 速度最快！
    this->spellDefense = 4;             // 法防略高
    this->elementType = ElementType::Wind;  // 风属性！

    // 击杀奖励
    this->experienceReward = 30 + level * 5;
    this->goldReward = 12 + level * 2;
    this->SetDropItem("风之羽毛", 0.3);     // 30% 概率掉落
}

WindSprite::~WindSprite()
{
}

std::vector<std::shared_ptr<Enemy>> WindSprite::PerformAction(Character* target)
{
    // 风刃：1.2 倍伤害
    PerformPhysicalAttack(target, 1.2, " 风刃攻击！");
    return {};
}

void WindSprite::DisplayInfo() const
{
    std::cout << "[风精灵] " << name << " Lv." << level
              << " | 生命值: " << hp << "/" << maxHp
              << ", 攻击: " << attackPower
              << ", 防御: " << defend
              << ", 速度: " << speed
              << " | 经验奖励: " << experienceReward
              << ", 金币: " << goldReward
              << " | 属性: 风"
              << " | 掉落: " << dropItem
              << " (" << (dropRate * 100) << "%)" << std::endl;
}
