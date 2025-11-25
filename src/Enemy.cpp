#include "raylib.h"
#include "Player.h"
#include "GameState.h"
#include "Enemy.h"
#include "raymath.h"// 包含向量数学函数
#include <vector>

const int TILE_SIZE = 32;//标准图块大小
const char* enemySpriteAddress = "res/graphics/enemy/Enemy_Sprite.png";//敌人精灵位置

using namespace std;

/**
 * @brief 在地图上绘制敌人
 * 
 * @param ctx 
 * @param enemy 
 */
void DrawEnemy(const GameContext& ctx){
    for(const Enemy& enemy : ctx.enemies){
        if(enemy.isActive){

            //绘制角色
            Rectangle source = {0,0,32,64};
            source.width = 32.0f;
            source.height = 64.0f;
            source.y = 0.0f;

            // 修改 source.x 来“裁剪”正确的帧
            switch (enemy.currentDirection)
            {
                //第 1 帧 (x=0) 
                case ENEMY_DIR_RIGHT:
                    source.x = 0.0f;   // 0 * 32
                    break;
            
                //第 2 帧 (x=32) 
                case ENEMY_DIR_UP:
                    source.x = 32.0f;  // 1 * 32
                    break;                
                //第 3 帧 (x=64) 
                case ENEMY_DIR_LEFT:
                    source.x = 64.0f;  // 2 * 32
                    break;
                //第 4 帧 (x=96) 
                case ENEMY_DIR_DOWN:
                    source.x = 96.0f;  // 3 * 32
                    break;
        
                //防止未初始化或垃圾值导致的花屏Bug
                default:
                    source.x = 32.0f;  // 默认朝下 (Down)
                    TraceLog(LOG_WARNING, "[Enemy] 检测到异常的 currentDirection 值: %d, 使用默认朝向",enemy.currentDirection);
                    break;
        
            }

            //  让角色精灵图绘制在平滑的"视觉位置"上
            Vector2 drawDestPosition = {
                enemy.visualPosition.x,
                enemy.visualPosition.y - TILE_SIZE // 向上偏移一个瓦片高度，使脚踩在格子上
            };

            // 绘制玩家精灵图
            if(ctx.enemySpriteSheet.id != 0){
            // 纹理加载成功，绘制精灵图
                DrawTextureRec(ctx.enemySpriteSheet, source, drawDestPosition, WHITE);
            } else {
            // 纹理未加载，绘制蓝色方块作为占位符
            DrawRectangle(
                (int)drawDestPosition.x,
                (int)drawDestPosition.y,
                32, 64, BLUE
            );
            DrawText("NO SPRITE", enemy.gridX * TILE_SIZE, enemy.gridY * TILE_SIZE, 10, WHITE);
            }
        }
    }
}

/**
 * @brief 更新敌人状态
 * 
 * @param ctx 
 */
void UpdateEnemies(GameContext& ctx){
    float dt = GetFrameTime(); // 帧时间增量

    for(Enemy& enemy : ctx.enemies){
        if(!enemy.isActive) continue; 
            float distanceToPlayer = Vector2Distance(enemy.visualPosition, ctx.player.visualPosition); // 计算敌人与玩家之间的距离

            switch (enemy.aiState)//切换敌人AI状态
            {
            case AI_STATE_PATROL://巡逻模式
                if(distanceToPlayer < enemy.aggroRange){
                    enemy.aiState = AI_STATE_CHASING;       //如果进入索敌范围，自动切换为追击模式
                }else if(!enemy.isMoving){

                    for(int attempt = 0; attempt < 10; ++attempt){
                        float randomX = enemy.patrolCenter.x + GetRandomValue((int)-enemy.patrolRange, (int)enemy.patrolRange);
                        float randomY = enemy.patrolCenter.y + GetRandomValue((int)-enemy.patrolRange, (int)enemy.patrolRange);

                        if (randomX < 0 || randomX >= ctx.width * TILE_SIZE || 
                            randomY < 0 || randomY >= ctx.height * TILE_SIZE){
                                continue; // 超出地图边界，重新选择
                        }

                        int targetGridX = (int)(randomX / TILE_SIZE);
                        int targetGridY = (int)(randomY / TILE_SIZE);

                        if (targetGridY >= 0 && targetGridY < ctx.tiles.size() &&
                            targetGridX >= 0 && targetGridX < ctx.tiles[targetGridY].size()) {
                
                            TileType type = ctx.tiles[targetGridY][targetGridX];
                
                            // 只有目标点是 空地(EMPTY) 或 草地(GRASS) 时才移动
                            if (type != TileType::WALL) {
                                enemy.moveTarget = {randomX, randomY};
                                enemy.isMoving = true;
                                break; // 找到合法点，退出尝试循环
                            }
                        }
                        
                    }
                }
                break;
            case AI_STATE_CHASING://追击模式

                if(distanceToPlayer > enemy.aggroRange * 1.5f){
                    enemy.aiState = AI_STATE_PATROL;       //如果玩家离开索敌范围，切换回巡逻模式
                    enemy.moveTarget = enemy.patrolCenter; // 返回巡逻中心
                    enemy.isMoving = true;
                }else {
                    enemy.moveTarget = ctx.player.visualPosition;   //更新移动目标为玩家位置
                    enemy.isMoving = true;
                }
                break;
            default:
                break;
            }

            if(enemy.isMoving){
                Vector2 oldPosition = enemy.visualPosition;
                //使用跟玩家移动相同的平滑移动逻辑
                enemy.visualPosition = Vector2MoveTowards(
                    enemy.visualPosition,
                    enemy.moveTarget,
                    enemy.moveSpeed * dt
                );

            Vector2 movement = Vector2Subtract(enemy.visualPosition, oldPosition);

            if(Vector2Length(movement) > 0.1f){
                //更新敌人朝向
                if(abs(movement.x) > abs(movement.y)){
                    //水平移动
                    enemy.currentDirection = (movement.x > 0) ? ENEMY_DIR_RIGHT : ENEMY_DIR_LEFT;
                }else{
                    //垂直移动
                    enemy.currentDirection = (movement.y > 0) ? ENEMY_DIR_DOWN : ENEMY_DIR_UP;
                }
            }

                enemy.gridX = (int)(enemy.visualPosition.x + TILE_SIZE/2) / TILE_SIZE;
                enemy.gridY = (int)(enemy.visualPosition.y + TILE_SIZE/2) / TILE_SIZE;

                //检查是否到达目标位置
            if(Vector2Distance(enemy.visualPosition, enemy.moveTarget) < 2.0f){
                //如果没有追击玩家，则停止移动
                if(enemy.aiState != AI_STATE_CHASING){
                    enemy.isMoving = false;
                    enemy.visualPosition = enemy.moveTarget; // 校准位置
                }
            }
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