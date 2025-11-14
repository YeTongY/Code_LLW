/*
 * LLW 项目 - 主程序入口
 * 这是一个测试程序，用于验证 Raylib 和编译环境是否正常工作
 * 同时演示如何在 Raylib 中加载和使用中文字体
 */

#include "raylib.h"
#include <vector>  // C++ 动态数组容器，比 malloc 更安全方便

int main(void)
{
    // ========== 初始化窗口 ==========
    const int screenWidth = 800;   // 窗口宽度（像素）
    const int screenHeight = 600;  // 窗口高度（像素）
    
    // 创建一个 800x600 的窗口，标题为 "LLW Project - Test Window"
    InitWindow(screenWidth, screenHeight, "LLW Project - Test Window");
    
    // ========== 加载中文字体 ==========
    // Raylib 默认字体只支持 ASCII 字符（英文字母、数字、标点），不支持中文
    // 要显示中文，必须：
    // 1. 从系统加载一个包含中文的字体文件（如宋体、黑体）
    // 2. 告诉 Raylib 要加载哪些字符（通过 Unicode 码点数组）
    
    // 使用 C++ 的 vector 容器来存储 Unicode 码点
    // vector 的优点：
    // - 自动管理内存，不需要手动 malloc/free
    // - 可以动态增长，不需要预先计算大小
    // - 更安全，不会忘记释放内存导致内存泄漏
    std::vector<int> codepoints;
    
    // ========== 第一步：加载所有可打印的 ASCII 字符 ==========
    // ASCII 码 32-126 包括：空格、字母 A-Z/a-z、数字 0-9、标点符号等
    for (int i = 32; i <= 126; i++) {
        codepoints.push_back(i);  // 用 push_back 向 vector 末尾添加元素
    }
    
    // ========== 第二步：从我们要显示的中文文本中提取字符 ==========
    // 定义一个包含所有我们要显示的中文的字符串
    const char* chineseText = "欢迎来到LLW项目！如果你看到中文，说明字体加载成功这是Raylib渲染的图形";
    
    // 调用 Raylib 的函数，从这个字符串中提取所有字符的 Unicode 码点
    int chineseCount = 0;  // 将会存储找到了多少个字符
    int *chineseCodepoints = LoadCodepoints(chineseText, &chineseCount);
    
    // 将提取出的中文字符码点添加到 vector 中
    for (int i = 0; i < chineseCount; i++) {
        codepoints.push_back(chineseCodepoints[i]);
    }
    
    // 释放临时的中文码点数组（Raylib 函数返回的指针仍需手动释放）
    UnloadCodepoints(chineseCodepoints);
    
    // ========== 第三步：从系统字体文件加载字体 ==========
    Font chineseFont = {0};  // 字体对象，初始化为空
    bool fontLoaded = false; // 标记字体是否成功加载
    
    // 尝试加载多个中文字体，按优先级尝试（哪个先成功就用哪个）
    const char* fontPaths[] = {
        "C:/Windows/Fonts/simhei.ttf",   // 黑体（粗体，适合标题）
        "C:/Windows/Fonts/simsun.ttc",   // 宋体（Windows 自带，兼容性好）
        "C:/Windows/Fonts/msyh.ttc",     // 微软雅黑（现代、美观）
        "C:/Windows/Fonts/simkai.ttf"    // 楷体（书法风格）
    };
    
    // 循环尝试加载这些字体，直到成功为止
    for (int i = 0; i < 4 && !fontLoaded; i++) {
        // LoadFontEx 参数说明：
        // - 第1个：字体文件路径
        // - 第2个：字体大小（像素，这里是 48）
        // - 第3个：要加载的字符码点数组（vector.data() 获取底层数组指针）
        // - 第4个：数组长度（vector.size() 获取元素个数）
        chineseFont = LoadFontEx(fontPaths[i], 48, codepoints.data(), codepoints.size());
        
        // 检查字体是否加载成功（通过检查纹理 ID 是否不为 0）
        fontLoaded = (chineseFont.texture.id != 0);
    }
    
    // vector 会在离开作用域时自动释放内存，无需手动 free
    
    // ========== 游戏循环设置 ==========
    SetTargetFPS(60);  // 设置目标帧率为 60 FPS（每秒 60 帧）
    
    // ========== 主游戏循环 ==========
    // 这个循环会一直运行，直到用户关闭窗口或按下 ESC 键
    while (!WindowShouldClose())
    {
        // --- 更新逻辑 ---
        // （这里还没有游戏逻辑代码，以后会添加）
        
        // --- 绘制画面 ---
        BeginDrawing();  // 开始绘制本帧画面
        
            ClearBackground(RAYWHITE);  // 用白色清空整个屏幕
            
            // 根据字体是否成功加载，显示不同的内容
            if (fontLoaded) {
                // 字体加载成功的情况
                
                // 在左上角显示成功提示（用 Raylib 默认字体，支持英文）
                DrawText("Font loaded successfully!", 10, 10, 20, GREEN);
                
                // 使用我们加载的中文字体绘制文本
                // DrawTextEx 参数说明：
                // - 第1个：字体对象
                // - 第2个：要显示的文本
                // - 第3个：位置坐标 {x, y}
                // - 第4个：字体大小
                // - 第5个：字符间距
                // - 第6个：颜色
                DrawTextEx(chineseFont, "欢迎来到 LLW 项目！", (Vector2){180, 200}, 32, 2, LIGHTGRAY);
                DrawTextEx(chineseFont, "Welcome to LLW Project!", (Vector2){180, 250}, 30, 2, GRAY);
                
                // 绘制说明信息
                DrawTextEx(chineseFont, "如果你看到中文，说明字体加载成功！", (Vector2){150, 350}, 24, 2, DARKGRAY);
                DrawTextEx(chineseFont, "这是 Raylib 渲染的图形", (Vector2){270, 540}, 20, 1, DARKBLUE);
            } else {
                // 字体加载失败的情况（降级方案）
                DrawText("Font failed to load!", 10, 10, 20, RED);
                DrawText("Using default font (no Chinese support)", 10, 40, 20, RED);
                DrawText("Welcome to LLW Project!", 190, 200, 30, LIGHTGRAY);
            }
            
            // 绘制退出提示（用默认字体）
            DrawText("Press ESC to exit", 280, 400, 20, DARKGRAY);
            
            // 绘制一个测试图形：蓝色圆形
            DrawCircle(400, 500, 30, BLUE);
        
        EndDrawing();  // 结束本帧绘制
    }
    
    // ========== 清理资源并退出 ==========
    // 如果字体加载成功了，需要释放字体占用的内存
    if (fontLoaded) {
        UnloadFont(chineseFont);
    }
    
    // 关闭窗口并清理 Raylib 资源
    CloseWindow();
    
    return 0;  // 程序正常退出
}
