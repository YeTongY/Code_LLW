#pragma once
#include "GameState.h"

/**
 * @brief 用于加载字体
 * 
 * @param engine Gamestate里的engine核心数据文件
 */
void loadGameFont (GameEngine &engine);

/**
 * @brief 卸载已烘焙的字体
 * 
 * @param engine Gamestate里的engine核心数据文件
 */
void unloadGameFont (GameEngine &engine);