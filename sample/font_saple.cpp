/*
 * LLW 项目 - 字体加载示例
 * 这是一个测试程序，用于演示如何使用 FontLoader 模块加载字体
 */

#include "raylib.h"
#include "../src/GameState.h"   // 引入 GameEngine 结构
#include "../src/FontLoader.h"  // 引入字体加载函数

int main(void)
{
    // ========== 初始化窗口 ==========
    const int screenWidth = 800;   // 窗口宽度（像素）
    const int screenHeight = 600;  // 窗口高度（像素）
    
    InitWindow(screenWidth, screenHeight, "LLW Project - FontLoader Test");
    
    // ========== 使用新的加载器加载字体 ==========
    // 1. 创建一个 GameEngine 实例
    GameEngine engine = {0}; 
    
    // 2. 调用封装好的函数来加载字体
    //    这个函数会自动处理所有逻辑：读取脚本、提取码点、烘焙字体
    loadGameFont(engine);
    
    // ========== 游戏循环设置 ==========
    SetTargetFPS(60);  // 设置目标帧率为 60 FPS（每秒 60 帧）
    
    // ========== 主游戏循环 ==========
    while (!WindowShouldClose())
    {
        // --- 更新逻辑 ---
        // (无)
        
        // --- 绘制画面 ---
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            
            // 根据字体是否成功加载，显示不同的内容
            if (engine.mainFont.texture.id != 0) {
                // 字体加载成功
                DrawText("Font loaded successfully using FontLoader!", 10, 10, 20, DARKGREEN);
                
                // 使用 engine.mainFont 绘制文本
                DrawTextEx(engine.mainFont, "欢迎来到 LLW 项目！", (Vector2){180, 200}, 32, 2, LIGHTGRAY);
                DrawTextEx(engine.mainFont, "Welcome to LLW Project!", (Vector2){180, 250}, 30, 2, GRAY);
                DrawTextEx(engine.mainFont, "字体由 FontLoader 模块加载", (Vector2){150, 350}, 24, 2, DARKGRAY);
                DrawTextEx(engine.mainFont, "这是 Raylib 渲染的图形", (Vector2){270, 540}, 20, 1, DARKBLUE);

            } else {
                // 字体加载失败
                DrawText("Font failed to load using FontLoader!", 10, 10, 20, RED);
                DrawText("Check file paths in FontLoader.cpp and run from project root.", 10, 40, 20, RED);
            }
            
            DrawText("Press ESC to exit", 280, 400, 20, DARKGRAY);
            DrawCircle(400, 500, 30, BLUE);
        
        EndDrawing();
    }
    
    // ========== 清理资源并退出 ==========
    // 调用封装好的函数来卸载字体
    unloadGameFont(engine);
    
    CloseWindow();
    
    return 0;
}
