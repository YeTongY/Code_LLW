#include "FontLoader.h"
#include <fstream>   // 包含 C++ 文件库
#include <sstream>   // 包含 C++ 字符串流库
#include <string>    // 包含 C++ 字符串库
#include <raylib.h>
#include "../lib/raylib/include/raygui.h"

using namespace std;


/**
 * @brief 仅存在于FontLoader中的辅助函数，用于读取文本文件
 * 
 * @param text_address 要读取的文本文件的地址
 * @return string 返回读取完成的字符串
 */
static string textLoader(const char* text_address){

    //1.输入文件流
    ifstream file(text_address);

    // 2. 检查文件是否成功打开 (和 C 语言的 if (file == NULL) 一样重要)
    if (!file.is_open()) {
        // TraceLog 是 raylib 的日志功能，比 printf 好
        TraceLog(LOG_WARNING, "[Loader] Failed to open file: %s", text_address);
        return ""; // 返回一个空字符串，表示失败
    }

    // 3. 创建一个“字符串流” (stringstream)
    stringstream buffer;

    buffer << file.rdbuf();//从文件流中读取文字，存放到buffer
    file.close();//完成对文件的读取，关闭文件

    return buffer.str();//返回读取完成后的字符串

}

/**
 * @brief 用于加载字体
 * 
 * @param ctx Gamestate里的GameContext核心数据文件
 */
void loadGameFont (GameContext &ctx){
    // 尝试多个可能的路径（支持从不同目录运行）
    const char* fontPaths[] = {
        "res/data/fonts/SourceHanSansSC-Regular.otf",
        "../res/data/fonts/SourceHanSansSC-Regular.otf",
        "../../res/data/fonts/SourceHanSansSC-Regular.otf"
    };
    
    const char* textPaths[] = {
        "res/data/dialogue/all_game_script.txt",
        "../res/data/dialogue/all_game_script.txt",
        "../../res/data/dialogue/all_game_script.txt"
    };
    
    // 查找文本文件
    string allGameText = "";
    for (int i = 0; i < 3; i++) {
        allGameText = textLoader(textPaths[i]);
        if (!allGameText.empty()) {
            TraceLog(LOG_INFO, "[FontLoader] Text file loaded from: %s", textPaths[i]);
            break;
        }
    }
    
    if (allGameText.empty()) {
        TraceLog(LOG_WARNING, "[FontLoader] Could not load text file, using default character set");
    }
    
    allGameText += "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!?.,:;-()";//给全文本文件添加基本英文字符以防止潜在的错误
    
    int codepointCount = 0; //初始化码点数量
    int* codepoints = LoadCodepoints(allGameText.c_str(), &codepointCount);//从文本文档里读取全部码点
    TraceLog(LOG_INFO, "[FontLoader] Found %d unique codepoints in script file.", codepointCount);//读取码点的日志输出

    // 尝试加载字体文件
    bool fontLoaded = false;
    for (int i = 0; i < 3; i++) {
        ctx.mainFont = LoadFontEx(
            fontPaths[i],
            48,
            codepoints,
            codepointCount
        );
        
        if (ctx.mainFont.texture.id != 0) {
            TraceLog(LOG_INFO, "[FontLoader] Font loaded successfully from: %s", fontPaths[i]);
            fontLoaded = true;
            break;
        }
    }
    
    UnloadCodepoints(codepoints); //释放码点
    
    if (!fontLoaded) {
        TraceLog(LOG_ERROR, "[FontLoader] FAILED to load font from all paths!");
        // 使用raylib默认字体作为后备
        ctx.mainFont = GetFontDefault();
        TraceLog(LOG_WARNING, "[FontLoader] Using default font as fallback");
    }

}

/**
 * @brief 卸载已烘焙的文件
 * 
 * @param ctx Gamestate里的GameContext核心数据文件
 */
void unloadGameFont (GameContext &ctx){
    if(ctx.mainFont.texture.id != 0){//检查字体是否被成功烘焙
        UnloadFont(ctx.mainFont);
        TraceLog(LOG_INFO, "[FontLoader] Unloaded game font.");
    }
}

/**
 * @brief 绘制“辉光”字体
 */
void DrawTextGlow(
    Font font, 
    const char* text, 
    Vector2 position, 
    float fontSize, 
    float spacing, 
    Color textColor, // (e.g., WHITE)
    Color glowColor  // (e.g., CYAN)
)
{
    // “辉光层”
    
    // 1. 计算“辉光”的位置
    Vector2 glowPos = { position.x + 2.0f, position.y + 2.0f };
    
    // (Fade() 来自 raygui.h)
    Color glowTint = Fade(glowColor, 0.4f); // (0.4f = 40% 透明度)

    // 3. 绘制“辉光层”
    DrawTextEx(
        font,
        text,
        glowPos,
        fontSize,
        spacing,
        glowTint  // <-- 使用“半透明”的辉光色
    );

    // 【【【P1 施工：Task 2 - “文字层” (Pass 2)】】】
    
    // 4. 绘制“主文本层”
    DrawTextEx(
        font,
        text,
        position, // <-- 使用“精确”的位置
        fontSize,
        spacing,
        textColor // <-- 使用“不透明”的主颜色
    );
}