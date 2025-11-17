#pragma once

#include "raylib.h"
#include "GameState.h"

/**
 * @brief 加载UI资产
 * 
 * @param ctx 核心数据结构体
 */
void LoadUIAssets(GameContext& ctx);



/**
 * @brief 卸载UI资产
 * 
 * @param ctx 核心数据结构体
 */
void UnloadUIAssets(GameContext& ctx);