#pragma once
#include <vector>
#include <string>

class Item;
class Character;

/**
 * @brief 背包系统类
 * 
 * 管理角色所持有的道具，支持添加、移除、使用道具等操作。
 * 使用动态数组存储道具指针，析构时自动释放所有道具对象。
 */
class Inventory
{
private:
    std::vector<Item*> items;  // 存储道具指针的动态数组

public:
    /**
     * @brief 默认构造函数
     */
    Inventory() = default;

    /**
     * @brief 析构函数
     * 
     * 遍历背包中的所有道具，释放每个道具对象的内存。
     */
    ~Inventory();

    /**
     * @brief 拷贝构造函数（删除）
     * 
     * 禁止浅拷贝，避免多个 Inventory 共享同一批道具指针导致重复释放。
     */
    Inventory(const Inventory&) = delete;

    /**
     * @brief 赋值运算符（删除）
     * 
     * 禁止赋值操作，避免浅拷贝问题。
     */
    Inventory& operator=(const Inventory&) = delete;

    /**
     * @brief 添加道具到背包
     * @param item 要添加的道具指针（必须是动态分配的对象）
     */
    void AddItem(Item* item);

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
     * 
     * 调用道具的 Use 方法，然后自动从背包中移除该道具。
     */
    bool UseItem(int index, Character* user);

    /**
     * @brief 显示背包内容
     * 
     * 遍历输出背包中所有道具的索引、名称和描述。
     */
    void ShowInventory() const;

    /**
     * @brief 获取背包中道具数量
     * @return 道具数量
     */
    size_t GetItemCount() const;

    /**
     * @brief 获取指定索引的道具指针
     * @param index 道具索引
     * @return 道具指针，无效索引返回 nullptr
     */
    Item* GetItem(int index) const;
};