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
#include "Event.h"          //事件系统
#include "UI.h"             //UI资源加载
#include "Audio.h"          //音频模块
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

    SetTraceLogLevel(LOG_WARNING); // 设置日志级别为 WARNING 及以上
    SetTargetFPS(60);
    
    //==========创建游戏上下文==========
    GameContext ctx;
    ctx.currentCombatant = nullptr;
    ctx.combatBackground = Texture2D{0};
    ctx.level0CombatDialogueTriggered = false;
    TraceLog(LOG_INFO, "[Main] GameContext 创建后，默认尺寸: %.0fx%.0f, isRunning=%d", ctx.screenWidth, ctx.screenHeight, ctx.isRunning);
    
    ctx.screenWidth = screenWidth;
    ctx.screenHeight = screenHeight;
    ctx.isRunning = true;
    
    TraceLog(LOG_INFO, "[Main] 赋值后，屏幕尺寸: %.0fx%.0f, isRunning=%d", ctx.screenWidth, ctx.screenHeight, ctx.isRunning);
    
    //==========初始化玩家==========
    ctx.player.gridX = 1;  // 修改为安全位置（草地区域）
    ctx.player.gridY = 26;  // 修改为安全位置（草地区域）
    ctx.player.currentDirection = PLAYER_DIR_DOWN; // 【P1 修复】默认朝下，防止未初始化导致的花屏 Bug
    ctx.player.moveSpeed = 200.0f; // 移动速度：200像素/秒
    ctx.player.isMoving = false;   // 初始状态：静止
    ctx.player.visualPosition = { (float)(ctx.player.gridX * 32), (float)(ctx.player.gridY * 32) }; // 初始视觉位置与网格位置对齐
    ctx.player.moveTarget = ctx.player.visualPosition; // 目标位置初始化为当前位置
    ctx.player.stats.hp = 150;
    ctx.player.stats.maxHp = 150;
    ctx.player.stats.attack = 17;
    ctx.player.stats.defense = 10;
    
    TraceLog(LOG_INFO, "[Main] 玩家初始化完成 - 位置: (%d, %d), HP: %d/%d, isRunning=%d", 
             ctx.player.gridX, ctx.player.gridY, 
             ctx.player.stats.hp, ctx.player.stats.maxHp, ctx.isRunning);
   
    //===========================从 Tiled 文件加载地图
    // 注意：程序从项目根目录运行，直接使用 res/ 路径
    const char* possiblePaths[] = {
        "res/Level 0.tmx",
        "../res/Level 0.tmx",
        "../../res/Level 0.tmx"
    };
    
    bool mapLoaded = false;
    for (int i = 0; i < 3; i++) {
        if (LoadLevelFromTiled(ctx, possiblePaths[i])) {
            TraceLog(LOG_INFO, "[Main] Tiled 地图加载成功，路径: %s", possiblePaths[i]);
            mapLoaded = true;
            break;
        }
    }
    
    if (!mapLoaded) {
        TraceLog(LOG_ERROR, "[Main] Tiled 地图加载失败! 请确保从项目根目录运行程序");
        TraceLog(LOG_ERROR, "[Main] 当前工作目录: %s", GetWorkingDirectory());
        
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("ERROR: Tiled 地图文件加载失败!", 50, 50, 30, RED);
        DrawText("请从项目根目录运行程序，或检查 res/Level 1.tmx 是否存在", 50, 100, 20, RED);
        DrawText("5秒后自动退出...", 50, 150, 20, YELLOW);
        EndDrawing();
        
        WaitTime(5.0);  // 等待5秒让用户看到错误信息
        CloseWindow();
        return -1;
    }

    TraceLog(LOG_INFO, "[Main] 地图加载成功 - 大小：%dx%d, isRunning=%d", ctx.width, ctx.height, ctx.isRunning);

    //==========初始化事件==========
    //创建事件
    GameEvent npcEvent;

    //初始化事件属性
    


    

    // LoadLevelFromTiled 已经加载了地图数据、tileset 纹理
    // 不需要再手动设置尺寸或加载纹理
    
    TraceLog(LOG_INFO, "[Main] 地图初始化完成 - 大小: %dx%d, 瓦片大小: %d", 
             ctx.width, ctx.height, ctx.tileSize);
    
    //==========加载字体==========
    loadGameFont(ctx);
    TraceLog(LOG_INFO, "[Main] 游戏字体加载完成, isRunning=%d", ctx.isRunning);
    
    //==========加载玩家资源==========
    TraceLog(LOG_INFO, "[Main] 准备加载玩家资源，当前屏幕尺寸: %.0fx%.0f", ctx.screenWidth, ctx.screenHeight);
    LoadPlayerAssets(ctx);
    TraceLog(LOG_INFO, "[Main] 玩家精灵图加载完成, isRunning=%d", ctx.isRunning);

    //==========加载敌人资源==========
    LoadEnemyAssets(ctx);
    TraceLog(LOG_INFO, "[Main] 敌人资源加载完成, isRunning=%d", ctx.isRunning);
    
    //==========加载UI资源==========
    LoadUIAssets(ctx);
    TraceLog(LOG_INFO, "[Main] UI资源加载完成, isRunning=%d", ctx.isRunning);

    //==========加载战斗背景图==========
    const char* combatBackgroundCandidates[] = {
        "res/graphics/battebackground/L0_Battle_Background_Blur.png",
        "../res/graphics/battebackground/L0_Battle_Background_Blur.png",
        "../../res/graphics/battebackground/L0_Battle_Background_Blur.png"
    };

    for (const char* path : combatBackgroundCandidates) {
        Texture2D candidate = LoadTexture(path);
        if (candidate.id != 0) {
            ctx.combatBackground = candidate;
            TraceLog(LOG_INFO, "[Main] 战斗背景图加载成功: %s", path);
            break;
        }
        TraceLog(LOG_WARNING, "[Main] 战斗背景图加载失败: %s", path);
    }

    if (ctx.combatBackground.id == 0) {
        TraceLog(LOG_ERROR, "[Main] 未能加载战斗背景图，战斗画面将使用纯色背景");
    }

    //==========初始化音频设备==========
    InitAudioDevice(); // 初始化音频设备

    //==========加载音频资源==========
    const char* footstepCandidates[] = {
        "../res/audio/sfx/footstep00.wav",
        "res/audio/sfx/footstep00.wav",
        "../../res/audio/sfx/footstep00.wav"
    }; // 修改：多路径容错，兼容不同工作目录

    ctx.footstepLoop = Music{0};
    for (const char* path : footstepCandidates) {
        Music candidate = LoadMusicStream(path);
        if (IsMusicValid(candidate)) {
            candidate.looping = true; // 确保资源本身标记为循环
            ctx.footstepLoop = candidate;
            TraceLog(LOG_INFO, "[Audio] 脚步循环加载成功: %s", path);
            break;
        }
        TraceLog(LOG_WARNING, "[Audio] 脚步循环加载失败: %s", path);
    }

    if (!IsMusicValid(ctx.footstepLoop)) {
        TraceLog(LOG_ERROR, "[Audio] 无法加载脚步循环音效，后续将无法播放脚步声");
    } else {
        SetMusicVolume(ctx.footstepLoop, 0.8f);
        TraceLog(LOG_INFO, "[Audio] 脚步循环参数: frameCount=%u, sampleRate=%d", ctx.footstepLoop.frameCount, ctx.footstepLoop.stream.sampleRate);
    }


    //============加载探索背景音乐==========
    const char* explorationBGMCandidates[] = {
        "../res/audio/music/tartarus_0d04 (P3R ver.).wav",
        "res/audio/music/tartarus_0d04 (P3R ver.).wav",
        "../../res/audio/music/tartarus_0d04 (P3R ver.).wav"
    };

    ctx.explorationBGM = Music{0};
    for (const char* path : explorationBGMCandidates) {
        Music candidate = LoadMusicStream(path);
        if (IsMusicValid(candidate)) {
            candidate.looping = true; // 确保资源本身标记为循环
            ctx.explorationBGM = candidate;
            TraceLog(LOG_INFO, "[Audio] 探索背景音乐加载成功: %s", path);
            break;
        }
        TraceLog(LOG_WARNING, "[Audio] 探索背景音乐加载失败: %s", path);
    }

    if (!IsMusicValid(ctx.explorationBGM)) {
        TraceLog(LOG_ERROR, "[Audio] 无法加载探索背景音乐，后续将无法播放背景音乐");
    } else {
        SetMusicVolume(ctx.explorationBGM, 0.15f); // 默认音量调低，避免遮盖效果音
        TraceLog(LOG_INFO, "[Audio] 探索背景音乐参数: frameCount=%u, sampleRate=%d", ctx.explorationBGM.frameCount, ctx.explorationBGM.stream.sampleRate);
    }
    
    //============加载战斗背景音乐==========
    const char* combatBGMCandidates[] = {
        "../res/audio/music/A CLUE.wav",
        "res/audio/music/A CLUE.wav",
        "../../res/audio/music/A CLUE.wav"
    };

    ctx.combatBGM = Music{0};
    for (const char* path : combatBGMCandidates) {
        Music candidate = LoadMusicStream(path);
        if (IsMusicValid(candidate)) {
            candidate.looping = true; // 确保资源本身标记为循环
            ctx.combatBGM = candidate;
            TraceLog(LOG_INFO, "[Audio] 战斗背景音乐加载成功: %s", path);
            break;
        }
        TraceLog(LOG_WARNING, "[Audio] 战斗背景音乐加载失败: %s", path);
    }

    if (!IsMusicValid(ctx.combatBGM)) {
        TraceLog(LOG_ERROR, "[Audio] 无法加载战斗背景音乐，后续将无法播放背景音乐");
    } else {
        SetMusicVolume(ctx.combatBGM, 0.3f);
        TraceLog(LOG_INFO, "[Audio] 战斗背景音乐参数: frameCount=%u, sampleRate=%d", ctx.combatBGM.frameCount, ctx.combatBGM.stream.sampleRate);
    }


    //==========初始化状态机==========
    GameStateMachine_init(&ctx.state_machine);
    TraceLog(LOG_INFO, "[Main] 状态机初始化完成, isRunning=%d", ctx.isRunning);
    
    //==========创建并进入标题屏幕状态==========
    GameState* titleScreenState = CreateTitleScreenState();//11/18添加标题
    if (titleScreenState) {
        TraceLog(LOG_INFO, "[Main] 切换状态前 isRunning = %d", ctx.isRunning);
        GameStateMachine_change(&ctx.state_machine, &ctx, titleScreenState);
        TraceLog(LOG_INFO, "[Main] 成功进入标题屏幕状态");
        TraceLog(LOG_INFO, "[Main] 切换状态后 isRunning = %d", ctx.isRunning);
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

    

    TraceLog(LOG_INFO, "[Main] 准备进入主循环，isRunning = %d, WindowShouldClose = %d", ctx.isRunning, WindowShouldClose());
    while (ctx.isRunning && !WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        elapsedTime += deltaTime;
        frameCount++;
        
        //==========更新==========
        GameStateMachine_update(&ctx.state_machine, &ctx, deltaTime);
        
        //==========渲染==========
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        //渲染当前状态
        GameStateMachine_render(&ctx.state_machine, &ctx);
        
        //绘制额外的调试信息
        DrawFPS(10, 140);
        
        //绘制版本信息
        DrawText("LLW Project v0.1", ctx.screenWidth - 150, ctx.screenHeight - 25, 14, LIGHTGRAY);
        
        EndDrawing();

        //=========播放脚步声=========
        PlayFootstepSound(ctx);
    }
    
    //==========清理资源==========
    TraceLog(LOG_INFO, "[Main] 开始清理资源...");
    
    UnloadPlayerAssets(ctx);
    TraceLog(LOG_INFO, "[Main] 玩家资源已释放");
    
    UnloadUIAssets(ctx);
    TraceLog(LOG_INFO, "[Main] UI资源已释放");

    if (ctx.combatBackground.id != 0) {
        UnloadTexture(ctx.combatBackground);
        TraceLog(LOG_INFO, "[Main] 战斗背景图资源已释放");
    }
    
    unloadGameFont(ctx);
    TraceLog(LOG_INFO, "[Main] 字体资源已释放");
    
    GameStateMachine_shutdown(&ctx.state_machine, &ctx);
    TraceLog(LOG_INFO, "[Main] 状态机已关闭");
    
    CloseWindow();
    TraceLog(LOG_INFO, "[Main] 窗口已关闭");

    if (IsMusicValid(ctx.footstepLoop)) {
        if (IsMusicStreamPlaying(ctx.footstepLoop)) {
            StopMusicStream(ctx.footstepLoop);
        }
        UnloadMusicStream(ctx.footstepLoop);
        TraceLog(LOG_INFO, "[Main] 脚步循环资源已释放");
    }

    if (IsMusicValid(ctx.explorationBGM)) {
        if (IsMusicStreamPlaying(ctx.explorationBGM)) {
            StopMusicStream(ctx.explorationBGM);
        }
        UnloadMusicStream(ctx.explorationBGM);
        TraceLog(LOG_INFO, "[Main] 探索背景音乐资源已释放");
    }
     
    CloseAudioDevice(); // 关闭音频设备
    TraceLog(LOG_INFO, "[Main] 音频设备已关闭");
    
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
