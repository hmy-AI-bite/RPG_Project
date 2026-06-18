#include "Inventory.h"
#include "Item.h"
#include "Character.h"
#include <iostream>

// ============================================================
//                      添加道具
// ============================================================

void Inventory::AddItem(std::unique_ptr<Item> item)
{
    if (item)
    {
        std::cout << "已获得道具: " << item->GetName() << std::endl;
        items.push_back(std::move(item));
    }
}

// ============================================================
//                      移除道具
// ============================================================

bool Inventory::RemoveItem(int index)
{
    if (index < 0 || index >= static_cast<int>(items.size()))
    {
        std::cout << "错误：无效的道具索引！" << std::endl;
        return false;
    }

    // unique_ptr 自动释放内存
    items.erase(items.begin() + index);

    std::cout << "道具已移除" << std::endl;
    return true;
}

// ============================================================
//                      使用道具
// ============================================================

bool Inventory::UseItem(int index, Character* user)
{
    if (index < 0 || index >= static_cast<int>(items.size()))
    {
        std::cout << "错误：无效的道具索引！" << std::endl;
        return false;
    }

    if (user == nullptr)
    {
        std::cout << "错误：无效的角色指针！" << std::endl;
        return false;
    }

    Item* item = items[index].get();

    std::cout << "\n使用道具: " << item->GetName() << std::endl;

    item->Use(user);

    // 使用后自动从背包移除（消耗型道具）
    items.erase(items.begin() + index);

    return true;
}

// ============================================================
//                      显示背包
// ============================================================

void Inventory::ShowInventory() const
{
    std::cout << "\n===== 背包 =====" << std::endl;

    if (items.empty())
    {
        std::cout << "背包是空的" << std::endl;
        std::cout << "================" << std::endl;
        return;
    }

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

size_t Inventory::GetItemCount() const
{
    return items.size();
}

// ============================================================
//                      获取指定道具
// ============================================================

Item* Inventory::GetItem(int index) const
{
    if (index < 0 || index >= static_cast<int>(items.size()))
    {
        return nullptr;
    }
    return items[index].get();
}
