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
// 【【【GDD v3.9c 合同：“实现” GUI】】】
// (我们“定义” RAYGUI_IMPLEMENTATION，
//  来“命令” raygui.h 在“这里”施工它的“厨房”)
#define RAYGUI_IMPLEMENTATION
#include "../lib/raylib/include/raygui.h"

#include "GameState.h"
#include "ExplorationState.h"
#include "TitleScreenState.h"  //标题屏幕
#include "Player.h"
#include "Map.h"
#include "Enemy.h"          //未完成敌人模块
#include "FontLoader.h"     //字体加载
#include <cstdio>
#include <cstring>

// 前向声明 Windows API 函数，避免包含整个 windows.h
#ifdef _WIN32
extern "C" {
    __declspec(dllimport) int __stdcall SetConsoleOutputCP(unsigned int);
}
#define CP_UTF8 65001
#endif

int main(void)
{
#ifdef _WIN32
    // 设置控制台输出为 UTF-8 编码，解决中文日志乱码问题
    SetConsoleOutputCP(CP_UTF8);
#endif
    
    //==========初始化窗口==========
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    
    InitWindow(screenWidth, screenHeight, "LLW Project - Exploration State Test");
    SetTargetFPS(60);
    
    //==========创建游戏上下文==========
    GameContext ctx;
    TraceLog(LOG_INFO, "[Main] GameContext 创建后，默认尺寸: %.0fx%.0f", ctx.screenWidth, ctx.screenHeight);
    
    ctx.screenWidth = screenWidth;
    ctx.screenHeight = screenHeight;
    ctx.isRunning = true;
    
    TraceLog(LOG_INFO, "[Main] 赋值后，屏幕尺寸: %.0fx%.0f", ctx.screenWidth, ctx.screenHeight);
    
    //==========初始化玩家==========
    ctx.player.gridX = 2;  // 修改为安全位置（草地区域）
    ctx.player.gridY = 2;  // 修改为安全位置（草地区域）
    ctx.player.currentDirection = PLAYER_DIR_DOWN; // 【P1 修复】默认朝下，防止未初始化导致的花屏 Bug
    ctx.player.moveSpeed = 200.0f; // 移动速度：200像素/秒
    ctx.player.isMoving = false;   // 初始状态：静止
    ctx.player.visualPosition = { (float)(ctx.player.gridX * 32), (float)(ctx.player.gridY * 32) }; // 初始视觉位置与网格位置对齐
    ctx.player.moveTarget = ctx.player.visualPosition; // 目标位置初始化为当前位置
    ctx.player.stats.hp = 100;
    ctx.player.stats.maxHp = 100;
    ctx.player.stats.attack = 15;
    ctx.player.stats.defense = 8;
    
    TraceLog(LOG_INFO, "[Main] 玩家初始化完成 - 位置: (%d, %d), HP: %d/%d", 
             ctx.player.gridX, ctx.player.gridY, 
             ctx.player.stats.hp, ctx.player.stats.maxHp);
   
    //===========================从文件加载地图
    // 尝试多个可能的路径（支持从build目录或项目根目录运行）
    const char* possiblePaths[] = {
        "res/data/maps/home.txt",
        "../res/data/maps/home.txt",
        "../../res/data/maps/home.txt"
    };
    
    bool mapLoaded = false;
    for (int i = 0; i < 3; i++) {
        if (LoadMap(ctx, possiblePaths[i])) {
            TraceLog(LOG_INFO, "[Main] 地图加载成功，路径: %s", possiblePaths[i]);
            mapLoaded = true;
            break;
        }
    }
    
    if (!mapLoaded) {
        TraceLog(LOG_ERROR, "[Main] 地图加载失败! 请确保从项目根目录运行程序");
        TraceLog(LOG_ERROR, "[Main] 当前工作目录: %s", GetWorkingDirectory());
        
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("ERROR: 地图文件加载失败!", 50, 50, 30, RED);
        DrawText("请从项目根目录运行程序，或检查 res/data/maps/home.txt 是否存在", 50, 100, 20, RED);
        DrawText("5秒后自动退出...", 50, 150, 20, YELLOW);
        EndDrawing();
        
        WaitTime(5.0);  // 等待5秒让用户看到错误信息
        CloseWindow();
        return -1;
    }

    TraceLog(LOG_INFO, "[Main] 地图加载成功 - 大小：%dx%d", ctx.width, ctx.height);

    // 调试：检查玩家初始位置的地形
    if (ctx.player.gridY < (int)ctx.tiles.size() && 
        ctx.player.gridX < (int)ctx.tiles[ctx.player.gridY].size()) {
        int tileValue = (int)ctx.tiles[ctx.player.gridY][ctx.player.gridX];
        TraceLog(LOG_INFO, "[Main] 玩家位置 (%d,%d) 的地形类型: %d (0=EMPTY, 1=GRASS, 2=WALL)", 
                 ctx.player.gridX, ctx.player.gridY, tileValue);
        
        // 调试：打印玩家周围的地形
        TraceLog(LOG_INFO, "[Main] 调试 - 打印地图前3行前10列:");
        for (int y = 0; y < 3 && y < (int)ctx.tiles.size(); y++) {
            char rowStr[256] = "";
            for (int x = 0; x < 10 && x < (int)ctx.tiles[y].size(); x++) {
                char temp[8];
                sprintf(temp, "%d ", (int)ctx.tiles[y][x]);
                strcat(rowStr, temp);
            }
            TraceLog(LOG_INFO, "[Main]   行%d: %s", y, rowStr);
        }
    }

    // 设置地图的宽度、高度和瓦片大小
    ctx.height = static_cast<int>(ctx.tiles.size());
    ctx.width = (ctx.height > 0) ? static_cast<int>(ctx.tiles[0].size()) : 0;
    ctx.tileSize = 32; // 与 Player.cpp 中的 TILE_SIZE 保持一致
    
    TraceLog(LOG_INFO, "[Main] 地图初始化完成 - 大小: %dx%d, 瓦片大小: %d", 
             ctx.width, ctx.height, ctx.tileSize);
    
    //==========加载地图纹理==========
    LoadMapTextures(ctx);
    TraceLog(LOG_INFO, "[Main] 地图纹理加载完成");
    
    //==========加载字体==========
    loadGameFont(ctx);
    TraceLog(LOG_INFO, "[Main] 游戏字体加载完成");
    
    //==========加载玩家资源==========
    TraceLog(LOG_INFO, "[Main] 准备加载玩家资源，当前屏幕尺寸: %.0fx%.0f", ctx.screenWidth, ctx.screenHeight);
    LoadPlayerAssets(ctx);
    TraceLog(LOG_INFO, "[Main] 玩家精灵图加载完成");
    
    //==========初始化状态机==========
    GameStateMachine_init(&ctx.state_machine);
    TraceLog(LOG_INFO, "[Main] 状态机初始化完成");
    
    //==========创建并进入标题屏幕状态==========
    GameState* titleScreenState = CreateTitleScreenState();//11/18添加标题
    if (titleScreenState) {
        GameStateMachine_change(&ctx.state_machine, &ctx, titleScreenState);
        TraceLog(LOG_INFO, "[Main] 成功进入标题屏幕状态");
    } else {
        TraceLog(LOG_ERROR, "[Main] 创建标题屏幕状态失败！");
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
    
    UnloadPlayerAssets(ctx);
    TraceLog(LOG_INFO, "[Main] 玩家资源已释放");
    
    unloadGameFont(ctx);
    TraceLog(LOG_INFO, "[Main] 字体资源已释放");
    
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
