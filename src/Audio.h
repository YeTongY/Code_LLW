#pragma once
#include "GameState.h"

extern float stepTimer;          // 计时器
extern float stepInterval;     // 间隔时间：每 0.4 秒响一次

/**
 * @brief 根据移动状态播放脚步声
 * 
 * @param ctx 游戏核心数据库，包含玩家和音频信息。
 */
void PlayFootstepSound(GameContext &ctx);