// ==============================================================================
// 这是一个“自包含”的【【【演示程序】】】
// 它“假装”是 main.cpp, GameState.h, Map.h, Map.cpp
// 唯一的“真实”代码是你的 Player.cpp
//
// 编译它： g++ main.cpp Player.cpp FontLoader.cpp -o game.exe -lraylib ...
// ==============================================================================

#include "raylib.h"
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

// ===========================================
// 【【【模拟 GameState.h (队友 C)】】】
// ===========================================
struct Player; // 前向声明

// 0=地板, 1=墙
// (在真实的 GDD v2.6 中，这在 Map.h 里)
enum TileType {
    FLOOR = 0,
    WALL = 1
};

struct GameEngine {
    GameState currentState;
    Font mainFont;
    Camera2D camera;
    Player player; // 使用“实体”，不是指针
    vector<vector<TileType>> currentMapData;
};

// ===========================================
// 【【【模拟 Map.h (队友 A)】】】
// ===========================================
void loadMap(GameEngine& engine, const char* fileName);
void drawMap(const GameEngine& engine);


// ===========================================
// 【【【模拟 FontLoader.h (你)】】】
// ===========================================
void loadGameFont(GameEngine& engine);
void unloadGameFont(GameEngine& engine);


// ===========================================
// 【【【模拟 Player.h (你)】】】
// ===========================================
struct Stats { int hp, maxHp, attack, defense; };
struct Player { int gridX, gridY; Stats stats; };
void updatePlayer(GameEngine& engine);
void drawPlayer(const GameEngine& engine);


// ===========================================
// 【【【【【【 你的 Player.cpp 在这里 】】】】】
// (把你的 Player.cpp 文件的内容【完整】粘贴到这里)
// (或者在 G++ 编译时把它链接进来)
// ===========================================
// #include "Player.cpp" // (这是一个“偷懒”的 C++ 技巧，不推荐，但演示有效)
// (为了演示，我把你的 P0 代码粘贴进来)

const int TILE_SIZE = 32;

void updatePlayer(GameEngine& engine){
    int nextX = engine.player.gridX;
    int nextY = engine.player.gridY;

    if(IsKeyPressed(KEY_W)){ nextY--; }
    else if(IsKeyPressed(KEY_S)){ nextY++; }
    else if(IsKeyPressed(KEY_A)){ nextX--; }
    else if(IsKeyPressed(KEY_D)){ nextX++; }

    bool canMove = false; 

    // (【【已修复】】的“安全”碰撞检测)
    if(nextY >= 0 && nextY < engine.currentMapData.size() &&
       nextX >= 0 && nextX < engine.currentMapData[nextY].size())
    {
        if(engine.currentMapData[nextY][nextX] == FLOOR) // 0 = 地板
        {
            canMove = true;
        }
    }

    if(canMove){
        engine.player.gridX = nextX;
        engine.player.gridY = nextY;
    }
}

void drawPlayer(const GameEngine& engine){
    Camera2D camera = engine.camera;
    camera.zoom = 3.0f;
    camera.offset = { 1920 / 2.0f, 1080 / 2.0f };
    camera.target = { 
        (float)(engine.player.gridX * TILE_SIZE) + (TILE_SIZE / 2.0f),
        (float)(engine.player.gridY * TILE_SIZE) + (TILE_SIZE / 2.0f)
    };

    BeginMode2D(camera);

        // 【【【调用“模拟”的 drawMap】】】
        drawMap(engine); 

        DrawRectangle(engine.player.gridX * TILE_SIZE, engine.player.gridY * TILE_SIZE, TILE_SIZE, TILE_SIZE, RED);

    EndMode2D();
    
    // (P1 绘制 UI)
    DrawText("LLW P0 Demo", 10, 10, 20, WHITE);
}


// ===========================================
// 【【【模拟 Map.cpp (队友 A)】】】
// (这是一个“最小存根”实现)
// ===========================================
void loadMap(GameEngine& engine, const char* fileName)
{
    // (P0 演示：我们“硬编码”一个地图，而不读文件)
    engine.currentMapData.clear();
    engine.currentMapData = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 0, 0, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 0, 0, 1, 1, 0, 0, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 1, 0, 0, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };
}

void drawMap(const GameEngine& engine)
{
    // (GDD v2.5 合同：“愚蠢”的绘制器)
    for (int y = 0; y < engine.currentMapData.size(); y++) {
        for (int x = 0; x < engine.currentMapData[y].size(); x++) {
            if (engine.currentMapData[y][x] == WALL) {
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, GRAY);
            } else {
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, DARKGRAY);
            }
        }
    }
}


// ===========================================
// 【【【模拟 FontLoader.cpp (你)】】】
// (这是一个“最小存根”实现)
// ===========================================
void loadGameFont(GameEngine& engine) {
    // (P0 演示：我们只加载“默认”字体，跳过中文)
    engine.mainFont = GetFontDefault();
    TraceLog(LOG_INFO, "[FontLoader] Stub: Using default font.");
}
void unloadGameFont(GameEngine& engine) {
    // (默认字体不需要卸载)
}


// ===========================================
// 【【【“真实”的 main.cpp (队友 C)】】】
// (这就是“GDD 合同”的粘合剂)
// ===========================================
int main()
{
    // --- GDD v2.5 规范：1080p 窗口 ---
    InitWindow(1920, 1080, "LLW P0 Core (Demo)");
    SetTargetFPS(60);

    // --- GDD v2.5 规范：创建“中央数据库” ---
    GameEngine engine;
    
    // --- GDD v2.5 规范：初始化 ---
    engine.currentState = STATE_MAP;
    engine.player.gridX = 5; // 出生点
    engine.player.gridY = 5;
    engine.camera = { 0 }; // 初始化摄像机
    
    // --- GDD v2.5 规范：调用“模块” ---
    loadGameFont(engine); // 调用“你”的字体模块
    loadMap(engine, "demo.map"); // 调用“队友A”的地图模块
    
    // --- GDD v2.5 规范：游戏循环 ---
    while (!WindowShouldClose())
    {
        // --- 1. 更新 (Update) ---
        switch (engine.currentState)
        {
            case STATE_MAP:
                updatePlayer(engine); // 【【【调用“你”的 update】】】
                break;
            // case STATE_COMBAT:
                // updateCombat(engine); 
                // break;
        }

        // --- 2. 绘制 (Draw) ---
        BeginDrawing();
        ClearBackground(BLACK); // “核心”负责清屏

        switch (engine.currentState)
        {
            case STATE_MAP:
                drawPlayer(engine); // 【【【调用“你”的 draw (渲染总指挥)】】】
                break;
            // case STATE_COMBAT:
                // drawCombat(engine);
                // break;
        }

        EndDrawing();
    }

    // --- GDD v2.5 规范：清理 ---
    unloadGameFont(engine); // 调用“你”的清理
    CloseWindow();
    
    return 0;
}