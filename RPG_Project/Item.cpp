#include "Item.h"
#include "Character.h"
#include <iostream>

// ============================================================
//                      Potion 类实现
// ============================================================

Potion::Potion(int amount, PotionType type, const std::string& name, const std::string& desc)
    : amount(amount), type(type), name(name), description(desc) {}

Potion::Potion(const Potion& other) 
    : amount(other.amount), type(other.type), name(other.name), description(other.description) {}

std::string Potion::GetName() const { return name; }

std::string Potion::GetDescription() const { return description; }

void Potion::Use(Character* user)
{
    if (user != nullptr)
    {
        switch (type)
        {
        case PotionType::HEALTH:
            user->Heal(amount);
            std::cout << user->GetName() << " 使用了 " << name << "，恢复了 " << amount << " 点生命值！" << std::endl;
            break;
        case PotionType::ATTACK:
            user->SetAttackPower(user->GetAttackPower() + amount);
            std::cout << user->GetName() << " 使用了 " << name << "，攻击力临时增加 " << amount << " 点！" << std::endl;
            break;
        case PotionType::AGILITY:
            user->SetAgility(user->GetAgility() + amount);
            std::cout << user->GetName() << " 使用了 " << name << "，灵巧临时增加 " << amount << " 点！" << std::endl;
            break;
        case PotionType::SPEED:
            user->SetSpeed(user->GetSpeed() + amount);
            std::cout << user->GetName() << " 使用了 " << name << "，速度临时增加 " << amount << " 点！" << std::endl;
            break;
        case PotionType::DEFENSE:
            user->SetSpellDefense(user->GetSpellDefense() + amount);
            std::cout << user->GetName() << " 使用了 " << name << "，法术防御临时增加 " << amount << " 点！" << std::endl;
            break;
        }
    }
}

Item* Potion::Clone() const { return new Potion(*this); }

// ============================================================
//                      Weapon 类实现
// ============================================================

Weapon::Weapon(int attack, int agility, int speed, int defense, WeaponType type, const std::string& name, const std::string& desc)
    : attackBonus(attack), agilityBonus(agility), speedBonus(speed), defenseBonus(defense), type(type), name(name), description(desc) {}

Weapon::Weapon(const Weapon& other) 
    : attackBonus(other.attackBonus), agilityBonus(other.agilityBonus), 
      speedBonus(other.speedBonus), defenseBonus(other.defenseBonus),
      type(other.type), name(other.name), description(other.description) {}

std::string Weapon::GetName() const { return name; }

std::string Weapon::GetDescription() const { return description; }

void Weapon::Use(Character* user)
{
    if (user != nullptr)
    {
        user->SetAttackPower(user->GetAttackPower() + attackBonus);
        user->SetAgility(user->GetAgility() + agilityBonus);
        user->SetSpeed(user->GetSpeed() + speedBonus);
        user->SetSpellDefense(user->GetSpellDefense() + defenseBonus);
        
        std::cout << user->GetName() << " 装备了 " << name << "！" << std::endl;
        if (attackBonus != 0)
            std::cout << " 攻击力 " << (attackBonus > 0 ? "+" : "") << attackBonus << std::endl;
        if (agilityBonus != 0)
            std::cout << " 灵巧 " << (agilityBonus > 0 ? "+" : "") << agilityBonus << std::endl;
        if (speedBonus != 0)
            std::cout << " 速度 " << (speedBonus > 0 ? "+" : "") << speedBonus << std::endl;
        if (defenseBonus != 0)
            std::cout << " 法术防御 " << (defenseBonus > 0 ? "+" : "") << defenseBonus << std::endl;
    }
}

Item* Weapon::Clone() const { return new Weapon(*this); }