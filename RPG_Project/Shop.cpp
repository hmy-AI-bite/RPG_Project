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
//                      显示商店
// ============================================================

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
            int price = GetPrice(forSaleItems[i].get());
            std::string itemInfo = "[" + std::to_string(i) + "] " + forSaleItems[i]->GetName()
                                 + " - " + forSaleItems[i]->GetDescription();

            // 如果是武器，显示具体属性数值
            if (const Weapon* weapon = dynamic_cast<const Weapon*>(forSaleItems[i].get()))
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

int Shop::GetPrice(const Item* item) const
{
    if (const Potion* potion = dynamic_cast<const Potion*>(item))
    {
        switch (potion->GetType())
        {
        case PotionType::HEALTH:
            if (potion->GetAmount() <= 20) return 30;
            if (potion->GetAmount() <= 50) return 60;
            if (potion->GetAmount() <= 100) return 100;
            return 180;

        case PotionType::ATTACK:
            return potion->GetAmount() * 10;

        case PotionType::AGILITY:
            return potion->GetAmount() * 12;

        case PotionType::SPEED:
            return potion->GetAmount() * 12;

        case PotionType::DEFENSE:
            return potion->GetAmount() * 15;

        default:
            return 0;
        }
    }
    else if (const Weapon* weapon = dynamic_cast<const Weapon*>(item))
    {
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

        return std::max(basePrice, 10);
    }
    return 0;
}

// ============================================================
//                      购买道具
// ============================================================

void Shop::BuyItem(Player& buyer, int index)
{
    if (index < 0 || index >= static_cast<int>(forSaleItems.size()))
    {
        std::cout << "错误：无效的商品索引！" << std::endl;
        return;
    }

    Item* item = forSaleItems[index].get();
    int price = GetPrice(item);

    if (buyer.gold < price)
    {
        std::cout << "金币不足！需要 " << price << " 金币，当前只有 " << buyer.gold << " 金币。" << std::endl;
        return;
    }

    // 扣除金币
    buyer.gold -= price;
    std::cout << "花费了 " << price << " 金币。" << std::endl;

    // 创建道具副本并添加到玩家背包
    auto clonedItem = item->Clone();
    buyer.inventory.AddItem(std::move(clonedItem));

    std::cout << "购买成功！已获得 " << item->GetName() << std::endl;
}

// ============================================================
//                      出售道具
// ============================================================

void Shop::SellItem(Player& seller, int inventoryIndex)
{
    Inventory& inv = seller.inventory;

    if (inv.GetItemCount() == 0)
    {
        std::cout << "背包是空的，无法出售！" << std::endl;
        return;
    }

    if (inventoryIndex < 0 || inventoryIndex >= static_cast<int>(inv.GetItemCount()))
    {
        std::cout << "错误：无效的背包索引！" << std::endl;
        return;
    }

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

    // 从背包中移除道具
    inv.RemoveItem(inventoryIndex);
}
