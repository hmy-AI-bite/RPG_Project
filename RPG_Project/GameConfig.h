#pragma once

// ================================================================
//  GameConfig - 游戏配置表
//  所有可调优的数值集中管理，便于平衡性调整
// ================================================================

namespace GameConfig
{
    // ========== 行动条系统 ==========
    constexpr int MAX_GAUGE = 500;          // 行动条上限
    constexpr int ATB_TICK_MS = 50;         // ATB 每帧时间（毫秒）

    // ========== 升级系统 ==========
    constexpr int EXP_PER_LEVEL_BASE = 100; // 每级基础经验需求（level * EXP_PER_LEVEL_BASE）
    constexpr int AP_PER_LEVEL = 5;         // 每次升级获得的属性点

    // ========== 暴击系统 ==========
    constexpr double MAX_CRIT_CHANCE = 0.30;  // 最大暴击率
    constexpr double CRIT_MULTIPLIER = 1.5;   // 暴击伤害倍率
    constexpr double AGI_TO_CRIT_RATIO = 0.001; // 灵巧转暴击率比例（agi / 1000）

    // ========== 属性克制 ==========
    constexpr double ELEMENT_ADVANTAGE = 1.5;   // 克制倍率
    constexpr double ELEMENT_DISADVANTAGE = 0.75; // 被克制倍率
    constexpr double ELEMENT_NEUTRAL = 1.0;     // 中立倍率

    // ========== 伤害计算 ==========
    constexpr double PHYSICAL_ATK_MULT = 2.0;      // 物理伤害攻击力乘数
    constexpr double PHYSICAL_DEF_MULT = 1.5;      // 物理伤害防御力乘数
    constexpr double PHYSICAL_MIN_RATIO = 0.2;     // 物理保底伤害比例
    constexpr double MAGICAL_COEFFICIENT = 1.6;    // 魔法伤害系数
    constexpr double MAGICAL_DEF_SCALE = 200.0;    // 魔法防御缩放因子
    constexpr double MAGICAL_MIN_RATIO = 0.2;      // 魔法保底伤害比例
    constexpr double TRUE_DMG_ATK_RATIO = 0.5;     // 真实伤害攻击力比例
    constexpr double LEVEL_DIFF_BONUS = 0.025;     // 等级差加成（每级2.5%）
    constexpr double DMG_VARIANCE_MIN = 0.9;       // 伤害随机浮动最小值
    constexpr double DMG_VARIANCE_MAX = 1.1;       // 伤害随机浮动最大值

    // ========== 属性点分配效果 ==========
    namespace AttributeGain
    {
        // 战士（默认 Player）
        namespace Warrior
        {
            constexpr int HP = 8;
            constexpr int MP = 5;
            constexpr int ATK = 2;
            constexpr int DEF = 1;
            constexpr int SPD = 1;
            constexpr int AGI = 1;
            constexpr int SDEF = 1;
        }
        // 勇者
        namespace Braver
        {
            constexpr int HP = 6;
            constexpr int MP = 4;
            constexpr int ATK = 3;
            constexpr int DEF = 1;
            constexpr int SPD = 2;
            constexpr int AGI = 2;
            constexpr int SDEF = 1;
        }
        // 法师
        namespace Mage
        {
            constexpr int HP = 4;
            constexpr int MP = 7;
            constexpr int ATK = 1;
            constexpr int DEF = 1;
            constexpr int SPD = 2;
            constexpr int AGI = 2;
            constexpr int SDEF = 2;
            constexpr int SP = 3; // 法术强度
        }
        // 守护骑士
        namespace Guardian
        {
            constexpr int HP = 12;
            constexpr int MP = 5;
            constexpr int ATK = 1;
            constexpr int DEF = 3;
            constexpr int SPD = 1;
            constexpr int AGI = 1;
            constexpr int SDEF = 2;
        }
    }

    // ========== 怪物属性公式参数 ==========
    namespace MonsterStats
    {
        // 史莱姆
        namespace Slime
        {
            constexpr int BASE_HP = 30;
            constexpr int HP_PER_LV = 5;
            constexpr int BASE_ATK = 5;
            constexpr int ATK_PER_LV = 2;
            constexpr int BASE_DEF = 1;
            constexpr int DEF_PER_LV = 1;
            constexpr int BASE_AGI = 20;
            constexpr int AGI_PER_LV = 2;
            constexpr int SPD = 80;
            constexpr int SDEF = 2;
            constexpr int EXP_BASE = 20;
            constexpr int EXP_PER_LV = 5;
            constexpr int GOLD_BASE = 10;
            constexpr int GOLD_PER_LV = 3;
            constexpr double DROP_RATE = 0.3;
            constexpr double HEAL_ON_SPLIT = 0.25; // 分裂后恢复最大血量比例
        }

        // 火焰蜥蜴
        namespace FireLizard
        {
            constexpr int BASE_HP = 35;
            constexpr int HP_PER_LV = 4;
            constexpr int BASE_ATK = 7;
            constexpr int ATK_PER_LV = 3;
            constexpr int BASE_DEF = 1;
            constexpr int DEF_PER_LV = 1;
            constexpr int BASE_AGI = 15;
            constexpr int AGI_PER_LV = 2;
            constexpr int SPD = 90;
            constexpr int SDEF = 3;
            constexpr int EXP_BASE = 25;
            constexpr int EXP_PER_LV = 5;
            constexpr int GOLD_BASE = 15;
            constexpr int GOLD_PER_LV = 3;
            constexpr double DROP_RATE = 0.25;
        }

        // 风精灵
        namespace WindSprite
        {
            constexpr int BASE_HP = 28;
            constexpr int HP_PER_LV = 3;
            constexpr int BASE_ATK = 6;
            constexpr int ATK_PER_LV = 2;
            constexpr int BASE_DEF = 1;
            constexpr int DEF_PER_LV = 1;
            constexpr int BASE_AGI = 25;
            constexpr int AGI_PER_LV = 3;
            constexpr int SPD = 130;
            constexpr int SDEF = 4;
            constexpr int EXP_BASE = 30;
            constexpr int EXP_PER_LV = 5;
            constexpr int GOLD_BASE = 12;
            constexpr int GOLD_PER_LV = 2;
            constexpr double DROP_RATE = 0.3;
        }
    }
}
