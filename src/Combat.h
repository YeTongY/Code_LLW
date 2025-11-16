#pragma once
#include "GameState.h" // 【合同】必须包含 GDD 核心

/**
 * @brief (P0) 战斗状态的“更新”逻辑
 * @param engine 游戏引擎的引用
 */
void updateCombat(GameContext& ctx);

/**
 * @brief (P0) 战斗状态的“绘制”逻辑
 * @param engine 游戏引擎的引用 (const)
 */
void drawCombat(const GameContext& ctx);