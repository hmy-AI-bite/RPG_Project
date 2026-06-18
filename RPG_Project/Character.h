#pragma once
#include <string>
#include <vector>
#include <memory>
#include "DamageSystem.h"

// 前向声明
class Inventory;
class Weapon;

// ================================================================
//  行动条相关常数
//  "行动条"就像一个水桶，桶满了你才能行动
//  桶的大小是 MAX_GAUGE = 500，谁先把桶装满谁就先出手！
// ================================================================
const int MAX_GAUGE = 500;

// ================================================================
//  Character 类 - 所有角色和怪物的"爸爸"（基类）
//  定义了所有生物都有的东西：名字、血量、攻击力、防御力等等
// ================================================================
class Character
{
protected:
    // ---------- 基本属性 ----------
    std::string name;       // 名字，比如"战士·剑心"、"史莱姆"
    int hp;                 // 当前生命值（血条），归零就死了
    int maxHp;              // 最大生命值（血条上限）
    int level;              // 等级，等级越高越厉害
    int attackPower;        // 攻击力，数值越大打得越疼
    int defend;             // 防御力，数值越大越抗揍
    int agility;            // 灵巧（影响暴击率），灵巧越高越容易暴击
    int speed;              // 速度（决定行动条填充速率），速度越快越早行动
    int spellDefense;       // 法术防御力，用来抵挡魔法伤害
    ElementType elementType;    // 属性类型（火/风/地/水/中立）
    ResistanceProfile resistance;  // 抗性（对物理和魔法的减免百分比）

    // ---------- 行动条系统 ----------
    // 行动条就像倒计时器，满了就能行动
    // 取值范围：0 ~ MAX_GAUGE（也就是 0 ~ 500）
    int currentGauge;

public:
    // 构造函数：创建一个角色，需要指定名字、血量、攻击、防御、等级
    Character(const std::string& name, int maxHp, int attackPower, int defend, int level);
    virtual ~Character();

    // ========== 战斗相关 ==========

    // 受到伤害（简单版），damage 是伤害值
    void TakeDamage(int damage);

    // 受到伤害（详细版），包含暴击、属性克制等完整信息
    void TakeDamage(const DamageInfo& damageInfo);

    // 恢复生命值，amount 是回复量（不会超过最大血量）
    void Heal(int amount);

    // 还活着吗？（hp > 0 就还活着）
    bool IsAlive() const;

    // ========== 行动条相关 ==========

    // 每过 deltaTime 毫秒，根据速度往行动条里加水
    // 公式：行动条增加 = (速度 × 时间) ÷ 100
    void UpdateGauge(int deltaTime);

    // 行动条满了没有？（满了就能行动了！）
    bool IsReadyToAct() const;

    // 行动完之后，把行动条减掉 MAX_GAUGE（倒掉一桶水）
    void ResetGauge();

    // 获取行动条百分比（0~100%），用于显示进度条
    int GetGaugePercent() const;

    // ========== 显示信息 ==========

    // 在屏幕上打印角色的所有属性
    virtual void DisplayInfo() const;

    // 显示职业特有信息（子类重写，消除 dynamic_cast）
    virtual void DisplayClassSpecificInfo() const {}

    // ========== 攻击相关 ==========

    // 通用物理攻击：对目标造成物理伤害
    //   target      = 打谁
    //   skillRatio  = 技能倍率（比如 1.5 就是 1.5 倍伤害）
    //   attackName  = 攻击的名字（比如"使出普通攻击！"）
    DamageInfo PerformPhysicalAttack(Character* target, double skillRatio, const std::string& attackName);

    // ========== Getter（获取属性）==========
    std::string GetName() const { return name; }
    int GetHp() const { return hp; }
    int GetMaxHp() const { return maxHp; }
    virtual int GetAttackPower() const { return attackPower; }
    int GetLevel() const { return level; }
    int GetDefend() const { return defend; }
    virtual int GetAgility() const { return agility; }
    virtual int GetSpeed() const { return speed; }
    virtual int GetSpellDefense() const { return spellDefense; }
    int GetCurrentGauge() const { return currentGauge; }
    ElementType GetElementType() const { return elementType; }
    ResistanceProfile GetResistance() const { return resistance; }

    // ========== Setter（设置属性）==========
    void SetAgility(int value) { agility = value; }
    void SetSpeed(int value) { speed = value; }
    void SetSpellDefense(int value) { spellDefense = value; }
    void SetElementType(ElementType type) { elementType = type; }
    void SetPhysicalResistance(double value) { resistance.physicalResistance = value; }
    void SetMagicalResistance(double value) { resistance.magicalResistance = value; }
    void SetAttackPower(int value) { attackPower = value; }
    void SetHp(int value) { hp = value; }
    void SetMaxHp(int value) { maxHp = value; }
    void SetLevel(int value) { level = value; }
    void SetDefend(int value) { defend = value; }
};

// 前向声明：告诉编译器"这些类在后面定义了，先别着急"
class Skill;

// Player 需要 Inventory 的完整定义（因为有 inventory 成员变量）
#include "Inventory.h"

// ================================================================
//  Player 类 - 玩家角色（继承自 Character）
//  比普通角色多了：经验值、蓝量（MP）、技能、背包、金币
// ================================================================
class Player : public Character
{
protected:
    int experience;     // 经验值，攒够了就升级
    int mp;             // 当前蓝量（魔法值），放技能要消耗蓝
    int maxMp;          // 最大蓝量
    int availableAP;    // 可分配属性点数（升级后获得，自由加点用）

    // 技能列表：玩家学会的所有技能都存在这里
    std::vector<std::shared_ptr<Skill>> skills;

    // 装备槽：装备的武器（nullptr = 未装备）
    Weapon* equippedWeapon;

public:
    Inventory inventory;    // 背包（可以装药水、装备等道具）
    int gold;               // 金币数量

public:
    // 构造函数：创建一个玩家，默认是战士类型
    Player(const std::string& name);
    virtual ~Player();

    // 获得经验值，自动判断是否升级（支持连续升级）
    void GainExperience(int exp);

    // 升级！各职业有不同的成长值（虚函数，子类可重写）
    // 升级后获得可分配属性点，不再自动加属性
    virtual void LevelUp();

    // 分配属性点（手动加点，每次加1点到指定属性）
    // 返回 true 表示分配成功，false 表示没有可用点数或属性无效
    virtual bool AllocateAttribute(int attrIndex);

    // 计算升级所需经验值（随等级递增）
    // 公式：level × 100，比如 1→2 需要 100，5→6 需要 500
    int GetExpToNextLevel() const;

    // 显示玩家详细信息
    virtual void DisplayInfo() const override;

    // 获取背包
    Inventory& GetInventory() { return inventory; }

    // ========== 装备系统 ==========
    // 装备武器（会先卸下当前武器）
    void EquipWeapon(Weapon* weapon);
    // 卸下武器（归还到背包，不删除）
    Weapon* UnequipWeapon();
    // 获取当前装备的武器
    Weapon* GetEquippedWeapon() const { return equippedWeapon; }
    // 动态计算攻击力（基础 + 武器加成）
    int GetAttackPower() const;
    // 动态计算灵巧（基础 + 武器加成）
    int GetAgility() const;
    // 动态计算速度（基础 + 武器加成）
    int GetSpeed() const;
    // 动态计算法防（基础 + 武器加成）
    int GetSpellDefense() const;
    // 获取基础攻击力（不含武器加成）
    int GetBaseAttackPower() const { return attackPower; }
    // 获取基础灵巧
    int GetBaseAgility() const { return agility; }
    // 获取基础速度
    int GetBaseSpeed() const { return speed; }
    // 获取基础法防
    int GetBaseSpellDefense() const { return spellDefense; }

    // ========== 蓝量（MP）相关 ==========
    void ConsumeMp(int amount);     // 消耗蓝量（放技能时用）
    void RestoreMp(int amount);     // 恢复蓝量（喝蓝药时用）
    bool HasEnoughMp(int amount) const { return mp >= amount; }  // 蓝量够不够？

    // ========== 攻击与技能 ==========

    // 普通攻击（不耗蓝，倍率 1.0）
    DamageInfo NormalAttack(Character* defender);

    // 学习新技能
    void AddSkill(std::shared_ptr<Skill> skill);

    // 获取所有技能列表
    const std::vector<std::shared_ptr<Skill>>& GetSkills() const { return skills; }

    // 使用指定编号的技能（0 是第一个技能，1 是第二个...）
    DamageInfo UseSkill(int skillIndex, Character* defender);

    // ========== Getter ==========
    int GetExperience() const { return experience; }
    int GetMp() const { return mp; }
    int GetMaxMp() const { return maxMp; }
    int GetGold() const { return gold; }
    int GetAvailableAP() const { return availableAP; }

    // ========== Setter（用于存档系统）==========
    void SetMp(int value) { mp = value; }
    void SetMaxMp(int value) { maxMp = value; }
    void SetExperience(int value) { experience = value; }
    void SetGold(int value) { gold = value; }
    void SetAvailableAP(int value) { availableAP = value; }
};

// ================================================================
//  Braver 类 - 勇者（继承自 Player）
//  特点：高攻击、低防御、高暴击率
//  特有系统：怒气值（被打会涨怒气，怒气满了可以放大招）
//  升级成长：ATK+7, HP+10, MP+6, SPD+3, 灵巧+5（攻击成长最高）
// ================================================================
class Braver : public Player
{
private:
    int rage;   // 怒气值（0~100），怒气越高大招越猛

public:
    Braver(const std::string& name);
    virtual ~Braver();

    void RageAttack(Character* target);              // 怒气斩击（消耗怒气造成大量伤害）
    void IncreaseRage(int amount);  // 增加怒气（被打时触发）
    void ExecuteTrueAttack(Character* target);       // 刺客之刃（真实伤害，无视防御！）
    virtual void DisplayInfo() const override;
    virtual void DisplayClassSpecificInfo() const override;
    virtual void LevelUp() override;                 // 勇者专属升级（获得 AP，可自由分配）
    virtual bool AllocateAttribute(int attrIndex) override;  // 勇者专属加点

    int GetRage() const { return rage; }
};

// ================================================================
//  Mage 类 - 法师（继承自 Player）
//  特点：低血量、低防御、高法术攻击力、超高暴击率
//  特有属性：法术强度（spellPower）
//  升级成长：spellPower+7, MP+12, SPD+4, 灵巧+3（法术成长最高）
// ================================================================
class Mage : public Player
{
private:
    int spellPower;     // 法术强度，数值越高魔法伤害越大

public:
    Mage(const std::string& name, int spellPower);
    virtual ~Mage();

    void CastSpell(int mpCost);     // 施放法术（消耗蓝量）
    virtual void DisplayInfo() const override;
    virtual void DisplayClassSpecificInfo() const override;
    virtual void LevelUp() override;                      // 法师专属升级（获得 AP，可自由分配）
    virtual bool AllocateAttribute(int attrIndex) override;  // 法师专属加点

    int GetSpellPower() const { return spellPower; }
};

// ================================================================
//  Guardian 类 - 守护骑士（继承自 Player）
//  特点：高血量、低攻击、高防御、低速度、坦克定位
//  特有属性：护盾值（shield），可以抵挡伤害
//  属性：HP 150, ATK 18, DEF 20, AGI 30, SPD 80, MP 60
//  属性类型：地（Earth）
//  升级成长：HP+12, MP+5, ATK+1, DEF+3, SPD+1, 灵巧+1（防御成长最高）
// ================================================================
class Guardian : public Player
{
private:
    int shield;     // 护盾值（0~50），可以抵挡伤害
    static const int MAX_SHIELD = 50;

public:
    Guardian(const std::string& name);
    virtual ~Guardian();

    // 守护姿态：消耗 15 MP，获得 30 点护盾值
    void GuardStance();

    // 圣光斩：消耗 20 MP，造成 1.2 倍物理伤害，同时回复 attackPower * 0.3 点血量
    void HolySlash(Character* target);

    // 重写 TakeDamage：优先扣护盾，护盾归零后多余伤害才扣血
    virtual void TakeDamage(int damage);
    virtual void TakeDamage(const DamageInfo& damageInfo);

    // 显示守护骑士信息（包括护盾值）
    virtual void DisplayInfo() const override;
    virtual void DisplayClassSpecificInfo() const override;

    virtual void LevelUp() override;                      // 守护骑士专属升级（获得 AP，可自由分配）
    virtual bool AllocateAttribute(int attrIndex) override;  // 守护骑士专属加点

    int GetShield() const { return shield; }
    void SetShield(int value) { shield = value; if (shield > MAX_SHIELD) shield = MAX_SHIELD; if (shield < 0) shield = 0; }
};
