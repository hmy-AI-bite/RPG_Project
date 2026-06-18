#include "SaveLoadManager.h"
#include "Character.h"
#include "Item.h"
#include "Inventory.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <memory>

// ============================================================
//                      保存游戏
// ============================================================

void SaveLoadManager::SaveGame(const Player& player, const std::string& filename)
{
    std::ofstream outFile(filename, std::ios::binary);

    if (!outFile.is_open())
    {
        std::cout << "错误：无法打开存档文件 " << filename << " 进行写入！" << std::endl;
        return;
    }

    // ========== 保存玩家名称 ==========
    std::string name = player.GetName();
    uint32_t nameLength = static_cast<uint32_t>(name.size());

    outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
    outFile.write(name.c_str(), nameLength);

    // ========== 保存基础属性 ==========
    int hp = player.GetHp();
    int maxHp = player.GetMaxHp();
    int mp = player.GetMp();
    int maxMp = player.GetMaxMp();
    int level = player.GetLevel();
    int exp = player.GetExperience();
    int atk = player.GetBaseAttackPower();  // 保存基础攻击力（不含武器）
    int def = player.GetDefend();
    int spd = player.GetBaseSpeed();        // 保存基础速度
    int gold = player.GetGold();

    outFile.write(reinterpret_cast<const char*>(&hp), sizeof(hp));
    outFile.write(reinterpret_cast<const char*>(&maxHp), sizeof(maxHp));
    outFile.write(reinterpret_cast<const char*>(&mp), sizeof(mp));
    outFile.write(reinterpret_cast<const char*>(&maxMp), sizeof(maxMp));
    outFile.write(reinterpret_cast<const char*>(&level), sizeof(level));
    outFile.write(reinterpret_cast<const char*>(&exp), sizeof(exp));
    outFile.write(reinterpret_cast<const char*>(&atk), sizeof(atk));
    outFile.write(reinterpret_cast<const char*>(&def), sizeof(def));
    outFile.write(reinterpret_cast<const char*>(&spd), sizeof(spd));
    outFile.write(reinterpret_cast<const char*>(&gold), sizeof(gold));

    // ========== 保存背包物品 ==========
    const Inventory& inv = player.inventory;
    uint32_t itemCount = static_cast<uint32_t>(inv.GetItemCount());

    outFile.write(reinterpret_cast<const char*>(&itemCount), sizeof(itemCount));

    for (uint32_t i = 0; i < itemCount; ++i)
    {
        Item* item = inv.GetItem(static_cast<int>(i));

        if (item == nullptr)
        {
            uint8_t itemType = 0;
            outFile.write(reinterpret_cast<const char*>(&itemType), sizeof(itemType));
            continue;
        }

        Potion* potion = dynamic_cast<Potion*>(item);
        Weapon* weapon = dynamic_cast<Weapon*>(item);

        if (potion)
        {
            uint8_t itemType = 1;
            outFile.write(reinterpret_cast<const char*>(&itemType), sizeof(itemType));

            uint8_t potionType = static_cast<uint8_t>(potion->GetType());
            outFile.write(reinterpret_cast<const char*>(&potionType), sizeof(potionType));

            int amount = potion->GetAmount();
            outFile.write(reinterpret_cast<const char*>(&amount), sizeof(amount));

            std::string name = potion->GetName();
            uint32_t nameLen = static_cast<uint32_t>(name.size());
            outFile.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
            outFile.write(name.c_str(), nameLen);

            std::string desc = potion->GetDescription();
            uint32_t descLen = static_cast<uint32_t>(desc.size());
            outFile.write(reinterpret_cast<const char*>(&descLen), sizeof(descLen));
            outFile.write(desc.c_str(), descLen);
        }
        else if (weapon)
        {
            uint8_t itemType = 2;
            outFile.write(reinterpret_cast<const char*>(&itemType), sizeof(itemType));

            uint8_t weaponType = static_cast<uint8_t>(weapon->GetType());
            outFile.write(reinterpret_cast<const char*>(&weaponType), sizeof(weaponType));

            int attack = weapon->GetAttackBonus();
            int agility = weapon->GetAgilityBonus();
            int speed = weapon->GetSpeedBonus();
            int defense = weapon->GetDefenseBonus();
            outFile.write(reinterpret_cast<const char*>(&attack), sizeof(attack));
            outFile.write(reinterpret_cast<const char*>(&agility), sizeof(agility));
            outFile.write(reinterpret_cast<const char*>(&speed), sizeof(speed));
            outFile.write(reinterpret_cast<const char*>(&defense), sizeof(defense));

            std::string name = weapon->GetName();
            uint32_t nameLen = static_cast<uint32_t>(name.size());
            outFile.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
            outFile.write(name.c_str(), nameLen);

            std::string desc = weapon->GetDescription();
            uint32_t descLen = static_cast<uint32_t>(desc.size());
            outFile.write(reinterpret_cast<const char*>(&descLen), sizeof(descLen));
            outFile.write(desc.c_str(), descLen);
        }
        else
        {
            uint8_t itemType = 0;
            outFile.write(reinterpret_cast<const char*>(&itemType), sizeof(itemType));
        }
    }

    outFile.close();
    std::cout << "游戏已保存到 " << filename << std::endl;
}

// ============================================================
//                      加载游戏
// ============================================================

bool SaveLoadManager::LoadGame(Player& player, const std::string& filename)
{
    std::ifstream inFile(filename, std::ios::binary);

    if (!inFile.is_open())
    {
        std::cout << "错误：存档文件 " << filename << " 不存在！" << std::endl;
        return false;
    }

    // ========== 读取玩家名称 ==========
    uint32_t nameLength;
    inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));

    if (!inFile.good())
    {
        std::cout << "错误：存档文件格式错误（读取名称长度失败）！" << std::endl;
        inFile.close();
        return false;
    }

    std::string name;
    name.resize(nameLength);
    inFile.read(&name[0], nameLength);

    // ========== 读取基础属性 ==========
    int hp, maxHp, mp, maxMp, level, exp, atk, def, spd, gold;

    inFile.read(reinterpret_cast<char*>(&hp), sizeof(hp));
    inFile.read(reinterpret_cast<char*>(&maxHp), sizeof(maxHp));
    inFile.read(reinterpret_cast<char*>(&mp), sizeof(mp));
    inFile.read(reinterpret_cast<char*>(&maxMp), sizeof(maxMp));
    inFile.read(reinterpret_cast<char*>(&level), sizeof(level));
    inFile.read(reinterpret_cast<char*>(&exp), sizeof(exp));
    inFile.read(reinterpret_cast<char*>(&atk), sizeof(atk));
    inFile.read(reinterpret_cast<char*>(&def), sizeof(def));
    inFile.read(reinterpret_cast<char*>(&spd), sizeof(spd));
    inFile.read(reinterpret_cast<char*>(&gold), sizeof(gold));

    if (!inFile.good())
    {
        std::cout << "错误：存档文件格式错误（读取属性失败）！" << std::endl;
        inFile.close();
        return false;
    }

    // ========== 应用读取的数据到玩家对象 ==========
    player.SetHp(hp);
    player.SetMaxHp(maxHp);
    player.SetMp(mp);
    player.SetMaxMp(maxMp);
    player.SetLevel(level);
    player.SetExperience(exp);
    player.SetAttackPower(atk);   // 设置基础攻击力
    player.SetDefend(def);
    player.SetSpeed(spd);         // 设置基础速度
    player.SetGold(gold);

    // ========== 读取背包物品 ==========
    uint32_t itemCount;
    inFile.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));

    if (!inFile.good())
    {
        std::cout << "错误：存档文件格式错误（读取物品数量失败）！" << std::endl;
        inFile.close();
        return false;
    }

    // 清空原背包
    while (player.inventory.GetItemCount() > 0)
    {
        player.inventory.RemoveItem(0);
    }

    // 读取并重建每个物品
    for (uint32_t i = 0; i < itemCount; ++i)
    {
        uint8_t itemType;
        inFile.read(reinterpret_cast<char*>(&itemType), sizeof(itemType));

        if (!inFile.good())
        {
            std::cout << "警告：部分物品读取失败！" << std::endl;
            break;
        }

        if (itemType == 1)  // Potion
        {
            uint8_t potionType;
            inFile.read(reinterpret_cast<char*>(&potionType), sizeof(potionType));

            int amount;
            inFile.read(reinterpret_cast<char*>(&amount), sizeof(amount));

            uint32_t nameLen;
            inFile.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            std::string name(nameLen, '\0');
            inFile.read(&name[0], nameLen);

            uint32_t descLen;
            inFile.read(reinterpret_cast<char*>(&descLen), sizeof(descLen));
            std::string desc(descLen, '\0');
            inFile.read(&desc[0], descLen);

            player.inventory.AddItem(
                std::make_unique<Potion>(amount, static_cast<PotionType>(potionType), name, desc)
            );
        }
        else if (itemType == 2)  // Weapon
        {
            uint8_t weaponType;
            inFile.read(reinterpret_cast<char*>(&weaponType), sizeof(weaponType));

            int attack, agility, speed, defense;
            inFile.read(reinterpret_cast<char*>(&attack), sizeof(attack));
            inFile.read(reinterpret_cast<char*>(&agility), sizeof(agility));
            inFile.read(reinterpret_cast<char*>(&speed), sizeof(speed));
            inFile.read(reinterpret_cast<char*>(&defense), sizeof(defense));

            uint32_t nameLen;
            inFile.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            std::string name(nameLen, '\0');
            inFile.read(&name[0], nameLen);

            uint32_t descLen;
            inFile.read(reinterpret_cast<char*>(&descLen), sizeof(descLen));
            std::string desc(descLen, '\0');
            inFile.read(&desc[0], descLen);

            player.inventory.AddItem(
                std::make_unique<Weapon>(attack, agility, speed, defense, static_cast<WeaponType>(weaponType), name, desc)
            );
        }
        else
        {
            std::cout << "警告：发现未知物品类型，已跳过。" << std::endl;
        }
    }

    inFile.close();
    std::cout << "游戏已从 " << filename << " 加载成功！" << std::endl;
    return true;
}
