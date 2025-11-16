/*
 * LLW 项目 - 探索状态功能测试
 * 2025/11/16
 * 
 * 测试内容：
 * - 状态机系统
 * - 探索状态（玩家移动、摄像机跟随、UI显示）
 * - 玩家模块（WASD移动、碰撞检测）
 * - 地图模块（地图渲染）
 */

#include "raylib.h"
#include "GameState.h"
#include "ExplorationState.h"
#include "Player.h"
#include "Map.h"
#include <cstdio>

int main(void)
{
    //==========初始化窗口==========
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    
    InitWindow(screenWidth, screenHeight, "LLW Project - Exploration State Test");
    SetTargetFPS(60);
    
    //==========创建游戏上下文==========
    GameContext ctx;
    ctx.screenWidth = screenWidth;
    ctx.screenHeight = screenHeight;
    ctx.isRunning = true;
    
    //==========初始化玩家==========
    ctx.player.gridX = 5;
    ctx.player.gridY = 5;
    ctx.player.stats.hp = 100;
    ctx.player.stats.maxHp = 100;
    ctx.player.stats.attack = 15;
    ctx.player.stats.defense = 8;
    
    TraceLog(LOG_INFO, "[Main] 玩家初始化完成 - 位置: (%d, %d), HP: %d/%d", 
             ctx.player.gridX, ctx.player.gridY, 
             ctx.player.stats.hp, ctx.player.stats.maxHp);
    
    //==========创建测试地图==========
    // 创建一个 20x15 的测试地图
    // EMPTY = 可行走的地板
    // WALL = 墙壁（不可行走）
    ctx.tiles = {
        {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL},
        {WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL},
        {WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL},
        {WALL, EMPTY, EMPTY, WALL, WALL, WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, WALL, WALL, EMPTY, EMPTY, EMPTY, WALL},
        {WALL, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, WALL},
        {WALL, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, WALL},
        {WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, WALL, WALL, WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL},
        {WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL},
        {WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL},
        {WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, WALL, WALL, WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL},
        {WALL, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, WALL},
        {WALL, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, EMPTY, EMPTY, EMPTY, WALL},
        {WALL, EMPTY, EMPTY, WALL, WALL, WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL, WALL, WALL, EMPTY, EMPTY, EMPTY, WALL},
        {WALL, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, WALL},
        {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}
    };
    
    // 设置地图的宽度、高度和瓦片大小
    ctx.height = static_cast<int>(ctx.tiles.size());
    ctx.width = (ctx.height > 0) ? static_cast<int>(ctx.tiles[0].size()) : 0;
    ctx.tileSize = 32; // 与 Player.cpp 中的 TILE_SIZE 保持一致
    
    TraceLog(LOG_INFO, "[Main] 地图初始化完成 - 大小: %dx%d, 瓦片大小: %d", 
             ctx.width, ctx.height, ctx.tileSize);
    
    //==========初始化状态机==========
    GameStateMachine_init(&ctx.state_machine);
    TraceLog(LOG_INFO, "[Main] 状态机初始化完成");
    
    //==========创建并进入探索状态==========
    GameState* explorationState = createExplorationState();
    if (explorationState) {
        GameStateMachine_change(&ctx.state_machine, &ctx, explorationState);
        TraceLog(LOG_INFO, "[Main] 成功进入探索状态");
    } else {
        TraceLog(LOG_ERROR, "[Main] 创建探索状态失败！");
        CloseWindow();
        return -1;
    }
    
    //==========显示测试说明==========
    TraceLog(LOG_INFO, "========================================");
    TraceLog(LOG_INFO, "      探索状态功能测试");
    TraceLog(LOG_INFO, "========================================");
    TraceLog(LOG_INFO, "操作说明:");
    TraceLog(LOG_INFO, "  WASD - 移动玩家");
    TraceLog(LOG_INFO, "  E    - 触发交互（测试）");
    TraceLog(LOG_INFO, "  ESC  - 退出游戏");
    TraceLog(LOG_INFO, "");
    TraceLog(LOG_INFO, "测试内容:");
    TraceLog(LOG_INFO, "  1. 玩家网格移动");
    TraceLog(LOG_INFO, "  2. 碰撞检测（墙壁阻挡）");
    TraceLog(LOG_INFO, "  3. 摄像机跟随");
    TraceLog(LOG_INFO, "  4. UI 信息显示");
    TraceLog(LOG_INFO, "  5. 玩家属性显示");
    TraceLog(LOG_INFO, "========================================");
    
    //==========主游戏循环==========
    float elapsedTime = 0.0f;
    int frameCount = 0;
    
    //====================初始化调用map纹理加载
    LoadMapTextures(ctx);

    while (ctx.isRunning && !WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        elapsedTime += deltaTime;
        frameCount++;
        
        //每秒输出一次性能信息
        if (frameCount % 60 == 0) {
            TraceLog(LOG_INFO, "[Main] FPS: %d, 玩家位置: (%d, %d)", 
                     GetFPS(), ctx.player.gridX, ctx.player.gridY);
        }
        
        //==========更新==========
        GameStateMachine_update(&ctx.state_machine, &ctx, deltaTime);
        
        //==========渲染==========
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        //渲染当前状态
        GameStateMachine_render(&ctx.state_machine, &ctx);
        
        //绘制额外的调试信息
        DrawText("Exploration State Test", 10, 120, 14, DARKPURPLE);
        DrawFPS(10, 140);
        
        //绘制版本信息
        DrawText("LLW Project v0.1", ctx.screenWidth - 150, ctx.screenHeight - 25, 14, LIGHTGRAY);
        
        EndDrawing();
    }
    
    //==========清理资源==========
    TraceLog(LOG_INFO, "[Main] 开始清理资源...");
    
    GameStateMachine_shutdown(&ctx.state_machine, &ctx);
    TraceLog(LOG_INFO, "[Main] 状态机已关闭");
    
    CloseWindow();
    TraceLog(LOG_INFO, "[Main] 窗口已关闭");
    
    //==========输出测试总结==========
    TraceLog(LOG_INFO, "========================================");
    TraceLog(LOG_INFO, "      测试结束");
    TraceLog(LOG_INFO, "========================================");
    TraceLog(LOG_INFO, "运行时间: %.2f 秒", elapsedTime);
    TraceLog(LOG_INFO, "总帧数: %d", frameCount);
    TraceLog(LOG_INFO, "平均 FPS: %.1f", frameCount / elapsedTime);
    TraceLog(LOG_INFO, "玩家最终位置: (%d, %d)", ctx.player.gridX, ctx.player.gridY);
    TraceLog(LOG_INFO, "========================================");
    
    return 0;
}
