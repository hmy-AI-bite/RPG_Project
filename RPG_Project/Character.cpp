#include "Character.h"
#include "Inventory.h"
#include "Skill.h"
#include <iostream>

// ================================================================
//  Character 类 - 所有角色和怪物的"爸爸"类
//  下面实现 Character 中定义的所有方法
// ================================================================

// ---------- 构造函数 ----------
// 创建一个新角色时，把各项属性设置为传入的值
// 如果有些属性没传（如灵巧、速度），就用默认值
Character::Character(const std::string& name, int maxHp, int attackPower, int defend, int level)
    : name(name), maxHp(maxHp), hp(maxHp), attackPower(attackPower), defend(defend), level(level),
      agility(10), speed(100), spellDefense(5), elementType(ElementType::Neutral), currentGauge(0)
{
    // 默认值说明：
    //   灵巧 = 10   → 暴击率 1%
    //   速度 = 100  → 中等速度
    //   法防 = 5    → 基础法术防御
    //   属性 = 中立 → 无属性克制关系
    //   行动条 = 0  → 刚进战斗还没攒行动条
}

Character::~Character()
{
}

// ---------- 受到伤害（简单版）----------
// 直接扣血，扣到 0 为止（不会变成负数）
void Character::TakeDamage(int damage)
{
    if (damage <= 0) return;    // 伤害 <= 0 就忽略
    hp -= damage;               // 扣血
    if (hp < 0) hp = 0;         // 血量最低为 0，不会变成负数
    std::cout << name << " 受到了 " << damage << " 点伤害！生命值: " << hp << "/" << maxHp << std::endl;
}

// ---------- 受到伤害（详细版）----------
// 包含暴击信息，如果暴击了会显示【暴击】字样
void Character::TakeDamage(const DamageInfo& damageInfo)
{
    int finalDamage = static_cast<int>(damageInfo.finalDamage);  // 最终伤害取整
    hp -= finalDamage;
    if (hp < 0) hp = 0;

    // 如果暴击了，显示【暴击】标记让玩家知道
    std::string criticalText = damageInfo.isCritical ? "【暴击】" : "";
    std::cout << name << " 受到了 " << finalDamage << " 点伤害 " << criticalText
              << "！生命值: " << hp << "/" << maxHp << std::endl;
}

// ---------- 恢复生命值 ----------
// 回血但不会超过最大血量（不能溢出）
void Character::Heal(int amount)
{
    if (amount <= 0) return;
    hp += amount;
    if (hp > maxHp) hp = maxHp;     // 最多回到满血
    std::cout << name << " 恢复了 " << amount << " 点生命值。生命值: " << hp << "/" << maxHp << std::endl;
}

// ---------- 还活着吗？----------
// hp > 0 就是活着，hp = 0 就是死了
bool Character::IsAlive() const
{
    return hp > 0;
}

// ---------- 更新行动条 ----------
// 每过 deltaTime 毫秒，根据速度往行动条里"加水"
// 公式：行动条增加 = (速度 × deltaTime) ÷ 100
// 比如速度 100，每 50 毫秒：增加 (100 × 50) ÷ 100 = 50 点
// 满条是 500，所以大约 10 帧就能满
void Character::UpdateGauge(int deltaTime)
{
    currentGauge += (speed * deltaTime) / 100;

    // 防止行动条溢出太多（最多存 2 倍，也就是 1000）
    if (currentGauge > MAX_GAUGE * 2)
    {
        currentGauge = MAX_GAUGE * 2;
    }
}

// ---------- 行动条满了没？----------
// 满了（>= 500）就能行动了！
bool Character::IsReadyToAct() const
{
    return currentGauge >= MAX_GAUGE;
}

// ---------- 行动后重置行动条 ----------
// 行动完之后，倒掉一桶水（减去 500）
// 如果有溢出（比如原本 520），剩余 20 会保留到下一轮
void Character::ResetGauge()
{
    currentGauge -= MAX_GAUGE;
    if (currentGauge < 0)
    {
        currentGauge = 0;
    }
}

// ---------- 获取行动条百分比 ----------
// 返回 0~100 的百分比，用于在界面上画进度条
int Character::GetGaugePercent() const
{
    return (currentGauge * 100) / MAX_GAUGE;
}

// ---------- 通用物理攻击 ----------
// 这是所有物理攻击的"模板"，普通攻击和物理技能都通过它来实现
// 流程：
//   1. 用伤害计算器算出伤害
//   2. 打印攻击信息
//   3. 让目标受到伤害
DamageInfo Character::PerformPhysicalAttack(Character* target, double skillRatio, const std::string& attackName)
{
    // 目标不能为空
    if (!target)
    {
        DamageInfo empty;
        empty.finalDamage = 0;
        return empty;
    }

    // 创建伤害计算器，计算伤害
    DamageCalculator calculator;
    DamageInfo damageInfo = calculator.CalculatePhysicalDamage(
        attackPower,                    // 我的攻击力
        target->GetDefend(),            // 目标的防御力
        level,                          // 我的等级
        target->GetLevel(),             // 目标的等级
        skillRatio,                     // 技能倍率
        elementType,                    // 我的属性类型
        target->GetElementType(),       // 目标的属性类型（用于属性克制判定）
        agility,                        // 我的灵巧（决定暴击率）
        target->GetResistance()         // 目标的抗性
    );

    // 打印攻击信息 + 造成伤害
    std::cout << name << attackName;
    target->TakeDamage(damageInfo);

    return damageInfo;
}

// ---------- 显示信息 ----------
// 在屏幕上打印角色的所有属性
void Character::DisplayInfo() const
{
    std::cout << "[" << name << "] 生命值: " << hp << "/" << maxHp
              << ", 攻击: " << attackPower
              << ", 防御: " << defend
              << ", 法防: " << spellDefense
              << ", 灵巧: " << agility
              << ", 速度: " << speed
              << ", 等级: " << level << std::endl;
}


// ================================================================
//  Player 类 - 玩家角色
//  比普通角色多了经验、蓝量、技能、背包、金币
// ================================================================

// ---------- 构造函数 ----------
// 默认创建战士类型的玩家：
//   血量 120、攻击 20、防御 10、蓝量 50、金币 100
Player::Player(const std::string& name)
    : Character(name, 120, 20, 10, 1), experience(0), maxMp(50), mp(50), gold(100)
{
    // 战士属性：均衡型，能抗能打
    hp = maxHp;                 // 初始满血
    agility = 60;               // 灵巧 60 → 暴击率 6%
    spellDefense = 8;           // 法防 8
    speed = 100;                // 速度 100（中等）
    elementType = ElementType::Neutral;  // 无属性
}

Player::~Player()
{
}

// ---------- 获得经验值 ----------
// 每获得 100 经验就升一级！
void Player::GainExperience(int exp)
{
    if (exp <= 0) return;
    experience += exp;
    std::cout << name << " 获得了 " << exp << " 点经验值！总经验: " << experience << std::endl;

    // 经验 >= 100 就升级
    if (experience >= 100)
    {
        LevelUp();
    }
}

// ---------- 升级！----------
// 消耗 100 经验，等级 +1，各项属性提升
void Player::LevelUp()
{
    if (experience >= 100)
    {
        experience -= 100;      // 消耗 100 经验
        level++;                // 等级 +1
        maxHp += 10;            // 最大血量 +10
        hp = maxHp;             // 升级后回满血
        maxMp += 10;            // 最大蓝量 +10
        mp = maxMp;             // 升级后回满蓝
        attackPower += 5;       // 攻击力 +5
        defend += 2;            // 防御力 +2
        std::cout << name << " 升级到 " << level << " 级！属性已增强！" << std::endl;
    }
}

// ---------- 消耗蓝量 ----------
// 放技能时消耗蓝，蓝量最低为 0
void Player::ConsumeMp(int amount)
{
    if (amount <= 0) return;
    mp -= amount;
    if (mp < 0) mp = 0;
    std::cout << name << " 消耗了 " << amount << " 点蓝量。当前蓝量: " << mp << "/" << maxMp << std::endl;
}

// ---------- 恢复蓝量 ----------
// 喝蓝药时恢复蓝，不会超过最大值
void Player::RestoreMp(int amount)
{
    if (amount <= 0) return;
    mp += amount;
    if (mp > maxMp) mp = maxMp;
    std::cout << name << " 恢复了 " << amount << " 点蓝量。当前蓝量: " << mp << "/" << maxMp << std::endl;
}

// ---------- 普通攻击 ----------
// 不耗蓝，技能倍率 1.0（正常伤害）
DamageInfo Player::NormalAttack(Character* defender)
{
    return PerformPhysicalAttack(defender, 1.0, " 使出普通攻击！");
}

// ---------- 学习新技能 ----------
// 把技能加到技能列表里
void Player::AddSkill(std::shared_ptr<Skill> skill)
{
    if (skill)
    {
        skills.push_back(skill);
        std::cout << name << " 学会了新技能 [" << skill->GetName() << "]！" << std::endl;
    }
}

// ---------- 使用技能 ----------
// 使用技能列表中的第 skillIndex 个技能
// 流程：
//   1. 检查技能编号是否有效
//   2. 检查目标是否有效
//   3. 检查蓝量够不够
//   4. 消耗蓝量
//   5. 执行技能并造成伤害
DamageInfo Player::UseSkill(int skillIndex, Character* defender)
{
    // 检查技能编号
    if (skillIndex < 0 || skillIndex >= static_cast<int>(skills.size()))
    {
        std::cout << name << " 没有这个技能！" << std::endl;
        return DamageInfo();
    }

    // 检查目标
    if (!defender)
    {
        std::cout << "目标无效！" << std::endl;
        return DamageInfo();
    }

    auto& skill = skills[skillIndex];
    int mpCost = skill->GetMpCost();

    // 检查蓝量够不够
    if (!HasEnoughMp(mpCost))
    {
        std::cout << name << " 蓝量不足！需要 " << mpCost
                  << " 点蓝量，当前蓝量: " << mp << "/" << maxMp << std::endl;
        return DamageInfo();
    }

    // 消耗蓝量
    ConsumeMp(mpCost);

    // 执行技能并应用伤害
    DamageInfo info = skill->ExecuteSkill(this, defender);
    defender->TakeDamage(info);

    return info;
}

// ---------- 显示玩家信息 ----------
void Player::DisplayInfo() const
{
    std::cout << "[战士] " << name
              << " | 生命值: " << hp << "/" << maxHp
              << ", 蓝量: " << mp << "/" << maxMp
              << ", 攻击: " << attackPower
              << ", 防御: " << defend
              << ", 灵巧: " << agility
              << ", 速度: " << speed
              << ", 等级: " << level
              << ", 经验: " << experience << std::endl;
}


// ================================================================
//  Braver 类 - 勇者
//  高攻击、低防御、高暴击率，有怒气系统
// ================================================================

// ---------- 构造函数 ----------
Braver::Braver(const std::string& name)
    : Player(name), rage(0)
{
    // 勇者属性：高攻击力、低防御、高暴击率
    maxHp = 140;                // 血量 140（比战士多）
    hp = maxHp;
    defend = 6;                 // 防御 6（比战士低）
    attackPower = 35;           // 攻击 35（比战士高很多！）
    agility = 120;              // 灵巧 120 → 暴击率 12%（暴击概率更高！）
    spellDefense = 5;
    speed = 110;                // 速度 110（比战士快一点）
    maxMp = 60;
    mp = maxMp;
    elementType = ElementType::Water;  // 水属性
}

Braver::~Braver()
{
}

// ---------- 怒气斩击 ----------
// 消耗 50 点怒气，造成攻击力 × 2 + 怒气 ÷ 10 的伤害
void Braver::RageAttack(Character* target)
{
    if (!target) return;

    if (rage < 50)
    {
        std::cout << name << " 怒气不足！当前怒气: " << rage << "/100" << std::endl;
        return;
    }

    int damage = attackPower * 2 + rage / 10;
    std::cout << name << " 使出怒气斩击！伤害: " << damage << std::endl;
    target->TakeDamage(damage);     // 实际扣血！
    rage = 0;   // 怒气用完
}

// ---------- 增加怒气 ----------
// 被打时会加怒气，最高 100
void Braver::IncreaseRage(int amount)
{
    if (amount <= 0) return;
    rage += amount;
    if (rage > 100) rage = 100;
    std::cout << name << " 的怒气增加到 " << rage << "/100" << std::endl;
}

// ---------- 刺客之刃（真实伤害）----------
// 无视防御和抗性！伤害 = 30 + 攻击力 × 0.5
void Braver::ExecuteTrueAttack(Character* target)
{
    if (!target) return;

    // 使用伤害计算器生成真实伤害信息
    DamageCalculator calculator;
    DamageInfo info = calculator.CalculateTrueDamage(
        30,                         // 固定伤害 30
        attackPower,                // 攻击力
        level,                      // 我的等级
        target->GetLevel()          // 目标的等级
    );

    std::cout << name << " 使出刺客之刃！造成真实伤害: " << static_cast<int>(info.finalDamage)
              << " (无视防御!)" << std::endl;

    target->TakeDamage(info);       // 实际扣血！
}

// ---------- 显示勇者信息 ----------
void Braver::DisplayInfo() const
{
    std::cout << "[勇敢者] " << name
              << " | 生命值: " << hp << "/" << maxHp
              << ", 蓝量: " << mp << "/" << maxMp
              << ", 攻击: " << attackPower
              << ", 防御: " << defend
              << ", 灵巧: " << agility
              << ", 速度: " << speed
              << ", 等级: " << level
              << ", 经验: " << experience
              << ", 怒气: " << rage << "/100" << std::endl;
}


// ================================================================
//  Mage 类 - 法师
//  低血量、低防御、高法术攻击力、超高暴击率
// ================================================================

// ---------- 构造函数 ----------
Mage::Mage(const std::string& name, int spellPower)
    : Player(name), spellPower(spellPower)
{
    // 法师属性：低血量、低防御、高暴击
    maxHp = 80;                 // 血量 80（最少）
    hp = maxHp;
    defend = 3;                 // 防御 3（最低）
    attackPower = 28;           // 攻击 28（中等）
    agility = 150;              // 灵巧 150 → 暴击率 15%（最高！）
    spellDefense = 8;           // 法防 8（对魔法有一定抵抗）
    speed = 120;                // 速度 120（最快！）
    maxMp = 100;                // 蓝量 100（最多！）
    mp = maxMp;
    elementType = ElementType::Fire;  // 火属性
}

Mage::~Mage()
{
}

// ---------- 施放法术 ----------
// 消耗蓝量施放法术（火焰术）
void Mage::CastSpell(int mpCost)
{
    if (mpCost <= 0)
    {
        std::cout << "技能消耗无效！" << std::endl;
        return;
    }

    if (!HasEnoughMp(mpCost))
    {
        std::cout << name << " 蓝量不足！需要 " << mpCost
                  << " 点蓝量，当前蓝量: " << mp << std::endl;
        return;
    }

    ConsumeMp(mpCost);
    std::cout << name << " 施放了火焰术！" << std::endl;
}

// ---------- 显示法师信息 ----------
void Mage::DisplayInfo() const
{
    std::cout << "[法师] " << name
              << " | 生命值: " << hp << "/" << maxHp
              << ", 蓝量: " << mp << "/" << maxMp
              << ", 攻击: " << attackPower
              << ", 法力: " << spellPower
              << ", 防御: " << defend
              << ", 灵巧: " << agility
              << ", 速度: " << speed
              << ", 等级: " << level
              << ", 经验: " << experience << std::endl;
}
