#include "Inventory.h"
#include "Item.h"
#include "Character.h"
#include <iostream>

// ============================================================
//                      Inventory 析构函数
// ============================================================

/**
 * @brief 析构函数实现
 * 
 * 遍历背包中的所有道具指针，逐个释放内存，避免内存泄漏。
 */
Inventory::~Inventory()
{
    // 遍历所有道具并释放内存
    for (Item* item : items)
    {
        delete item;
    }
    // 清空数组（可选，因为对象即将销毁）
    items.clear();
}

// ============================================================
//                      添加道具
// ============================================================

/**
 * @brief 添加道具到背包
 * 
 * 将道具指针添加到动态数组末尾。
 * 注意：传入的 item 必须是通过 new 动态分配的对象，
 * 因为背包析构时会自动 delete 这些对象。
 * 
 * @param item 要添加的道具指针
 */
void Inventory::AddItem(Item* item)
{
    if (item != nullptr)
    {
        items.push_back(item);
        std::cout << "已获得道具: " << item->GetName() << std::endl;
    }
}

// ============================================================
//                      移除道具
// ============================================================

/**
 * @brief 按索引移除道具
 * 
 * 检查索引是否有效，如果有效则释放道具内存并从数组中移除。
 * 
 * @param index 道具索引（从0开始）
 * @return 成功返回 true，失败返回 false
 */
bool Inventory::RemoveItem(int index)
{
    // 检查索引是否有效
    if (index < 0 || index >= static_cast<int>(items.size()))
    {
        std::cout << "错误：无效的道具索引！" << std::endl;
        return false;
    }

    // 释放道具对象内存
    delete items[index];
    
    // 从数组中移除该元素
    items.erase(items.begin() + index);
    
    std::cout << "道具已移除" << std::endl;
    return true;
}

// ============================================================
//                      使用道具
// ============================================================

/**
 * @brief 使用背包中的道具
 * 
 * 先检查索引有效性，然后调用道具的 Use 方法，
 * 使用完成后自动从背包中移除该道具。
 * 
 * @param index 要使用的道具索引
 * @param user 使用道具的角色指针
 * @return 成功返回 true，失败返回 false
 */
bool Inventory::UseItem(int index, Character* user)
{
    // 检查索引是否有效
    if (index < 0 || index >= static_cast<int>(items.size()))
    {
        std::cout << "错误：无效的道具索引！" << std::endl;
        return false;
    }

    // 检查角色指针是否有效
    if (user == nullptr)
    {
        std::cout << "错误：无效的角色指针！" << std::endl;
        return false;
    }

    // 获取道具指针
    Item* item = items[index];
    
    // 输出使用信息
    std::cout << "\n使用道具: " << item->GetName() << std::endl;
    
    // 调用道具的 Use 方法
    item->Use(user);
    
    // 使用后自动从背包移除（消耗型道具）
    delete item;
    items.erase(items.begin() + index);
    
    return true;
}

// ============================================================
//                      显示背包
// ============================================================

/**
 * @brief 显示背包中所有道具
 * 
 * 遍历输出每个道具的索引、名称和描述信息。
 * 如果背包为空，输出提示信息。
 */
void Inventory::ShowInventory() const
{
    std::cout << "\n===== 背包 =====" << std::endl;
    
    if (items.empty())
    {
        std::cout << "背包是空的" << std::endl;
        std::cout << "================" << std::endl;
        return;
    }

    // 遍历所有道具并显示信息
    for (size_t i = 0; i < items.size(); ++i)
    {
        std::cout << "[" << i << "] " << items[i]->GetName() 
                  << " - " << items[i]->GetDescription() << std::endl;
    }
    
    std::cout << "================" << std::endl;
}

// ============================================================
//                      获取道具数量
// ============================================================

/**
 * @brief 获取背包中道具的数量
 * 
 * @return 道具数量
 */
size_t Inventory::GetItemCount() const
{
    return items.size();
}

// ============================================================
//                      获取指定道具
// ============================================================

/**
 * @brief 获取指定索引的道具指针
 * 
 * @param index 道具索引
 * @return 道具指针，无效索引返回 nullptr
 */
Item* Inventory::GetItem(int index) const
{
    // 检查索引是否有效
    if (index < 0 || index >= static_cast<int>(items.size()))
    {
        return nullptr;
    }
    return items[index];
}