#include "raylib.h"  // 新增：显式引入 raylib 音频接口
#include "Audio.h"

void StopFootstepSound(GameContext &ctx)
{
    if (!IsMusicValid(ctx.footstepLoop)) {
        ctx.isFootstepLooping = false;
        ctx.footstepIdleTimer = 0.0f;
        return;
    }

    if (IsMusicStreamPlaying(ctx.footstepLoop)) {
        StopMusicStream(ctx.footstepLoop);
        TraceLog(LOG_TRACE, "[Audio] 强制停止脚步循环");
    }

    ctx.isFootstepLooping = false;
    ctx.footstepIdleTimer = 0.0f;
}

void PlayFootstepSound(GameContext &ctx)
{
    if (!IsMusicValid(ctx.footstepLoop)) {
        return; // 音效未加载时直接返回，防止播放失败
    }

    if (!ctx.enableFootstepAudio) {
        StopFootstepSound(ctx);
        return;
    }

    const float dt = GetFrameTime();
    constexpr float idleGraceSeconds = 0.1f;

    if (ctx.player.isMoving)
    {
        if (!ctx.isFootstepLooping)
        {
            TraceLog(LOG_INFO, "[Audio] 启动脚步循环音效");
            ctx.footstepLoop.looping = true; // 确保持续循环
            PlayMusicStream(ctx.footstepLoop);
            ctx.isFootstepLooping = true;
        }

        ctx.footstepIdleTimer = 0.0f;
    }
    else if (ctx.isFootstepLooping)
    {
        ctx.footstepIdleTimer += dt;

        if (ctx.footstepIdleTimer >= idleGraceSeconds)
        {
            TraceLog(LOG_TRACE, "[Audio] 玩家停止移动，关闭脚步循环");
            StopMusicStream(ctx.footstepLoop);
            ctx.isFootstepLooping = false;
            ctx.footstepIdleTimer = 0.0f;
        }
    }

    if (ctx.isFootstepLooping)
    {
        UpdateMusicStream(ctx.footstepLoop); // 维持流式播放
    }
}
