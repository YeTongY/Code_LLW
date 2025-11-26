#include "raylib.h"
#include "Player.h"
#include "GameState.h"
#include "Enemy.h"
#include "raymath.h"// 包含向量数学函数
#include <vector>
#include <cmath>

const int TILE_SIZE = 32;//标准图块大小
const char* enemySpriteAddress = "res/graphics/enemy/Enemy_Sprite.png";//敌人精灵位置

using namespace std;

namespace {

Vector2 GridToWorld(int gridX, int gridY)
{
    return { gridX * (float)TILE_SIZE, gridY * (float)TILE_SIZE };
}

bool IsInsideMap(const GameContext& ctx, int gridX, int gridY)
{
    return gridY >= 0 && gridY < (int)ctx.tiles.size() &&
           gridX >= 0 && !ctx.tiles.empty() && gridX < (int)ctx.tiles[gridY].size();
}

bool IsTileWalkable(const GameContext& ctx, int gridX, int gridY)
{
    if (!IsInsideMap(ctx, gridX, gridY)) {
        return false;
    }

    TileType type = ctx.tiles[gridY][gridX];
    return type == TileType::EMPTY || type == TileType::GRASS;
}

EnemyDirection DirectionFromDelta(int dx, int dy)
{
    if (dx > 0) return ENEMY_DIR_RIGHT;
    if (dx < 0) return ENEMY_DIR_LEFT;
    if (dy > 0) return ENEMY_DIR_DOWN;
    return ENEMY_DIR_UP;
}

bool StartMoveToTile(Enemy& enemy, const GameContext& ctx, int nextGridX, int nextGridY)
{
    int diff = abs(nextGridX - enemy.gridX) + abs(nextGridY - enemy.gridY);
    if (diff != 1 || !IsTileWalkable(ctx, nextGridX, nextGridY)) {
        return false;
    }

    enemy.moveTarget = GridToWorld(nextGridX, nextGridY);
    enemy.isMoving = true;
    enemy.currentDirection = DirectionFromDelta(nextGridX - enemy.gridX, nextGridY - enemy.gridY);
    return true;
}

bool TryStepToward(Enemy& enemy, const GameContext& ctx, int targetGridX, int targetGridY)
{
    int dx = targetGridX - enemy.gridX;
    int dy = targetGridY - enemy.gridY;

    if (dx == 0 && dy == 0) {
        return false;
    }

    // 根据距离大小优先在差值更大的轴上移动
    vector<Vector2> tryOrder;
    if (abs(dx) >= abs(dy) && dx != 0) {
        tryOrder.push_back({ (float)(dx > 0 ? 1 : -1), 0 });
    }
    if (dy != 0) {
        tryOrder.push_back({ 0, (float)(dy > 0 ? 1 : -1) });
    }
    if (abs(dy) > abs(dx) && dx != 0) {
        tryOrder.push_back({ (float)(dx > 0 ? 1 : -1), 0 });
    }

    for (const Vector2& dir : tryOrder) {
        int nextX = enemy.gridX + (int)dir.x;
        int nextY = enemy.gridY + (int)dir.y;
        if (StartMoveToTile(enemy, ctx, nextX, nextY)) {
            return true;
        }
    }

    return false;
}

bool TryPatrolStep(Enemy& enemy, const GameContext& ctx)
{
    // 在四方向中随机挑选一个方向尝试移动
    const Vector2 dirs[4] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };
    int startIndex = GetRandomValue(0, 3);
    int centerGridX = (int)(enemy.patrolCenter.x / TILE_SIZE);
    int centerGridY = (int)(enemy.patrolCenter.y / TILE_SIZE);
    int patrolRadiusTiles = (int)(enemy.patrolRange / TILE_SIZE);

    for (int attempt = 0; attempt < 4; ++attempt) {
        int idx = (startIndex + attempt) % 4;
        int nextX = enemy.gridX + (int)dirs[idx].x;
        int nextY = enemy.gridY + (int)dirs[idx].y;

        if (patrolRadiusTiles > 0) {
            if (abs(nextX - centerGridX) > patrolRadiusTiles ||
                abs(nextY - centerGridY) > patrolRadiusTiles) {
                continue;
            }
        }

        if (StartMoveToTile(enemy, ctx, nextX, nextY)) {
            return true;
        }
    }

    return false;
}

void UpdateEnemyMovement(Enemy& enemy, float dt)
{
    if (!enemy.isMoving) {
        return;
    }

    enemy.visualPosition = Vector2MoveTowards(enemy.visualPosition, enemy.moveTarget, enemy.moveSpeed * dt);

    if (Vector2Distance(enemy.visualPosition, enemy.moveTarget) < 1.0f) {
        enemy.visualPosition = enemy.moveTarget;
        enemy.isMoving = false;
        enemy.gridX = (int)(enemy.moveTarget.x / TILE_SIZE);
        enemy.gridY = (int)(enemy.moveTarget.y / TILE_SIZE);
    }
}

}

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

        if(enemy.aiState == AI_STATE_PATROL && distanceToPlayer < enemy.aggroRange){
            enemy.aiState = AI_STATE_CHASING;
        }else if(enemy.aiState == AI_STATE_CHASING && distanceToPlayer > enemy.aggroRange * 1.5f){
            enemy.aiState = AI_STATE_PATROL;
        }

        UpdateEnemyMovement(enemy, dt);
        if(enemy.isMoving){
            continue; // 正在移动时不尝试新的目标
        }

        bool startedMove = false;
        if(enemy.aiState == AI_STATE_CHASING){
            startedMove = TryStepToward(enemy, ctx, ctx.player.gridX, ctx.player.gridY);
        }else{
            startedMove = TryPatrolStep(enemy, ctx);

            if(!startedMove){
                //回中心点保证在巡逻范围内缓慢移动
                int centerGridX = (int)(enemy.patrolCenter.x / TILE_SIZE);
                int centerGridY = (int)(enemy.patrolCenter.y / TILE_SIZE);
                startedMove = TryStepToward(enemy, ctx, centerGridX, centerGridY);
            }
        }

        if(!startedMove){
            enemy.moveTarget = GridToWorld(enemy.gridX, enemy.gridY); // 保持对齐
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