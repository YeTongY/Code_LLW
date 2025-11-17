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
    for(const Enemy& enemyl : enemy.enemy){
        if(enemyl.isActive){
            return true;
        }
    }
    return false;
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
    Enemy enemy1 = {};
    enemy1.gridX = 10;
    enemy1.gridY = 10;
    enemy1.isActive = true;                     //存活
    enemy1.isMoving = false;                    //移动
    enemy1.stats = {50,50,10,5};                //属性
    enemy1.Enemytexture = texture;              //贴图
    enemy1.currentDirection = ENEMY_DIR_DOWN;   //敌人朝向

    enemy.enemy.push_back(enemy1);

    //可以在下面添加更多敌人
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

            //绘制角色
            Rectangle source;//声明绘制源
            source.width = 32.0f;
            source.height = 64.0f;
            source.y = 0.0f;

            // 修改 source.x 来“裁剪”正确的帧
            switch (enemy.currentDirection)
            {
                //第 1 帧 (x=0) 
                case ENEMY_DIR_LEFT:
                    source.x = 0.0f;   // 0 * 32
                    break;
            
                //第 2 帧 (x=32) 
                case ENEMY_DIR_DOWN:
                    source.x = 32.0f;  // 1 * 32
                    break;                
                //第 3 帧 (x=64) 
                case ENEMY_DIR_RIGHT:
                    source.x = 64.0f;  // 2 * 32
                    break;
                //第 4 帧 (x=96) 
                case ENEMY_DIR_UP:
                    source.x = 96.0f;  // 3 * 32
                    break;
        
                //防止未初始化或垃圾值导致的花屏Bug
                default:
                    source.x = 32.0f;  // 默认朝下 (Down)
                    TraceLog(LOG_WARNING, "[Enemy] 检测到异常的 currentDirection 值: %d, 使用默认朝向",enemy.currentDirection);
                    break;
        
            }

            //  让角色精灵图绘制在平滑的“视觉位置”上
            Vector2 drawDestPosition = {
                enemy.visualPosition.x,
                enemy.visualPosition.y - TILE_SIZE // 向上偏移一个瓦片高度，使脚踩在格子上
            };

            // 绘制玩家精灵图
            if(enemy.Enemytexture.id != 0){
            // 纹理加载成功，绘制精灵图
                DrawTextureRec(enemy.Enemytexture, source, drawDestPosition, WHITE);
            } else {
            // 纹理未加载，绘制蓝色方块作为占位符
            DrawRectangle(
                (int)drawDestPosition.x,
                (int)drawDestPosition.y,
                32, 64, BLUE
            );
            DrawText("NO SPRITE", enemy.gridX * TILE_SIZE, enemy.gridY * TILE_SIZE, 10, WHITE);
            }

            Rectangle source = {0, 0, (float)ctx.enemySpriteSheet.width, (float)ctx.enemySpriteSheet.height};
            Vector2 position = {
                enemy.gridX * TILE_SIZE,
                enemy.gridY * TILE_SIZE};
            DrawTextureRec(ctx.enemySpriteSheet, source, position, WHITE);
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
        ctx.enemySpriteSheet = LoadTexture(possiblePaths[i]);
        if(ctx.enemySpriteSheet.id != 0){
            TraceLog(LOG_INFO, "[Enemy] 敌人精灵图加载成功: %s (ID: %d, 尺寸: %dx%d)", 
                     possiblePaths[i], 
                     ctx.enemySpriteSheet.id,
                     ctx.enemySpriteSheet.width,
                     ctx.enemySpriteSheet.height);
            loaded = true;
            break;
        }
    }

    if(!loaded){
        TraceLog(LOG_ERROR, "[Enemy] 敌人精灵图加载失败，尝试了所有路径");
        TraceLog(LOG_ERROR, "[Enemy] 当前工作目录: %s", GetWorkingDirectory());
        TraceLog(LOG_ERROR, "[Enemy] 将使用蓝色方块作为占位符");        
    }
}

/**
 * @brief 卸载敌人贴图
 * 
 * @param ctx 
 */
void UnloadEnemyAssets(GameContext& ctx){
    if(ctx.enemySpriteSheet.id != 0){
        UnloadTexture(ctx.enemySpriteSheet);
    }
}