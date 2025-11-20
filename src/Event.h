#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <vector>
#include "raylib.h"

using namespace std;

// 前向声明
struct GameContext;

typedef enum{
    NONE = 0,
    DIALOGUE, //对话
    TELEPORT, //传送
    COMBAT //战斗
}EventType;

typedef enum{
    ON_INTERACT = 0, //按E
    ON_ENTER_ZONE, //进入触发区域
    ON_AUTO_START //自动触发
}EventTriggerType;


//1. 对话专用数据
struct EventData_Dialogue{
    string scriptPath;//剧本路径
};

//2. 传送门专用数据
struct EventData_Portal{
    string targetMap;//目标地图路径
    Vector2 targetPosition;//目标坐标
    string targetSpawnPoint;//目标出生点
    int facingDir;//目标朝向：见player的枚举
    Rectangle bounds;//触发区域

};
//TODO NPC 数据



struct GameEvent{
    string triggerType;//触发条件：npc/zone/auto
    string triggerValue;//触发值
    string scriptPath;//脚本路径
    Rectangle area;//触发区域
    bool isOneShot;//是否为一次性的
    bool isTrigged;//是否已被触发
    EventType eventType;//见event上面的枚举


    //各事件所需数据
    vector<EventData_Dialogue>  dialogue;
    vector<EventData_Portal> portal;
    //EventData_Combat;
    //EventData_NPC;
};

void ExecuteEvents(GameContext &ctx);

EventTriggerType CheckEvents(GameContext &ctx);

#endif // EVENT_H

