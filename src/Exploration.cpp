#include "ExplorationState.h"
#include "Player.h" 
#include "Map.h"
#include "raylib.h"
#include "raymath.h"
#include "GameState.h"
#include "Combat.h"
#include "Dialogue.h"
#include <cstdlib>
#include <cstdio>
//================================
#include "Event.h"
#include "Dialogue.h"
#include "Combat.h"
#include "UI.h"  // 新增：引入UI接口以便在探索状态绘制HUD
#include "Audio.h" // 新增：控制脚步音效



const int TILE_SIZE = 32; // 与 Player.cpp 保持一致
//===========================================2025/11/25新增配合combat模块的函数
static void TryTriggerCombat(GameContext* ctx)
{
    if (!ctx) return;

    const float triggerDistance = ctx->tileSize > 0 ? ctx->tileSize * 0.75f : TILE_SIZE * 0.75f; // 触发战斗的距离阈值
    const float triggerDistanceSq = triggerDistance * triggerDistance; // 阈值平方，用于平方距离比较
    const Vector2 playerPos = ctx->player.visualPosition; // 玩家当前像素位置

    for (Enemy& enemy : ctx->enemies)
    {
        if (!enemy.isActive) continue;

        const float distanceSq = Vector2DistanceSqr(playerPos, enemy.visualPosition);
        if (distanceSq > triggerDistanceSq) continue;

        ctx->currentCombatant = &enemy;
        GameState* combatState = CreateCombatState(&enemy); // 进入与该敌人的战斗状态
        if (combatState)
        {
            TraceLog(LOG_INFO, "[Exploration] 触发与敌人(%d,%d)的战斗", enemy.gridX, enemy.gridY);
            GameStateMachine_change(&ctx->state_machine, ctx, combatState);
        }
        else
        {
            TraceLog(LOG_ERROR, "[Exploration] 创建战斗状态失败，重置 currentCombatant");
            ctx->currentCombatant = nullptr;
        }
        break;
    }
}
//=============================================================================
//==========================探索状态实现

void exploration_enter(GameContext* ctx, void* state_data)
{
    ExplorationData* expd = static_cast<ExplorationData*>(state_data); // 当前探索状态数据

    //初始化探索数据
    expd->isActive = true;
    expd->elapsedTime = 0.0f;

    //初始化摄像机 - 跟随玩家
    ctx->camera.target = Vector2{
        static_cast<float>(ctx->player.gridX * TILE_SIZE) + (TILE_SIZE / 2.0f), // 将玩家中心对准摄像机
        static_cast<float>(ctx->player.gridY * TILE_SIZE) + (TILE_SIZE / 2.0f)
    };
    ctx->camera.offset = Vector2{ 
        static_cast<float>(ctx->screenWidth) / 2.0f, 
        static_cast<float>(ctx->screenHeight) / 2.0f 
    };
    ctx->camera.rotation = 0.0f;
    ctx->camera.zoom = 3.0f; // 与 drawPlayer 保持一致

    TraceLog(LOG_INFO, "[Exploration] 进入探索状态");

    ctx->enableFootstepAudio = true;
    ctx->footstepIdleTimer = 0.0f;

    //=========播放探索背景音乐=========
    PlayExplorationBGM(*ctx);
}

void exploration_exit(GameContext* ctx, void* state_data)
{
    ExplorationData* expd = static_cast<ExplorationData*>(state_data); // 待关闭的探索状态
    expd->isActive = false;
    
    TraceLog(LOG_INFO, "[Exploration] 退出探索状态");

    ctx->enableFootstepAudio = false;
    ctx->player.isMoving = false;
    StopFootstepSound(*ctx);
    
}

void exploration_update(GameContext* ctx, void* state_data)
{
    ExplorationData* expd = static_cast<ExplorationData*>(state_data); // 探索状态运行数据
    
    if (!expd->isActive) return;

    //更新已运行时间
    expd->elapsedTime += GetFrameTime();

    //======核心：直接调用 Player 模块的更新函数======
    // 包含：键盘输入、碰撞检测、位置更新
    updatePlayer(*ctx);
    UpdateEnemies(*ctx);
    TryTriggerCombat(ctx);

    // 持续刷新探索背景音乐的流缓冲，避免播放停顿
    if (ctx->isExplorationBGMPlaying) {
        UpdateMusicStream(ctx->explorationBGM);
    }

    //==========战斗状态更新============
   Rectangle playerRect = {
        ctx->player.visualPosition.x + 8,
        ctx->player.visualPosition.y + 32,
        16,
        32
    }; // 玩家碰撞检测矩形

    for(auto& enemy : ctx->enemies){
        if(enemy.isActive){
            Rectangle EnemyRcet{
                enemy.visualPosition.x + 8,
                enemy.visualPosition.y + 32,
                16,
                32
            }; // 敌人碰撞检测矩形

            if(CheckCollisionRecs(playerRect, EnemyRcet)){
                TraceLog(LOG_INFO, "[Exploration] 玩家与敌人发生碰撞，准备进入战斗状态");
                
                GameState* combatState = CreateCombatState(&enemy); // 与该敌人开战

                if(combatState){
                    GameStateMachine_change(&ctx->state_machine, ctx, combatState);
                }
                
                break; // 只处理第一个碰撞的敌人
            }
        }
    }



    //==========开始事件更新============
    ExecuteEvents(*ctx);


    

    // --- 事件处理逻辑 ---
    if (IsKeyPressed(KEY_E)) {
        TraceLog(LOG_INFO, "[Exploration] 按下 E 键，开始检查交互事件...");
        //CheckAndExecuteEvents(*ctx); 
        
    }

    //退出游戏
    if (IsKeyPressed(KEY_ESCAPE)) {
        ctx->isRunning = false;
        TraceLog(LOG_INFO, "[Exploration] 按下ESC - 退出游戏");
    }
}

void exploration_render(GameContext* ctx, void* state_data)
{
    ExplorationData* expd = static_cast<ExplorationData*>(state_data); // 当前渲染引用的数据
    
    // 安全检查：允许从对话状态调用（此时 state_data 为 nullptr）
    if (expd != nullptr && !expd->isActive) return;

    //======核心：直接调用 Player 模块的渲染函数======
    // 包含：BeginMode2D、DrawMap、绘制玩家、EndMode2D（滤镜在内部应用）
    DrawMapScene(*ctx);

    // 新增：调用 HUD 绘制，以显示玩家血量头像等信息
    DrawHUD(*ctx);

    //在 EndMode2D 之后，绘制 UI（不受摄像机和滤镜影响）
    DrawTextEx(ctx->mainFont, "探索模式", Vector2{10, 10}, 30, 1, BLACK);
    DrawTextEx(ctx->mainFont, "操作说明:", Vector2{10, 40}, 24, 1, DARKGRAY);
    DrawTextEx(ctx->mainFont, "  WASD - 移动", Vector2{10, 60}, 24, 1, DARKGRAY);
    DrawTextEx(ctx->mainFont, "  E - 交互", Vector2{10, 80}, 24, 1, DARKGRAY);
    DrawTextEx(ctx->mainFont, "  ESC - 退出", Vector2{10, 100}, 24, 1, DARKGRAY);
    
    //显示玩家坐标
    char posText[64]; // 玩家坐标文本缓冲
    std::sprintf(posText, "(%d, %d)", ctx->player.gridX, ctx->player.gridY);
    DrawTextEx(ctx->mainFont, "位置: ", Vector2{10, (float)ctx->screenHeight - 60}, 24, 1, DARKGREEN);
    DrawText(posText, 80, static_cast<int>(ctx->screenHeight) - 60, 24, DARKGREEN);
    
    //显示玩家属性
    char hpText[64]; // 玩家血量文本缓冲
    std::sprintf(hpText, "%d/%d", ctx->player.stats.hp, ctx->player.stats.maxHp);
    DrawTextEx(ctx->mainFont, "HP: ", Vector2{10, (float)ctx->screenHeight - 40}, 24, 1, RED);
    DrawText(hpText, 80, static_cast<int>(ctx->screenHeight) - 40, 24, RED);
    
    //显示运行时间
    char timeText[64]; // 运行时间文本缓冲
    std::sprintf(timeText, "%.1fs", expd ? expd->elapsedTime : 0.0f);
    DrawTextEx(ctx->mainFont, "时间: ", Vector2{10, (float)ctx->screenHeight - 20}, 20, 1, DARKGRAY);
    DrawText(timeText, 80, static_cast<int>(ctx->screenHeight) - 20, 20, DARKGRAY);


}

//探索状态创建函数
GameState* createExplorationState()
{
    //使用 new 分配探索状态数据
    ExplorationData* expd = new ExplorationData(); // 探索状态专用的数据包
    if (!expd) {
        TraceLog(LOG_ERROR, "[Exploration] 创建失败：内存分配失败");
        return nullptr;
    }
    
    //创建状态
    GameState* state = Gamestate_create(
        exploration_enter,      // 进入回调
        exploration_exit,       // 退出回调
        exploration_update,     // 更新回调
        exploration_render,     // 渲染回调
        expd,                   // 状态数据
        sizeof(ExplorationData) // 数据大小
    );
    // state 为注册进状态机的探索状态指针
    
    if (!state) {
        TraceLog(LOG_ERROR, "[Exploration] 创建失败");
        delete expd;
        return nullptr;
    }
    
    TraceLog(LOG_INFO, "[Exploration] 创建成功");
    return state;
}
