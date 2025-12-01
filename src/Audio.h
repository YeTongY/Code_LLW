#pragma once
#include "GameState.h"

/**
 * @brief 根据当前状态播放或停止脚步循环音效
 *
 * @param ctx 游戏核心数据库，包含玩家和音频信息。
 */
void PlayFootstepSound(GameContext &ctx);

/**
 * @brief 立刻终止脚步循环音效并重置内部状态
 */
void StopFootstepSound(GameContext &ctx);


/**
 * @brief 播放探索背景音乐
 */
void PlayExplorationBGM(GameContext &ctx);

/**
 * @brief 停止探索背景音乐
 */
void StopExplorationBGM(GameContext &ctx);