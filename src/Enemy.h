#ifndef ENEMY_H

#pragma once

#include <vector>
#include <string>
#include "raylib.h"
#include "Player.h"

struct Enemy {
    int gridX;
    int gridY;

    Stats stats;                    // 敌人和玩家一样，拥有HP、攻击力等属性

    Texture2D Enemytexture;         // 敌人在大地图上显示的纹理
};

bool isActive(const Enemy& enemy);                                                  // 敌人是否存活并显示在地图上
void InitEnemy(Enemy& enemy, int x, int y,const Stats& stats,Texture2D texture);    // 初始化敌人
void DrawEnemy(const GameContext& ctx, const Enemy& enemy);                         // 在地图上绘制敌人

#endif // ENEMY_H