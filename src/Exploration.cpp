#include "ExplorationState.h"
#include "Player.h" 
#include "Map.h"
#include "raylib.h"
#include <cstdlib>
#include <cstdio>

const int TILE_SIZE = 32; // 与 Player.cpp 保持一致

//==========================探索状态实现

void exploration_enter(GameContext* ctx, void* state_data)
{
    ExplorationData* expd = static_cast<ExplorationData*>(state_data);

    //初始化探索数据
    expd->isActive = true;
    expd->elapsedTime = 0.0f;

    //初始化摄像机 - 跟随玩家
    ctx->camera.target = Vector2{
        static_cast<float>(ctx->player.gridX * TILE_SIZE) + (TILE_SIZE / 2.0f),
        static_cast<float>(ctx->player.gridY * TILE_SIZE) + (TILE_SIZE / 2.0f)
    };
    ctx->camera.offset = Vector2{ 
        static_cast<float>(ctx->screenWidth) / 2.0f, 
        static_cast<float>(ctx->screenHeight) / 2.0f 
    };
    ctx->camera.rotation = 0.0f;
    ctx->camera.zoom = 3.0f; // 与 drawPlayer 保持一致

    TraceLog(LOG_INFO, "[Exploration] 进入探索状态");
}

void exploration_exit(GameContext* ctx, void* state_data)
{
    ExplorationData* expd = static_cast<ExplorationData*>(state_data);
    expd->isActive = false;
    
    TraceLog(LOG_INFO, "[Exploration] 退出探索状态");
}

void exploration_update(GameContext* ctx, void* state_data)
{
    ExplorationData* expd = static_cast<ExplorationData*>(state_data);
    
    if (!expd->isActive) return;

    //更新已运行时间
    expd->elapsedTime += GetFrameTime();

    //======核心：直接调用 Player 模块的更新函数======
    // 包含：键盘输入、碰撞检测、位置更新
    updatePlayer(*ctx);

    //更新摄像机跟随玩家
    ctx->camera.target = Vector2{
        static_cast<float>(ctx->player.gridX * TILE_SIZE) + (TILE_SIZE / 2.0f),
        static_cast<float>(ctx->player.gridY * TILE_SIZE) + (TILE_SIZE / 2.0f)
    };

    //处理状态切换
    if (IsKeyPressed(KEY_E)) {
        TraceLog(LOG_INFO, "[Exploration] 按下E键 - 触发交互");
        // 未来可以切换到战斗状态：
        // GameStateMachine_change(&ctx->state_machine, ctx, createCombatState());
    }

    //退出游戏
    if (IsKeyPressed(KEY_ESCAPE)) {
        ctx->isRunning = false;
        TraceLog(LOG_INFO, "[Exploration] 按下ESC - 退出游戏");
    }
}

void exploration_render(GameContext* ctx, void* state_data)
{
    ExplorationData* expd = static_cast<ExplorationData*>(state_data);
    
    if (!expd->isActive) return;

    //======核心：直接调用 Player 模块的渲染函数======
    // 包含：BeginMode2D、DrawMap、绘制玩家、EndMode2D
    drawPlayer(*ctx);

    //在 EndMode2D 之后，绘制 UI（不受摄像机影响）
    DrawText("探索模式", 10, 10, 20, BLACK);
    DrawText("操作提示:", 10, 40, 16, DARKGRAY);
    DrawText("  WASD - 移动", 10, 60, 14, DARKGRAY);
    DrawText("  E - 交互", 10, 80, 14, DARKGRAY);
    DrawText("  ESC - 退出", 10, 100, 14, DARKGRAY);
    
    //显示玩家坐标
    char posText[64];
    std::sprintf(posText, "位置: (%d, %d)", ctx->player.gridX, ctx->player.gridY);
    DrawText(posText, 10, ctx->screenHeight - 60, 16, DARKGREEN);
    
    //显示玩家属性
    char hpText[64];
    std::sprintf(hpText, "HP: %d/%d", ctx->player.stats.hp, ctx->player.stats.maxHp);
    DrawText(hpText, 10, ctx->screenHeight - 40, 16, RED);
    
    //显示运行时间
    char timeText[64];
    std::sprintf(timeText, "时间: %.1fs", expd->elapsedTime);
    DrawText(timeText, 10, ctx->screenHeight - 20, 14, DARKGRAY);
}

//探索状态创建函数
GameState* createExplorationState()
{
    //使用 new 分配探索状态数据（C++ 风格）
    ExplorationData* expd = new ExplorationData();
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
    
    if (!state) {
        TraceLog(LOG_ERROR, "[Exploration] 创建失败");
        delete expd;
        return nullptr;
    }
    
    TraceLog(LOG_INFO, "[Exploration] 创建成功");
    return state;
}
