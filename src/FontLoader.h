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