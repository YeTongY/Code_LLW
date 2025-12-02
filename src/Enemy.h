#ifndef ENEMY_H

#pragma once

#include <vector>
#include <string>
#include "raylib.h"
#include "Player.h"

struct GameContext;

typedef enum {//枚举敌人方向
    ENEMY_DIR_DOWN = 0, // 面向下方
    ENEMY_DIR_UP,       // 面向上方
    ENEMY_DIR_LEFT,     // 面向左方
    ENEMY_DIR_RIGHT     // 面向右方
} EnemyDirection;

typedef enum {
    AI_STATE_PATROL, // 巡逻状态
    AI_STATE_CHASING // 追击状态
} AIState;

struct Enemy {//声明敌人主结构体
    int gridX; // 敌人的 X 坐标
    int gridY; // 敌人的 Y 坐标
    float moveSpeed;// 移动速度 像素/秒
    bool isMoving;//敌人移动状态
    Vector2 visualPosition;//敌人渲染位置
    Vector2 moveTarget;//敌人目标移动位置

    Vector2 patrolCenter; // 巡逻点
    AIState aiState; // 敌人AI状态
    float aggroRange; // 敌人的索敌范围
    float patrolRange; // 敌人的巡逻范围

    bool isActive; // 敌人是否存活并显示在地图上

    Stats stats; // 敌人的属性

    EnemyDirection currentDirection; // 敌人"当前"的方向

    string textureKey; // 敌人纹理的键值
};

void DrawEnemySprite(const GameContext &ctx, const Enemy &enemy);                                                 // 在地图上绘制敌人
void LoadEnemyAssets(GameContext& ctx);                                                 // 加载敌人贴图
void UnloadEnemyAssets(GameContext& ctx);                                               // 卸载敌人贴图
void UpdateEnemies(GameContext& ctx);                                                   // 更新敌人状态

#endif // ENEMY_H