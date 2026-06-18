#pragma once
#include "Character.h"
#include "Monster.h"
#include "GameConfig.h"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <iostream>

// ================================================================
//  ATBTest - ATB 战斗辅助命名空间
//
//  提供 ATB 战斗中的通用工具函数：
//    行动条计算、行动顺序预测、快进、队列收集等
//  被 BattleSystem 和 TestManager 共享使用
// ================================================================
namespace ATBTest {

    const int TICK_MS = 50;
    const int MAX_PREDICT = 3;

    struct PredictEntry {
        std::string name;
        int speed;
        int framesToReady;
    };

    struct ActionEntry {
        bool isPlayer;
        int speed;
        size_t enemyIndex;
    };

    // 计算到满条所需帧数
    inline int FramesToReady(const Character* c) {
        int remaining = MAX_GAUGE - c->GetCurrentGauge();
        if (remaining <= 0) return 0;
        int perFrame = (c->GetSpeed() * TICK_MS) / 100;
        if (perFrame <= 0) return 99999;
        return (remaining + perFrame - 1) / perFrame;
    }

    // 预测接下来 N 个行动者
    inline std::vector<PredictEntry> PredictNextActors(
        const std::vector<Character*>& units, int count = MAX_PREDICT) {
        std::vector<PredictEntry> result;
        for (auto* u : units) {
            if (!u->IsAlive()) continue;
            result.push_back({ u->GetName(), u->GetSpeed(), FramesToReady(u) });
        }
        std::sort(result.begin(), result.end(),
            [](const PredictEntry& a, const PredictEntry& b) {
                if (a.framesToReady != b.framesToReady)
                    return a.framesToReady < b.framesToReady;
                return a.speed > b.speed;
            });
        if ((int)result.size() > count) result.resize(count);
        return result;
    }

    // 显示行动顺序
    inline void ShowActionOrder(const std::vector<PredictEntry>& predictions) {
        std::cout << "[行动顺序] ";
        for (size_t i = 0; i < predictions.size(); ++i) {
            if (i > 0) std::cout << " -> ";
            std::cout << predictions[i].name;
            if (predictions[i].framesToReady > 0)
                std::cout << "(" << predictions[i].framesToReady << "帧)";
            else
                std::cout << "(就绪)";
        }
        std::cout << "\n";
    }

    // 快进到下一个行动者满条
    inline int FastForwardToNextAction(
        Player* player,
        std::vector<std::shared_ptr<Enemy>>& enemies) {
        int framesPassed = 0;
        while (true) {
            if (player->IsAlive() && player->IsReadyToAct()) return framesPassed;
            for (auto& e : enemies)
                if (e->IsAlive() && e->IsReadyToAct()) return framesPassed;
            framesPassed++;
            if (player->IsAlive()) player->UpdateGauge(TICK_MS);
            for (auto& e : enemies)
                if (e->IsAlive()) e->UpdateGauge(TICK_MS);
        }
    }

    // 收集所有满条单位
    inline std::vector<ActionEntry> CollectReadyActors(
        Player* player,
        const std::vector<std::shared_ptr<Enemy>>& enemies) {
        std::vector<ActionEntry> queue;
        if (player->IsAlive() && player->IsReadyToAct())
            queue.push_back({ true, player->GetSpeed(), 0 });
        for (size_t i = 0; i < enemies.size(); ++i)
            if (enemies[i]->IsAlive() && enemies[i]->IsReadyToAct())
                queue.push_back({ false, enemies[i]->GetSpeed(), i });
        std::sort(queue.begin(), queue.end(),
            [](const ActionEntry& a, const ActionEntry& b) {
                return a.speed > b.speed;
            });
        return queue;
    }

    // 构建预测用单位列表
    inline std::vector<Character*> BuildUnitList(
        Player* player,
        const std::vector<std::shared_ptr<Enemy>>& enemies) {
        std::vector<Character*> units;
        if (player->IsAlive()) units.push_back(player);
        for (auto& e : enemies)
            if (e->IsAlive()) units.push_back(e.get());
        return units;
    }
}
