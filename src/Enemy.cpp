#include "raylib.h"
#include "Player.h"
#include "GameState.h"
#include "Enemy.h"
#include "raymath.h" // 包含向量数学函数
#include <vector>
#include <cmath>

const int TILE_SIZE = 32;                                               // 标准图块大小
const char *enemySpriteAddress = "res/graphics/enemy/Enemy_Sprite.png"; // 敌人精灵位置

using namespace std;

namespace
{

    Vector2 GridToWorld(int gridX, int gridY)
    {
        return {gridX * (float)TILE_SIZE, gridY * (float)TILE_SIZE};
    }

    bool IsInsideMap(const GameContext &ctx, int gridX, int gridY)
    {
        return gridY >= 0 && gridY < (int)ctx.tiles.size() &&
               gridX >= 0 && !ctx.tiles.empty() && gridX < (int)ctx.tiles[gridY].size();
    }

    bool IsTileWalkable(const GameContext &ctx, int gridX, int gridY)
    {
        if (!IsInsideMap(ctx, gridX, gridY))
        {
            return false;
        }

        TileType type = ctx.tiles[gridY][gridX];
        return type == TileType::EMPTY || type == TileType::GRASS;
    }

    EnemyDirection DirectionFromDelta(int dx, int dy)
    {
        if (dx > 0)
            return ENEMY_DIR_RIGHT;
        if (dx < 0)
            return ENEMY_DIR_LEFT;
        if (dy > 0)
            return ENEMY_DIR_DOWN;
        return ENEMY_DIR_UP;
    }

    bool StartMoveToTile(Enemy &enemy, const GameContext &ctx, int nextGridX, int nextGridY)
    {
        int diff = abs(nextGridX - enemy.gridX) + abs(nextGridY - enemy.gridY);
        if (diff != 1 || !IsTileWalkable(ctx, nextGridX, nextGridY))
        {
            return false;
        }

        enemy.moveTarget = GridToWorld(nextGridX, nextGridY);
        enemy.isMoving = true;
        enemy.currentDirection = DirectionFromDelta(nextGridX - enemy.gridX, nextGridY - enemy.gridY);
        return true;
    }

    bool TryStepToward(Enemy &enemy, const GameContext &ctx, int targetGridX, int targetGridY)
    {
        int dx = targetGridX - enemy.gridX;
        int dy = targetGridY - enemy.gridY;

        if (dx == 0 && dy == 0)
        {
            return false;
        }

        // 根据距离大小优先在差值更大的轴上移动
        vector<Vector2> tryOrder;
        if (abs(dx) >= abs(dy) && dx != 0)
        {
            tryOrder.push_back({(float)(dx > 0 ? 1 : -1), 0});
        }
        if (dy != 0)
        {
            tryOrder.push_back({0, (float)(dy > 0 ? 1 : -1)});
        }
        if (abs(dy) > abs(dx) && dx != 0)
        {
            tryOrder.push_back({(float)(dx > 0 ? 1 : -1), 0});
        }

        for (const Vector2 &dir : tryOrder)
        {
            int nextX = enemy.gridX + (int)dir.x;
            int nextY = enemy.gridY + (int)dir.y;
            if (StartMoveToTile(enemy, ctx, nextX, nextY))
            {
                return true;
            }
        }

        return false;
    }

    bool TryPatrolStep(Enemy &enemy, const GameContext &ctx)
    {
        // 在四方向中随机挑选一个方向尝试移动
        const Vector2 dirs[4] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
        int startIndex = GetRandomValue(0, 3);
        int centerGridX = (int)(enemy.patrolCenter.x / TILE_SIZE);
        int centerGridY = (int)(enemy.patrolCenter.y / TILE_SIZE);
        int patrolRadiusTiles = (int)(enemy.patrolRange / TILE_SIZE);

        for (int attempt = 0; attempt < 4; ++attempt)
        {
            int idx = (startIndex + attempt) % 4;
            int nextX = enemy.gridX + (int)dirs[idx].x;
            int nextY = enemy.gridY + (int)dirs[idx].y;

            if (patrolRadiusTiles > 0)
            {
                if (abs(nextX - centerGridX) > patrolRadiusTiles ||
                    abs(nextY - centerGridY) > patrolRadiusTiles)
                {
                    continue;
                }
            }

            if (StartMoveToTile(enemy, ctx, nextX, nextY))
            {
                return true;
            }
        }

        return false;
    }

    void UpdateEnemyMovement(Enemy &enemy, float dt)
    {
        if (!enemy.isMoving)
        {
            return;
        }

        enemy.visualPosition = Vector2MoveTowards(enemy.visualPosition, enemy.moveTarget, enemy.moveSpeed * dt);

        if (Vector2Distance(enemy.visualPosition, enemy.moveTarget) < 1.0f)
        {
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
// --- 请替换 Enemy.cpp 中的 DrawEnemySprite ---

void DrawEnemySprite(const GameContext &ctx, const Enemy &enemy)
{
    Texture2D textureToDraw = {0}; // 初始化为空
    bool hasTexture = false;
    Color debugColor = WHITE; // 用于调试

    // 1. 尝试查找贴图
    auto it = ctx.enemyTextures.find(enemy.textureKey);

    if (it != ctx.enemyTextures.end()) {
        textureToDraw = it->second;
        hasTexture = true;
        // 调试：找到了 Key，但图片本身ID是否有效？
        if (textureToDraw.id <= 0) {
            debugColor = RED; // 找到了Key，但图片ID无效
            hasTexture = false;
        }
    } 
    else {
        // 没找到 Key，尝试用 Default
        if (ctx.enemyTextures.count("Default")) {
            textureToDraw = ctx.enemyTextures.at("Default");
            hasTexture = true;
            debugColor = YELLOW; // 使用了 Default 保底
        } else {
            debugColor = MAGENTA; // 连 Default 都没有
        }
    }

    // 2. 绘制逻辑
    if (hasTexture && textureToDraw.id > 0) {
        // 正常绘制
        Rectangle source = {0, 0, 32, 64};
        source.width = 32.0f;
        source.height = 64.0f;
        source.y = 0.0f;

        switch (enemy.currentDirection)
        {
            case ENEMY_DIR_RIGHT: source.x = 0.0f; break;
            case ENEMY_DIR_UP:    source.x = 32.0f; break;
            case ENEMY_DIR_LEFT:  source.x = 64.0f; break;
            case ENEMY_DIR_DOWN:  source.x = 96.0f; break;
            default: source.x = 32.0f; break;
        }

        Vector2 drawDestPosition = {
            enemy.visualPosition.x,
            enemy.visualPosition.y - TILE_SIZE 
        };

        DrawTextureRec(textureToDraw, source, drawDestPosition, WHITE);
        
        // 调试文字：画出当前用的 Key，方便确认
        // DrawText(enemy.textureKey.c_str(), (int)drawDestPosition.x, (int)drawDestPosition.y - 20, 10, GREEN);
    } 
    else {
        // 异常绘制：画一个带颜色的方块，根据颜色判断错误类型
        Rectangle dest = {
            enemy.visualPosition.x, 
            enemy.visualPosition.y - TILE_SIZE, 
            32, 
            64
        };
        DrawRectangleRec(dest, debugColor);
        
        // 显示具体的 Key，方便排查拼写错误
        DrawText(enemy.textureKey.c_str(), (int)dest.x, (int)dest.y - 15, 10, debugColor);
    }
}

/**
 * @brief 更新敌人状态
 *
 * @param ctx
 */
void UpdateEnemies(GameContext &ctx)
{
    float dt = GetFrameTime(); // 帧时间增量

    for (Enemy &enemy : ctx.enemies)
    {
        if (!enemy.isActive)
            continue;

        float distanceToPlayer = Vector2Distance(enemy.visualPosition, ctx.player.visualPosition); // 计算敌人与玩家之间的距离

        if (enemy.aiState == AI_STATE_PATROL && distanceToPlayer < enemy.aggroRange)
        {
            enemy.aiState = AI_STATE_CHASING;
        }
        else if (enemy.aiState == AI_STATE_CHASING && distanceToPlayer > enemy.aggroRange * 1.5f)
        {
            enemy.aiState = AI_STATE_PATROL;
        }

        UpdateEnemyMovement(enemy, dt);
        if (enemy.isMoving)
        {
            continue; // 正在移动时不尝试新的目标
        }

        bool startedMove = false;
        if (enemy.aiState == AI_STATE_CHASING)
        {
            startedMove = TryStepToward(enemy, ctx, ctx.player.gridX, ctx.player.gridY);
        }
        else
        {
            startedMove = TryPatrolStep(enemy, ctx);

            if (!startedMove)
            {
                // 回中心点保证在巡逻范围内缓慢移动
                int centerGridX = (int)(enemy.patrolCenter.x / TILE_SIZE);
                int centerGridY = (int)(enemy.patrolCenter.y / TILE_SIZE);
                startedMove = TryStepToward(enemy, ctx, centerGridX, centerGridY);
            }
        }

        if (!startedMove)
        {
            enemy.moveTarget = GridToWorld(enemy.gridX, enemy.gridY); // 保持对齐
        }
    }
}

/**
 * @brief 加载敌人贴图
 *
 * @param ctx
 */
void LoadEnemyAssets(GameContext &ctx)
{
    ctx.enemyTextures.clear();

    // 定义你要加载的素材清单 (名字 -> 路径)
    // 这里的名字 (如 "Default", "Pink") 就是在 Tiled 里要填的值
    map<string, const char*> assetsToLoad = {
        {"Default", "../res/graphics/enemy/Enemy_Sprite.png"}, // 默认西装男
        {"Pink",    "../res/graphics/enemy/Enemy_Pink.png"},   // 粉发妹
        {"Guard",   "../res/graphics/enemy/Enemy_Guard.png"}   // 保安
    };
    
    //循环加载
    for(auto const& [key, path] : assetsToLoad){
        //兼容路径尝试
        const char* possiblePaths[] = { path, nullptr, nullptr};
        string path1 = string("../") + path;
        string path2 = string("../../") + path;
        possiblePaths[1] = path1.c_str();
        possiblePaths[2] = path2.c_str();

        bool loaded = false;
        for(int i = 0; i < 3; i++){
            Texture2D tex = LoadTexture(possiblePaths[i]);
            if(tex.id != 0){
                ctx.enemyTextures[key] = tex;
                TraceLog(LOG_INFO, "[Enemy] 加载敌人素材 [%s] 成功", key.c_str());
                loaded = true;
                break;
            }
        }
        if(!loaded){
            TraceLog(LOG_ERROR, "[Enemy] 加载敌人素材 [%s] 失败，尝试了所有路径", key.c_str());
        }
    }

}

/**
 * @brief 卸载敌人贴图
 *
 * @param ctx
 */
void UnloadEnemyAssets(GameContext &ctx)
{
    for(auto const& [key, tex] : ctx.enemyTextures){
        if(tex.id != 0){
            UnloadTexture(tex);
            TraceLog(LOG_INFO, "[Enemy] 卸载敌人素材 [%s] 成功", key.c_str());
        }
    }
    ctx.enemyTextures.clear();
}