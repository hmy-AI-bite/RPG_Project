#pragma once
#include <vector>
#include <string>

class Item;
class Player;

/**
 * @brief 商店系统类
 * 
 * 管理商店的待售商品，提供购买和出售功能。
 * 商店拥有自己的商品列表，玩家购买时创建副本，出售时删除原道具。
 */
class Shop
{
private:
    std::vector<Item*> forSaleItems;  // 待售道具列表

public:
    /**
     * @brief 构造函数
     * 
     * 初始化商店商品，添加默认的道具到货架。
     */
    Shop();

    /**
     * @brief 析构函数
     * 
     * 释放商店中所有待售道具的内存。
     */
    ~Shop();

    /**
     * @brief 显示商店商品列表
     * 
     * 遍历输出商店中所有商品的信息，包括序号、名称、描述和价格。
     */
    void ShowShop() const;

    /**
     * @brief 购买道具
     * 
     * 玩家从商店购买道具，扣除相应金币，创建道具副本添加到玩家背包。
     * 
     * @param buyer 购买道具的玩家引用
     * @param index 要购买的商品索引
     */
    void BuyItem(Player& buyer, int index);

    /**
     * @brief 出售道具
     * 
     * 玩家将背包中的道具出售给商店，获得半价金币。
     * 道具从背包中移除并释放内存。
     * 
     * @param seller 出售道具的玩家引用
     * @param inventoryIndex 玩家背包中道具的索引
     */
    void SellItem(Player& seller, int inventoryIndex);

    /**
     * @brief 获取道具价格
     * 
     * 根据道具类型返回对应的销售价格。
     * 
     * @param item 道具指针
     * @return 道具价格
     */
    int GetPrice(const Item* item) const;
};