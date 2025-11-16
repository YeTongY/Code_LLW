#pragma once

#include "raylib.h"
#include "Player.h"
#include "GameState.h"
#include "Enemy.h"
#include <vector>

using namespace std;

/**
 * @brief 敌人是否存活并显示在地图上
 * 
 * @param enemy 
 * @return true 
 * @return false 
 */
bool isActive(const Enemy& enemy){

}

/**
 * @brief // 初始化敌人
 * 
 * @param enemy 
 * @param x 
 * @param y 
 * @param stats 
 * @param texture 
 */
void InitEnemy(Enemy& enemy, int x, int y,const Stats& stats,Texture2D texture){

}

/**
 * @brief 在地图上绘制敌人
 * 
 * @param ctx 
 * @param enemy 
 */
void DrawEnemy(const GameContext& ctx, const Enemy& enemy){

}
