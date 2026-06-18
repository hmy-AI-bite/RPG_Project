#pragma once
#include <vector>
#include <memory>
#include <string>

class Item;
class Player;
class Potion;
class Weapon;

/**
 * @brief 商店系统类
 * 
 * 管理商店的待售商品，提供购买和出售功能。
 * 使用 unique_ptr 自动管理商品内存。
 */
class Shop
{
private:
    std::vector<std::unique_ptr<Item>> forSaleItems;  // 待售道具列表

public:
    Shop();
    ~Shop() = default;

    /**
     * @brief 显示商店商品列表
     */
    void ShowShop() const;

    /**
     * @brief 购买道具
     * @param buyer 购买道具的玩家引用
     * @param index 要购买的商品索引
     */
    void BuyItem(Player& buyer, int index);

    /**
     * @brief 出售道具
     * @param seller 出售道具的玩家引用
     * @param inventoryIndex 玩家背包中道具的索引
     */
    void SellItem(Player& seller, int inventoryIndex);

    /**
     * @brief 获取道具价格
     * @param item 道具指针
     * @return 道具价格
     */
    int GetPrice(const Item* item) const;
};