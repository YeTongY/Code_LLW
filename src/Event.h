#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <vector>

using namespace std;

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
    string targetMapPath;//目标地图路径
    Vector2 targetPosition;//目标坐标
    int facingDir;//目标朝向：见player的枚举

};
//TODO NPC 数据



struct GameEvent{
    string triggerType;//触发条件：npc/zone/auto
    Rectangle area;//触发区域
    bool isOneShot;//是否为一次性的
    bool isTrigged;//是否已被触发
    int EventType;//见event上面的枚举


    //各事件所需数据
    EventData_Dialogue dialogue;
    EventData_Portal teleport;
    //EventData_Combat;
    //EventData_NPC;
};


GameContext CheckAndExecuteEvents(GameContext &ctx){
    Rectangle playerHitBox = ;//玩家碰撞箱

}

#endif // EVENT_H

