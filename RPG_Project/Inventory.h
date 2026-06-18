#pragma once
#include <vector>
#include <memory>
#include <string>

class Item;
class Character;

/**
 * @brief 背包系统类
 * 
 * 管理角色所持有的道具，支持添加、移除、使用道具等操作。
 * 使用 unique_ptr 自动管理道具生命周期，避免内存泄漏。
 */
class Inventory
{
private:
    std::vector<std::unique_ptr<Item>> items;  // 存储道具的动态数组（自动管理内存）

public:
    Inventory() = default;

    /**
     * @brief 析构函数（默认，unique_ptr 自动释放）
     */
    ~Inventory() = default;

    /**
     * @brief 拷贝构造函数（删除）
     */
    Inventory(const Inventory&) = delete;

    /**
     * @brief 赋值运算符（删除）
     */
    Inventory& operator=(const Inventory&) = delete;

    /**
     * @brief 移动构造函数
     */
    Inventory(Inventory&&) = default;

    /**
     * @brief 移动赋值运算符
     */
    Inventory& operator=(Inventory&&) = default;

    /**
     * @brief 添加道具到背包（接管所有权）
     * @param item 要添加的道具指针（必须是动态分配的对象）
     */
    void AddItem(std::unique_ptr<Item> item);

    /**
     * @brief 按索引移除道具
     * @param index 道具在背包中的索引（从0开始）
     * @return 是否成功移除
     */
    bool RemoveItem(int index);

    /**
     * @brief 使用道具
     * @param index 要使用的道具索引
     * @param user 使用道具的角色指针
     * @return 是否成功使用
     */
    bool UseItem(int index, Character* user);

    /**
     * @brief 显示背包内容
     */
    void ShowInventory() const;

    /**
     * @brief 获取背包中道具数量
     */
    size_t GetItemCount() const;

    /**
     * @brief 获取指定索引的道具裸指针（不转移所有权）
     * @param index 道具索引
     * @return 道具指针，无效索引返回 nullptr
     */
    Item* GetItem(int index) const;
};