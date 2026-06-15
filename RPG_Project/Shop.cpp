#include "Shop.h"
#include "Item.h"
#include "Character.h"
#include "Inventory.h"
#include <iostream>
#include <iomanip>
#include <string>

// ============================================================
//                      构造函数
// ============================================================

/**
 * @brief 构造函数实现
 * 
 * 初始化商店商品，添加各种道具到货架：
 * - 药水类：治疗药水、力量药水、敏捷药水、迅捷药水、护盾药水
 * - 武器类：剑、匕首、法杖、弓、盾牌
 */
Shop::Shop()
{
    // === 药水类（6种）===
    forSaleItems.push_back(Potion::CreateSmallHealthPotion());
    forSaleItems.push_back(Potion::CreateMediumHealthPotion());
    forSaleItems.push_back(Potion::CreateStrengthPotion());
    forSaleItems.push_back(Potion::CreateAgilityPotion());
    forSaleItems.push_back(Potion::CreateSpeedPotion());
    forSaleItems.push_back(Potion::CreateDefensePotion());
    
    // === 武器类（6种）===
    forSaleItems.push_back(Weapon::CreateLongSword());
    forSaleItems.push_back(Weapon::CreateDagger());
    forSaleItems.push_back(Weapon::CreateBow());
    forSaleItems.push_back(Weapon::CreateStaff());
    forSaleItems.push_back(Weapon::CreateGreatSword());
    forSaleItems.push_back(Weapon::CreateShield());
}

// ============================================================
//                      析构函数
// ============================================================

/**
 * @brief 析构函数实现
 * 
 * 遍历释放商店中所有待售道具的内存。
 */
Shop::~Shop()
{
    for (Item* item : forSaleItems)
    {
        delete item;
    }
    forSaleItems.clear();
}

// ============================================================
//                      显示商店
// ============================================================

/**
 * @brief 显示商店商品列表
 * 
 * 遍历输出所有待售商品的信息。
 */
void Shop::ShowShop() const
{
    std::cout << "\n========================================================" << std::endl;
    std::cout << "              商店" << std::endl;
    std::cout << "========================================================" << std::endl;
    std::cout << "欢迎光临商店！" << std::endl;
    
    if (forSaleItems.empty())
    {
        std::cout << "商店暂时没有商品。" << std::endl;
    }
    else
    {
        for (size_t i = 0; i < forSaleItems.size(); ++i)
        {
            int price = GetPrice(forSaleItems[i]);
            std::string itemInfo = "[" + std::to_string(i) + "] " + forSaleItems[i]->GetName() 
                                 + " - " + forSaleItems[i]->GetDescription();
            
            // 如果是武器，显示具体属性数值
            if (const Weapon* weapon = dynamic_cast<const Weapon*>(forSaleItems[i]))
            {
                itemInfo += " 属性: ";
                
                // 总是显示攻击力
                itemInfo += "攻击 " + std::string(weapon->GetAttackBonus() >= 0 ? "+" : "") + std::to_string(weapon->GetAttackBonus());
                
                if (weapon->GetAgilityBonus() != 0)
                {
                    itemInfo += ", 灵巧 " + std::string(weapon->GetAgilityBonus() > 0 ? "+" : "") + std::to_string(weapon->GetAgilityBonus());
                }
                if (weapon->GetSpeedBonus() != 0)
                {
                    itemInfo += ", 速度 " + std::string(weapon->GetSpeedBonus() > 0 ? "+" : "") + std::to_string(weapon->GetSpeedBonus());
                }
                if (weapon->GetDefenseBonus() != 0)
                {
                    itemInfo += ", 法防 " + std::string(weapon->GetDefenseBonus() > 0 ? "+" : "") + std::to_string(weapon->GetDefenseBonus());
                }
            }
            
            itemInfo += " (价格: " + std::to_string(price) + " 金币)";
            
            std::cout << itemInfo << std::endl;
        }
    }
    
    std::cout << "========================================================" << std::endl;
}

// ============================================================
//                      获取价格
// ============================================================

/**
 * @brief 获取道具价格
 * 
 * 根据道具类型返回固定价格：
 * - 药水：30 金币
 * - 武器：80 金币
 * 
 * @param item 道具指针
 * @return 道具价格
 */
int Shop::GetPrice(const Item* item) const
{
    // 使用 dynamic_cast 检查对象类型
    if (const Potion* potion = dynamic_cast<const Potion*>(item))
    {
        // 根据药水类型和效果定价
        switch (potion->GetType())
        {
        case PotionType::HEALTH:
            if (potion->GetAmount() <= 20) return 30;
            if (potion->GetAmount() <= 50) return 60;
            if (potion->GetAmount() <= 100) return 100;
            return 180;
            
        case PotionType::ATTACK:
            return potion->GetAmount() * 10;  // 力量药水：每点攻击10金币
            
        case PotionType::AGILITY:
            return potion->GetAmount() * 12;  // 敏捷药水：每点灵巧12金币
            
        case PotionType::SPEED:
            return potion->GetAmount() * 12;  // 迅捷药水：每点速度12金币
            
        case PotionType::DEFENSE:
            return potion->GetAmount() * 15;  // 护盾药水：每点防御15金币
            
        default:
            return 0;
        }
    }
    else if (const Weapon* weapon = dynamic_cast<const Weapon*>(item))
    {
        // 根据武器类型和属性计算价格
        int basePrice = 0;
        
        switch (weapon->GetType())
        {
        case WeaponType::SWORD:
            basePrice = weapon->GetAttackBonus() * 10;
            break;
            
        case WeaponType::DAGGER:
            basePrice = weapon->GetAttackBonus() * 8 + weapon->GetAgilityBonus() * 6 + weapon->GetSpeedBonus() * 4;
            break;
            
        case WeaponType::STAFF:
            basePrice = weapon->GetAttackBonus() * 12 + weapon->GetAgilityBonus() * 6 + weapon->GetDefenseBonus() * 10;
            break;
            
        case WeaponType::BOW:
            basePrice = weapon->GetAttackBonus() * 8 + weapon->GetAgilityBonus() * 4 + weapon->GetSpeedBonus() * 6;
            break;
            
        case WeaponType::GREAT_SWORD:
            basePrice = weapon->GetAttackBonus() * 10;
            break;
            
        case WeaponType::SHIELD:
            basePrice = weapon->GetDefenseBonus() * 15;
            break;
            
        default:
            return 0;
        }
        
        // 处理负面属性（降低价格）
        if (weapon->GetAttackBonus() < 0) basePrice += weapon->GetAttackBonus() * 5;
        if (weapon->GetAgilityBonus() < 0) basePrice += weapon->GetAgilityBonus() * 3;
        if (weapon->GetSpeedBonus() < 0) basePrice += weapon->GetSpeedBonus() * 4;
        
        return std::max(basePrice, 10);  // 最低价格10金币
    }
    return 0;  // 未知类型返回 0
}

// ============================================================
//                      购买道具
// ============================================================

/**
 * @brief 购买道具
 * 
 * 检查索引有效性、判断金币是否足够，
 * 如果足够则扣除金币并创建道具副本添加到玩家背包。
 * 
 * @param buyer 购买道具的玩家引用
 * @param index 要购买的商品索引
 */
void Shop::BuyItem(Player& buyer, int index)
{
    // 检查索引是否有效
    if (index < 0 || index >= static_cast<int>(forSaleItems.size()))
    {
        std::cout << "错误：无效的商品索引！" << std::endl;
        return;
    }

    // 获取道具和价格
    Item* item = forSaleItems[index];
    int price = GetPrice(item);

    // 检查玩家金币是否足够（假设 Player 有 gold 成员）
    if (buyer.gold < price)
    {
        std::cout << "金币不足！需要 " << price << " 金币，当前只有 " << buyer.gold << " 金币。" << std::endl;
        return;
    }

    // 扣除金币
    buyer.gold -= price;
    std::cout << "花费了 " << price << " 金币。" << std::endl;

    // 创建道具副本（深拷贝）
    Item* clonedItem = item->Clone();

    // 添加到玩家背包（假设 Player 有 inventory 成员）
    buyer.inventory.AddItem(clonedItem);

    std::cout << "购买成功！已获得 " << item->GetName() << std::endl;
}

// ============================================================
//                      出售道具
// ============================================================

/**
 * @brief 出售道具
 * 
 * 玩家将背包中的道具出售给商店，获得半价金币。
 * 道具从背包中移除并释放内存。
 * 
 * @param seller 出售道具的玩家引用
 * @param inventoryIndex 玩家背包中道具的索引
 */
void Shop::SellItem(Player& seller, int inventoryIndex)
{
    // 获取玩家背包
    Inventory& inv = seller.inventory;

    // 检查背包中是否有道具
    if (inv.GetItemCount() == 0)
    {
        std::cout << "背包是空的，无法出售！" << std::endl;
        return;
    }

    // 检查索引是否有效
    if (inventoryIndex < 0 || inventoryIndex >= static_cast<int>(inv.GetItemCount()))
    {
        std::cout << "错误：无效的背包索引！" << std::endl;
        return;
    }

    // 获取背包中该位置的道具
    Item* item = inv.GetItem(inventoryIndex);
    
    if (item == nullptr)
    {
        std::cout << "错误：无法获取道具！" << std::endl;
        return;
    }

    // 计算出售价格（半价）
    int price = GetPrice(item);
    int sellPrice = price / 2;

    // 增加玩家金币
    seller.gold += sellPrice;
    std::cout << "出售 " << item->GetName() << " 获得了 " << sellPrice << " 金币。" << std::endl;

    // 从背包中移除道具（RemoveItem 会 delete 道具）
    inv.RemoveItem(inventoryIndex);
}