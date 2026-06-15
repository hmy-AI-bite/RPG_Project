#pragma once
#include <string>

class Player;

/**
 * @brief 存档和读档管理类
 * 
 * 使用静态方法提供游戏存档和读档功能。
 * 存档采用二进制格式。
 */
class SaveLoadManager
{
public:
    // ============================================================
    //                      存档/读档功能
    // ============================================================

    /**
     * @brief 保存游戏
     * 
     * 将玩家数据保存到二进制文件中。
     * 保存内容：名称、基础属性、背包物品。
     * 
     * @param player 要保存的玩家对象
     * @param filename 存档文件名，默认为 "save.dat"
     */
    static void SaveGame(const Player& player, const std::string& filename = "save.dat");

    /**
     * @brief 加载游戏
     * 
     * 从二进制文件读取玩家数据，重建玩家对象。
     * 如果文件不存在或格式错误，输出错误信息但不崩溃。
     * 
     * @param player 要加载的玩家对象（会被覆盖）
     * @param filename 存档文件名，默认为 "save.dat"
     * @return 成功返回 true，失败返回 false
     */
    static bool LoadGame(Player& player, const std::string& filename = "save.dat");
};