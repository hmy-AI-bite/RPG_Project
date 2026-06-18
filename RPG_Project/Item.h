#pragma once
#include <string>
#include <memory>

class Character;

// 道具抽象基类，定义道具的基本接口
class Item
{
public:
    virtual ~Item() = default;
    virtual std::string GetName() const = 0;
    virtual std::string GetDescription() const = 0;
    virtual void Use(Character* user) = 0;
    virtual std::unique_ptr<Item> Clone() const = 0;
};

// 药水类型枚举
enum class PotionType {
    HEALTH,      // 恢复生命
    ATTACK,      // 增加攻击
    AGILITY,     // 增加灵巧
    SPEED,       // 增加速度
    DEFENSE      // 增加防御
};

// 药水类：恢复角色属性
class Potion : public Item
{
private:
    int amount;
    PotionType type;
    std::string name;
    std::string description;

public:
    Potion(int amount, PotionType type, const std::string& name, const std::string& desc);
    Potion(const Potion& other);
    std::string GetName() const override;
    std::string GetDescription() const override;
    void Use(Character* user) override;
    std::unique_ptr<Item> Clone() const override;
    
    int GetAmount() const { return amount; }
    PotionType GetType() const { return type; }
    
    // 6种药水（返回 unique_ptr）
    static std::unique_ptr<Potion> CreateSmallHealthPotion() { return std::make_unique<Potion>(20, PotionType::HEALTH, "小型治疗药水", "恢复20点生命值"); }
    static std::unique_ptr<Potion> CreateMediumHealthPotion() { return std::make_unique<Potion>(50, PotionType::HEALTH, "中型治疗药水", "恢复50点生命值"); }
    static std::unique_ptr<Potion> CreateStrengthPotion() { return std::make_unique<Potion>(5, PotionType::ATTACK, "力量药水", "临时增加5点攻击力"); }
    static std::unique_ptr<Potion> CreateAgilityPotion() { return std::make_unique<Potion>(5, PotionType::AGILITY, "敏捷药水", "临时增加5点灵巧（提升暴击率）"); }
    static std::unique_ptr<Potion> CreateSpeedPotion() { return std::make_unique<Potion>(5, PotionType::SPEED, "迅捷药水", "临时增加5点速度"); }
    static std::unique_ptr<Potion> CreateDefensePotion() { return std::make_unique<Potion>(5, PotionType::DEFENSE, "护盾药水", "临时增加5点法术防御"); }
};

// 武器类型枚举
enum class WeaponType {
    SWORD,       // 剑类（均衡型）
    DAGGER,      // 匕首（高暴击）
    BOW,         // 弓（高速度）
    STAFF,       // 法杖（魔法攻击）
    GREAT_SWORD, // 巨剑（高攻击）
    SHIELD       // 盾牌（防御）
};

// 武器类：增加角色属性
class Weapon : public Item
{
private:
    int attackBonus;
    int agilityBonus;
    int speedBonus;
    int defenseBonus;
    WeaponType type;
    std::string name;
    std::string description;

public:
    Weapon(int attack, int agility, int speed, int defense, WeaponType type, const std::string& name, const std::string& desc);
    Weapon(const Weapon& other);
    std::string GetName() const override;
    std::string GetDescription() const override;
    void Use(Character* user) override;
    std::unique_ptr<Item> Clone() const override;
    
    int GetAttackBonus() const { return attackBonus; }
    int GetAgilityBonus() const { return agilityBonus; }
    int GetSpeedBonus() const { return speedBonus; }
    int GetDefenseBonus() const { return defenseBonus; }
    WeaponType GetType() const { return type; }
    
    // 6种武器，对应角色特性（返回 unique_ptr）
    static std::unique_ptr<Weapon> CreateLongSword() { return std::make_unique<Weapon>(10, 0, 0, 0, WeaponType::SWORD, "长剑", "精钢打造的长剑，均衡的攻击力"); }
    static std::unique_ptr<Weapon> CreateDagger() { return std::make_unique<Weapon>(3, 8, 3, 0, WeaponType::DAGGER, "匕首", "锋利的匕首，高暴击率"); }
    static std::unique_ptr<Weapon> CreateBow() { return std::make_unique<Weapon>(6, 3, 8, 0, WeaponType::BOW, "长弓", "精准的长弓，高速度攻击"); }
    static std::unique_ptr<Weapon> CreateStaff() { return std::make_unique<Weapon>(8, 5, 0, 5, WeaponType::STAFF, "法杖", "神秘法杖，增加法术防御"); }
    static std::unique_ptr<Weapon> CreateGreatSword() { return std::make_unique<Weapon>(20, -2, -3, 0, WeaponType::GREAT_SWORD, "巨剑", "沉重的巨剑，牺牲速度换取高攻击"); }
    static std::unique_ptr<Weapon> CreateShield() { return std::make_unique<Weapon>(-2, 0, -2, 10, WeaponType::SHIELD, "盾牌", "坚固的盾牌，牺牲攻击换取防御"); }
};