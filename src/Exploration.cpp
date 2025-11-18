#include "ExplorationState.h"
#include "Player.h" 
#include "Map.h"
#include "raylib.h"
#include "GameState.h"
#include <cstdlib>
#include <cstdio>
//================================
#include "Event.h"
#include "DialogueState.h"



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

    // 摄像机在 drawPlayer() 中设置，无需在此更新

    //处理状态切换
    if (IsKeyPressed(KEY_E)) {
        TraceLog(LOG_INFO, "[Exploration] 按下 E 键，开始检查交互事件...");
        // 遍历当前关卡的所有事件
        for (const auto& event : ctx->gameEvents) { 
            
            // 检查这个事件是不是我们关心的“按E交互”类型
            if (event.triggerType == "npc_interaction") {
                
                // TODO (进阶): 检查玩家是否真的在NPC旁边
                // bool playerIsNearTarget = (abs(ctx->player.gridX - targetX) <= 1 && abs(ctx->player.gridY - targetY) <= 1);
                // if (playerIsNearTarget) { ... }

                // 暂时，我们只要按E就触发第一个找到的交互事件
                TraceLog(LOG_INFO, "触发事件！加载剧本: %s", event.scriptPath.c_str());

                // 从事件中获取剧本路径并加载
                vector<DialogueLine> script = LoadDialogueScript(event.scriptPath.c_str());

                if (!script.empty()) {
                    // 切换到对话状态
                    GameStateMachine_change(&ctx->state_machine, ctx, createDialogueState(script));
                    return; // 触发了一个事件后，立刻结束本帧的update，防止一帧内触发多个事件
                }
            }
        }
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
    // 包含：BeginMode2D、DrawMap、绘制玩家、EndMode2D（滤镜在内部应用）
    DrawMapScene(*ctx);

    //在 EndMode2D 之后，绘制 UI（不受摄像机和滤镜影响）
    DrawTextEx(ctx->mainFont, "探索模式", Vector2{10, 10}, 30, 1, BLACK);
    DrawTextEx(ctx->mainFont, "操作说明:", Vector2{10, 40}, 24, 1, DARKGRAY);
    DrawTextEx(ctx->mainFont, "  WASD - 移动", Vector2{10, 60}, 24, 1, DARKGRAY);
    DrawTextEx(ctx->mainFont, "  E - 交互", Vector2{10, 80}, 24, 1, DARKGRAY);
    DrawTextEx(ctx->mainFont, "  ESC - 退出", Vector2{10, 100}, 24, 1, DARKGRAY);
    
    //显示玩家坐标
    char posText[64];
    std::sprintf(posText, "(%d, %d)", ctx->player.gridX, ctx->player.gridY);
    DrawTextEx(ctx->mainFont, "位置: ", Vector2{10, (float)ctx->screenHeight - 60}, 24, 1, DARKGREEN);
    DrawText(posText, 80, static_cast<int>(ctx->screenHeight) - 60, 24, DARKGREEN);
    
    //显示玩家属性
    char hpText[64];
    std::sprintf(hpText, "%d/%d", ctx->player.stats.hp, ctx->player.stats.maxHp);
    DrawTextEx(ctx->mainFont, "HP: ", Vector2{10, (float)ctx->screenHeight - 40}, 24, 1, RED);
    DrawText(hpText, 80, static_cast<int>(ctx->screenHeight) - 40, 24, RED);
    
    //显示运行时间
    char timeText[64];
    std::sprintf(timeText, "%.1fs", expd->elapsedTime);
    DrawTextEx(ctx->mainFont, "时间: ", Vector2{10, (float)ctx->screenHeight - 20}, 20, 1, DARKGRAY);
    DrawText(timeText, 80, static_cast<int>(ctx->screenHeight) - 20, 20, DARKGRAY);
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
