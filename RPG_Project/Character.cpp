#include "Character.h"
#include <iostream>

Character::Character(const std::string& name, int maxHp, int attackPower)
    : name(name), maxHp(maxHp), hp(maxHp), attackPower(attackPower)
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
    std::cout << name << " took " << damage << " damage! HP: " << hp << "/" << maxHp << std::endl;
}

void Character::Heal(int amount)
{
    if (amount <= 0) return;
    hp += amount;
    if (hp > maxHp) hp = maxHp;
    std::cout << name << " healed " << amount << " HP. HP: " << hp << "/" << maxHp << std::endl;
}

bool Character::IsAlive() const
{
    return hp > 0;
}

void Character::DisplayInfo() const
{
    std::cout << "[" << name << "] HP: " << hp << "/" << maxHp << ", ATK: " << attackPower << std::endl;
}