#pragma once
#include "GameState.h"

/**
 * @brief 用于加载字体
 * 
 * @param ctx Gamestate里的GameContext核心数据文件
 */
void loadGameFont (GameContext &ctx);

/**
 * @brief 卸载已烘焙的字体
 * 
 * @param ctx Gamestate里的GameContext核心数据文件
 */
void unloadGameFont (GameContext &ctx);


/**
 * @brief (P1) 绘制“辉光”字体 (GDD v3.9)
 * (这是一个“工具”函数，它会“画两次”来实现“辉光”)
 * @param font 
 * @param text 
 * @param position “主”文本的位置
 * @param fontSize 
 * @param spacing 
 * @param textColor “主”文本的颜色 (e.g., WHITE)
 * @param glowColor “辉光”的颜色 (e.g., CYAN)
 */
void DrawTextGlow(
    Font font, 
    const char* text, 
    Vector2 position, 
    float fontSize, 
    float spacing, 
    Color textColor, 
    Color glowColor
);