#pragma once
#include <string>
#include <vector>
#include <utility>

class Player;

/**
 * @brief 存档、读档和排行榜管理类
 * 
 * 使用静态方法提供游戏存档、读档和排行榜功能。
 * 存档采用二进制格式，排行榜采用文本格式。
 */
class SaveLoadManager
{
private:
    // 排行榜数据（名字，分数）
    static std::vector<std::pair<std::string, int>> leaderboard;

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

    // ============================================================
    //                      排行榜功能
    // ============================================================

    /**
     * @brief 添加分数到排行榜
     * 
     * 插入新记录，按分数降序排序，保留前10名。
     * 然后自动保存排行榜到文件。
     * 
     * @param playerName 玩家名称
     * @param score 分数（经验值）
     */
    static void AddScore(const std::string& playerName, int score);

    /**
     * @brief 显示排行榜
     * 
     * 按排名输出排行榜内容。
     */
    static void ShowLeaderboard();

    /**
     * @brief 保存排行榜到文件
     * 
     * 将排行榜保存为文本文件，每行格式为 "名字 分数"。
     * 
     * @param filename 文件名，默认为 "leaderboard.txt"
     */
    static void SaveLeaderboard(const std::string& filename = "leaderboard.txt");

    /**
     * @brief 从文件加载排行榜
     * 
     * 读取文本文件，填充排行榜并排序。
     * 如果文件不存在，排行榜保持为空。
     * 
     * @param filename 文件名，默认为 "leaderboard.txt"
     */
    static void LoadLeaderboard(const std::string& filename = "leaderboard.txt");
};