#include "raylib.h"  // 新增：显式引入 raylib 音频接口
#include "Audio.h"

// 立即停止脚步循环音效，并复位相关标记
void StopFootstepSound(GameContext &ctx)
{
    // 如果音效未加载，直接复位标记
    if (!IsMusicValid(ctx.footstepLoop)) {
        ctx.isFootstepLooping = false;
        ctx.footstepIdleTimer = 0.0f;
        return;
    }

    // 如果循环正在播放，立即停止
    if (IsMusicStreamPlaying(ctx.footstepLoop)) {
        StopMusicStream(ctx.footstepLoop);
        TraceLog(LOG_TRACE, "[Audio] 强制停止脚步循环");
    }

    // 复位循环标记和计时器
    ctx.isFootstepLooping = false;
    ctx.footstepIdleTimer = 0.0f;
}

// 每帧调用：根据移动状态控制脚步循环播放
void PlayFootstepSound(GameContext &ctx)
{
    if (!IsMusicValid(ctx.footstepLoop)) {
        return; // 音效未加载时直接返回，防止播放失败
    }

    // 如果禁用了脚步音效（例如战斗中），强制停止
    if (!ctx.enableFootstepAudio) {
        StopFootstepSound(ctx);
        return;
    }

    // 获取本帧时间差
    const float dt = GetFrameTime();
    // 停止移动后的缓冲时间（秒），防止瞬间停动产生杂音
    constexpr float idleGraceSeconds = 0.1f;

    // 玩家移动时启动或继续脚步循环
    // 玩家移动时启动或继续脚步循环
    if (ctx.player.isMoving)
    {
        // 首次启动循环
        if (!ctx.isFootstepLooping)
        {
            TraceLog(LOG_INFO, "[Audio] 启动脚步循环音效");
            ctx.footstepLoop.looping = true; // 确保持续循环
            PlayMusicStream(ctx.footstepLoop);
            ctx.isFootstepLooping = true;
        }

        // 重新开始移动时，清零缓冲计时器
        ctx.footstepIdleTimer = 0.0f;
    }
    // 停止移动后保留一段缓冲再关闭循环
    // 玩家停止移动，但循环仍在播放
    else if (ctx.isFootstepLooping)
    {
        // 累积停动时间
        ctx.footstepIdleTimer += dt;

        // 超过缓冲时间阈值后才真正停止循环
        if (ctx.footstepIdleTimer >= idleGraceSeconds)
        {
            TraceLog(LOG_TRACE, "[Audio] 玩家停止移动，关闭脚步循环");
            StopMusicStream(ctx.footstepLoop);
            ctx.isFootstepLooping = false;
            ctx.footstepIdleTimer = 0.0f;
        }
    }

    // 每帧更新音乐流缓冲区，保证连续播放
    if (ctx.isFootstepLooping)
    {
        UpdateMusicStream(ctx.footstepLoop);
    }
}
void PlayExplorationBGM(GameContext &ctx)
{
    // 如果音效未加载，直接返回
    if (!IsMusicValid(ctx.explorationBGM)) {
        return;
    }

    // 如果已经在播放，直接返回
    if (IsMusicStreamPlaying(ctx.explorationBGM)) {
        return;
    }

    // 设置为循环播放并启动
    ctx.explorationBGM.looping = true;
    PlayMusicStream(ctx.explorationBGM);
    ctx.isExplorationBGMPlaying = true;
    TraceLog(LOG_INFO, "[Audio] 启动探索背景音乐");
}

void StopExplorationBGM(GameContext &ctx)
{
    // 如果音效未加载，直接返回
    if (!IsMusicValid(ctx.explorationBGM)) {
        return;
    }

    // 如果正在播放，则停止
    if (IsMusicStreamPlaying(ctx.explorationBGM)) {
        StopMusicStream(ctx.explorationBGM);
        ctx.isExplorationBGMPlaying = false;
        TraceLog(LOG_INFO, "[Audio] 停止探索背景音乐");
    }
}

void PlayCombatBGM(GameContext &ctx)
{
    // 如果音效未加载，直接返回
    if (!IsMusicValid(ctx.combatBGM)) {
        return;
    }
    // 如果已经在播放，直接返回
    if (IsMusicStreamPlaying(ctx.combatBGM)) {
        return;
    }
    // 设置为循环播放并启动
    ctx.combatBGM.looping = true;
    PlayMusicStream(ctx.combatBGM);
    ctx.isCombatBGMPlaying = true;
    TraceLog(LOG_INFO, "[Audio] 启动战斗背景音乐");
}

void StopCombatBGM(GameContext &ctx)
{
    // 如果音效未加载，直接返回
    if (!IsMusicValid(ctx.combatBGM)) {
        return;
    }

    // 如果正在播放，则停止
    if (IsMusicStreamPlaying(ctx.combatBGM)) {
        StopMusicStream(ctx.combatBGM);
        ctx.isCombatBGMPlaying = false;
        TraceLog(LOG_INFO, "[Audio] 停止战斗背景音乐");
    }
}