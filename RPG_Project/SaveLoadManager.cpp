#include "SaveLoadManager.h"
#include "Character.h"
#include "Item.h"
#include "Inventory.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <cstdint>

// 静态成员初始化
std::vector<std::pair<std::string, int>> SaveLoadManager::leaderboard;

// ============================================================
//                      保存游戏
// ============================================================

/**
 * @brief 保存游戏到二进制文件
 * 
 * 文件格式：
 * 1. 名称长度(uint32_t) + 名称字符数组
 * 2. 基础属性：hp, maxHp, mp, maxMp, level, exp, atk, def, spd, gold
 * 3. 背包：物品数量(uint32_t) + 每个物品(类型+参数)
 */
void SaveLoadManager::SaveGame(const Player& player, const std::string& filename)
{
    // 打开文件（二进制写入模式）
    std::ofstream outFile(filename, std::ios::binary);
    
    if (!outFile.is_open())
    {
        std::cout << "错误：无法打开存档文件 " << filename << " 进行写入！" << std::endl;
        return;
    }

    // ========== 保存玩家名称 ==========
    std::string name = player.GetName();
    uint32_t nameLength = static_cast<uint32_t>(name.size());
    
    // 写入名称长度
    outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
    
    // 写入名称字符数组
    outFile.write(name.c_str(), nameLength);

    // ========== 保存基础属性 ==========
    int hp = player.GetHp();
    int maxHp = player.GetMaxHp();
    int mp = player.GetMp();
    int maxMp = player.GetMaxMp();
    int level = player.GetLevel();
    int exp = player.GetExperience();
    int atk = player.GetAttackPower();
    int def = player.GetDefend();
    int spd = player.GetSpeed();
    int gold = player.GetGold();

    // 写入各属性
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
    
    // 写入物品数量
    outFile.write(reinterpret_cast<const char*>(&itemCount), sizeof(itemCount));

    // 遍历每个物品并保存
    for (uint32_t i = 0; i < itemCount; ++i)
    {
        Item* item = inv.GetItem(static_cast<int>(i));
        
        if (item == nullptr)
        {
            // 写入无效类型标识
            uint8_t itemType = 0;
            outFile.write(reinterpret_cast<const char*>(&itemType), sizeof(itemType));
            continue;
        }

        // 判断物品类型并写入
        Potion* potion = dynamic_cast<Potion*>(item);
        Weapon* weapon = dynamic_cast<Weapon*>(item);

        if (potion)
        {
            // 类型标识：1 = Potion
            uint8_t itemType = 1;
            outFile.write(reinterpret_cast<const char*>(&itemType), sizeof(itemType));
            
            // 写入药水类型
            uint8_t potionType = static_cast<uint8_t>(potion->GetType());
            outFile.write(reinterpret_cast<const char*>(&potionType), sizeof(potionType));
            
            // 写入效果数值
            int amount = potion->GetAmount();
            outFile.write(reinterpret_cast<const char*>(&amount), sizeof(amount));
            
            // 写入名称
            std::string name = potion->GetName();
            uint32_t nameLen = static_cast<uint32_t>(name.size());
            outFile.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
            outFile.write(name.c_str(), nameLen);
            
            // 写入描述
            std::string desc = potion->GetDescription();
            uint32_t descLen = static_cast<uint32_t>(desc.size());
            outFile.write(reinterpret_cast<const char*>(&descLen), sizeof(descLen));
            outFile.write(desc.c_str(), descLen);
        }
        else if (weapon)
        {
            // 类型标识：2 = Weapon
            uint8_t itemType = 2;
            outFile.write(reinterpret_cast<const char*>(&itemType), sizeof(itemType));
            
            // 写入武器类型
            uint8_t weaponType = static_cast<uint8_t>(weapon->GetType());
            outFile.write(reinterpret_cast<const char*>(&weaponType), sizeof(weaponType));
            
            // 写入属性加成
            int attack = weapon->GetAttackBonus();
            int agility = weapon->GetAgilityBonus();
            int speed = weapon->GetSpeedBonus();
            int defense = weapon->GetDefenseBonus();
            outFile.write(reinterpret_cast<const char*>(&attack), sizeof(attack));
            outFile.write(reinterpret_cast<const char*>(&agility), sizeof(agility));
            outFile.write(reinterpret_cast<const char*>(&speed), sizeof(speed));
            outFile.write(reinterpret_cast<const char*>(&defense), sizeof(defense));
            
            // 写入名称
            std::string name = weapon->GetName();
            uint32_t nameLen = static_cast<uint32_t>(name.size());
            outFile.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
            outFile.write(name.c_str(), nameLen);
            
            // 写入描述
            std::string desc = weapon->GetDescription();
            uint32_t descLen = static_cast<uint32_t>(desc.size());
            outFile.write(reinterpret_cast<const char*>(&descLen), sizeof(descLen));
            outFile.write(desc.c_str(), descLen);
        }
        else
        {
            // 未知类型
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

/**
 * @brief 从二进制文件加载游戏
 * 
 * 按照保存的顺序读取数据，重建玩家对象。
 * 处理文件不存在或格式错误的情况。
 */
bool SaveLoadManager::LoadGame(Player& player, const std::string& filename)
{
    // 打开文件（二进制读取模式）
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

    // 读取名称字符数组
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
    // 使用 setter 方法恢复所有属性
    player.SetHp(hp);
    player.SetMaxHp(maxHp);
    player.SetMp(mp);
    player.SetMaxMp(maxMp);
    player.SetLevel(level);
    player.SetExperience(exp);
    player.SetAttackPower(atk);
    player.SetDefend(def);
    player.SetSpeed(spd);
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
            
            // 读取名称
            uint32_t nameLen;
            inFile.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            std::string name(nameLen, '\0');
            inFile.read(&name[0], nameLen);
            
            // 读取描述
            uint32_t descLen;
            inFile.read(reinterpret_cast<char*>(&descLen), sizeof(descLen));
            std::string desc(descLen, '\0');
            inFile.read(&desc[0], descLen);
            
            // 创建新药水并加入背包
            Item* potion = new Potion(amount, static_cast<PotionType>(potionType), name, desc);
            player.inventory.AddItem(potion);
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
            
            // 读取名称
            uint32_t nameLen;
            inFile.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            std::string name(nameLen, '\0');
            inFile.read(&name[0], nameLen);
            
            // 读取描述
            uint32_t descLen;
            inFile.read(reinterpret_cast<char*>(&descLen), sizeof(descLen));
            std::string desc(descLen, '\0');
            inFile.read(&desc[0], descLen);
            
            // 创建新武器并加入背包
            Item* weapon = new Weapon(attack, agility, speed, defense, static_cast<WeaponType>(weaponType), name, desc);
            player.inventory.AddItem(weapon);
        }
        else
        {
            // 未知类型，跳过
            std::cout << "警告：发现未知物品类型，已跳过。" << std::endl;
        }
    }

    inFile.close();
    std::cout << "游戏已从 " << filename << " 加载成功！" << std::endl;
    return true;
}

// ============================================================
//                      添加分数
// ============================================================

/**
 * @brief 添加分数到排行榜
 * 
 * 插入新记录，按分数降序排序，保留前10名。
 */
void SaveLoadManager::AddScore(const std::string& playerName, int score)
{
    // 添加新记录
    leaderboard.push_back(std::make_pair(playerName, score));
    
    // 按分数降序排序
    std::sort(leaderboard.begin(), leaderboard.end(),
        [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b)
        {
            return a.second > b.second;  // 分数高的排前面
        });
    
    // 保留前10名
    if (leaderboard.size() > 10)
    {
        leaderboard.resize(10);
    }
    
    // 自动保存排行榜
    SaveLeaderboard();
    
    std::cout << "新纪录已添加到排行榜！" << std::endl;
}

// ============================================================
//                      显示排行榜
// ============================================================

/**
 * @brief 显示排行榜内容
 */
void SaveLoadManager::ShowLeaderboard()
{
    std::cout << "\n===== 排行榜 =====" << std::endl;
    
    if (leaderboard.empty())
    {
        std::cout << "排行榜暂无记录" << std::endl;
        std::cout << "==================" << std::endl;
        return;
    }
    
    std::cout << "排名  玩家名称          分数" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    
    for (size_t i = 0; i < leaderboard.size(); ++i)
    {
        std::cout << std::setw(2) << (i + 1) << ".   "
                  << std::setw(15) << std::left << leaderboard[i].first
                  << "  " << std::setw(8) << leaderboard[i].second << std::endl;
    }
    
    std::cout << "==================" << std::endl;
}

// ============================================================
//                      保存排行榜
// ============================================================

/**
 * @brief 保存排行榜到文本文件
 * 
 * 格式：每行 "名字 分数"
 */
void SaveLoadManager::SaveLeaderboard(const std::string& filename)
{
    std::ofstream outFile(filename);
    
    if (!outFile.is_open())
    {
        std::cout << "错误：无法保存排行榜文件！" << std::endl;
        return;
    }
    
    for (const auto& entry : leaderboard)
    {
        outFile << entry.first << " " << entry.second << std::endl;
    }
    
    outFile.close();
}

// ============================================================
//                      加载排行榜
// ============================================================

/**
 * @brief 从文本文件加载排行榜
 */
void SaveLoadManager::LoadLeaderboard(const std::string& filename)
{
    std::ifstream inFile(filename);
    
    if (!inFile.is_open())
    {
        // 文件不存在，排行榜保持为空
        return;
    }
    
    leaderboard.clear();
    
    std::string name;
    int score;
    
    while (inFile >> name >> score)
    {
        leaderboard.push_back(std::make_pair(name, score));
    }
    
    // 按分数降序排序
    std::sort(leaderboard.begin(), leaderboard.end(),
        [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b)
        {
            return a.second > b.second;
        });
    
    // 保留前10名
    if (leaderboard.size() > 10)
    {
        leaderboard.resize(10);
    }
    
    inFile.close();
}