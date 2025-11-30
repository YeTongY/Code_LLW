#pragma once
#include "Audio.h"

void PlayFootstepSound(GameContext &ctx)
{

    if (ctx.player.isMoving)
    {
        //计时器累加
        stepTimer += GetFrameTime();

        //如果时间到了间隔点
        if (stepTimer >= stepInterval)
        {
            // --- 播放声音 ---
            SetSoundPitch(ctx.footstepfx, 0.9f + ((float)GetRandomValue(0, 20) / 100.0f)); // 加上刚才说的随机音高
            PlaySound(ctx.footstepfx);

            // --- 重置计时器 ---
            stepTimer = 0.0f;
        }
    }
    else
    {
        // 如果停下来了，把计时器设为“满的”
        // 这样下次起步时，第一脚会立刻响，没有延迟
        stepTimer = stepInterval;
    }
}
