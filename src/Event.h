#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <vector>
#include "raylib.h"

using namespace std;

// 前向声明
struct GameContext;

/**
 * @brief 游戏事件类型
 */
typedef enum{
    NONE = 0,   // 占位：不执行任何逻辑
    DIALOGUE,   // 对话事件
    TELEPORT,   // 传送事件
    COMBAT,     // 战斗事件
    RECOVER,    // 恢复事件
    TELEPORT_WITHOUT_LOADING, // 传送但不加载新地图（仅更新位置）
    HIDDEN
}EventType;

/**
 * @brief 事件触发方式
 */
typedef enum{
    ON_NONE = 0,     // 未设置触发条件
    ON_INTERACT,     // 玩家按下交互键
    ON_ENTER_ZONE,   // 玩家进入触发区域
    ON_AUTO_START    // 地图加载后自动触发
}EventTriggerType;


//1. 对话专用数据
struct EventData_Dialogue{
    string scriptPath; // 对话剧本路径，交由 Dialogue 模块解析
};

//2. 传送门专用数据
struct EventData_Portal{
    string targetMap;        // 目标地图路径
    Vector2 targetPosition;  // 目标坐标（若 0,0 则使用 bounds 中心）
    string targetSpawnPoint; // 目标出生点，后续解析为坐标
    int facingDir;           // 目标朝向：参考 Player 枚举
    Rectangle bounds;        // 触发区域，用作默认传送位置

};
//TODO NPC 数据

struct GameEvent{
    EventTriggerType triggerType; // 触发条件：npc/zone/auto
    string triggerValue;          // 触发附加参数（保留字段）
    string scriptPath;            // 通用脚本路径（对话事件使用）
    Rectangle area;               // 触发区域（世界坐标）
    bool isOneShot;               // 是否为一次性事件
    bool isTrigged;               // 防重复触发的运行时标记
    EventType eventType;          // 具体的事件类型


    //各事件所需数据
    vector<EventData_Dialogue>  dialogue;
    vector<EventData_Portal> portal;
    //EventData_Combat;
    //EventData_NPC;
};

/**
 * @brief 遍历并执行所有满足条件的事件
 */
void ExecuteEvents(GameContext &ctx);

/**
 * @brief 判断玩家是否满足指定事件的触发条件
 * @param ctx  全局上下文，包含玩家位置等信息
 * @param area 事件触发区域
 * @param type 触发方式（交互、进入区域等）
 * @return 成功触发时返回对应的触发枚举，否则返回 ON_NONE
 */
EventTriggerType CheckEvents(GameContext &ctx, Rectangle area, EventTriggerType type);
#endif // EVENT_H

