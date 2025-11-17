#pragma once

#include "raylib.h"
#include "Player.h"
#include "GameState.h"
#include "Enemy.h"
#include <vector>

const int TILE_SIZE = 32;//标准图块大小
const char* enemySpriteAddress = "res/graphics/enemy/Enemy_Sprite.png";//敌人精灵位置

using namespace std;

/**
 * @brief 敌人是否存活并显示在地图上
 * 
 * @param enemy 
 * @return true 
 * @return false 
 */
bool isActive(const GameContext& enemy){

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
void InitEnemy(GameContext& enemy, int x, int y,const Stats& stats,Texture2D texture){
    Enemy enemyl = {};
    enemyl.gridX = 10;
    enemyl.gridY = 10;
    enemyl.isActive = true;                     //存活
    enemyl.isMoving = false;                    //移动
    enemyl.stats = {50,50,10,5};                //属性
    enemyl.Enemytexture = texture;              //贴图
    enemyl.currentDirection = ENEMY_DIR_DOWN;   //敌人朝向

}

/**
 * @brief 在地图上绘制敌人
 * 
 * @param ctx 
 * @param enemy 
 */
void DrawEnemy(const GameContext& ctx){
    for(const Enemy& enemy : ctx.enemy){
        if(enemy.isActive){
            Rectangle source = {0, 0, (float)enemy.Enemytexture.width, (float)enemy.Enemytexture.height};
            Vector2 position = {
                enemy.gridX * TILE_SIZE,
                enemy.gridY * TILE_SIZE};
            DrawTextureRec(enemy.Enemytexture, source, position, WHITE);
        }
    }
}

/**
 * @brief 加载敌人贴图
 * 
 * @param ctx 
 */
void LoadEnemyAssets(GameContext& ctx){
    const char* possiblePaths[] = {
        "res/graphics/enemy/Enemy_Sprite.png",
        "../res/graphics/enemy/Enemy_Sprite.png",
        "../../res/graphics/enemy/Enemy_Sprite.png"
    };

    bool loaded = false;
    for (int i = 0; i < 3; i++) {
        ctx.enemy.Enemytexture = LoadTexture(possiblePaths[i]);
        if(ctx.enemy.Enemytexture.id != 0){
            TraceLog(LOG_INFO, "[Enemy] 敌人精灵图加载成功: %s (ID: %d, 尺寸: %dx%d)", 
                     possiblePaths[i], 
                     ctx.enemy.Enemytexture.id,
                     ctx.enemy.Enemytexture.width,
                     ctx.enemy.Enemytexture.height);
            loaded = true;
            break;
        }
    }

    if(!loaded){
        TraceLog(LOG_ERROR, "[Enemy] 敌人精灵图加载失败，尝试了所有路径");
        TraceLog(LOG_ERROR, "[Enemy] 当前工作目录: %s", GetWorkingDirectory());
        TraceLog(LOG_ERROR, "[Enemy] 将使用蓝色方块作为占位符");        //尚未实现蓝色敌人渲染部分
    }
}

/**
 * @brief 卸载敌人贴图
 * 
 * @param ctx 
 */
void UnloadEnemyAssets(GameContext& ctx){
    if(ctx.enemy.Enemytexture.id != 0){
        UnloadTexture(ctx.enemy.Enemytexture);
    }
}