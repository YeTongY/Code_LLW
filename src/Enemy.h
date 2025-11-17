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

struct Enemy {//声明敌人主结构体
    int gridX; // 敌人在"网格"上的 X 坐标 (逻辑)
    int gridY; // 敌人在"网格"上的 Y 坐标 (逻辑)
    float moveSpeed;// 移动速度 像素/秒
    bool isMoving;//敌人移动状态
    Vector2 visualPosition;//敌人渲染位置
    Vector2 moveTarget;//敌人目标移动位置

    bool isActive; // 敌人是否存活并显示在地图上

    Stats stats; // 敌人的属性

    Texture2D Enemytexture; // 敌人的精灵图贴图
    EnemyDirection currentDirection; // 敌人"当前"的方向
};

bool isActive(const GameContext& enemy);                                                // 敌人是否存活并显示在地图上
void InitEnemy(GameContext enemy, int x, int y,const Stats& stats,Texture2D texture);   // 初始化敌人
void DrawEnemy(const GameContext& ctx);                       // 在地图上绘制敌人
void LoadEnemyAssets(GameContext& ctx);                                                 // 加载敌人贴图
void UnloadEnemyAssets(GameContext& ctx);                                               // 卸载敌人贴图

#endif // ENEMY_H