#include "Event.h"
#include "GameState.h"
#include "Dialogue.h"
#include "Map.h"

//------------------------------------------------------------------------------
// Event.cpp 实现了游戏事件的触发与执行逻辑。
// 主要功能包括：
// 1. 检查玩家是否满足事件触发条件。
// 2. 根据事件类型执行相应的逻辑。
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CheckEvents 函数
// 功能：检查指定触发区域是否满足触发条件。
// 参数：
// - ctx: 游戏上下文，包含玩家状态、地图信息等。
// - area: 事件触发区域。
// - type: 触发类型（如 ON_INTERACT, ON_AUTO_START 等）。
// 返回值：满足条件的触发类型，若无触发则返回 ON_NONE。
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ExecuteEvents 函数
// 功能：遍历事件列表并执行满足条件的事件逻辑。
// 逻辑：
// - 检查事件是否已触发，若是且为一次性事件则跳过。
// - 根据事件类型执行相应逻辑：
//   - DIALOGUE: 加载对话脚本并切换到对话状态。
//   - TELEPORT: 切换地图并更新玩家位置。
//   - 其他类型：记录日志并标记事件已触发。
//------------------------------------------------------------------------------

EventTriggerType CheckEvents(GameContext &ctx, Rectangle area, EventTriggerType type)
{
    // 玩家碰撞盒使用当前地图的像素尺寸，若 tileSize 异常则回退到 32
    float tileSize = ctx.tileSize > 0 ? static_cast<float>(ctx.tileSize) : 32.0f;

    Rectangle playerBounds = {
        ctx.player.gridX * tileSize,
        ctx.player.gridY * tileSize,
        tileSize,
        tileSize};

    switch (type)
    {
    case ON_INTERACT:
    {
        // 玩家进入区域且按下交互键才触发
        if (CheckCollisionRecs(playerBounds, area) && IsKeyPressed(KEY_E))
        {
            return ON_INTERACT;
        }
        break;
    }
    case ON_AUTO_START:
    {
        // 自动触发事件在检测阶段直接返回
        return ON_AUTO_START;
        break;
    }
    case ON_ENTER_ZONE:
    {
        // 仅需进入区域即可触发
        if (CheckCollisionRecs(playerBounds, area))
        {
            return ON_ENTER_ZONE;
        }
        break;
    }
    default:
        TraceLog(LOG_WARNING, "[Event Checker] 传入的触发类型无效！");
        break;
    }
    return ON_NONE;
}

//------------------------------------------------------------------------------
// 遍历事件列表并执行满足条件的事件逻辑
//------------------------------------------------------------------------------
void ExecuteEvents(GameContext &ctx)
{
    for (auto &event : ctx.gameEvents)
    {
        if (event.isTrigged && event.isOneShot)
        {
            continue;
        }

        if (event.triggerType == CheckEvents(ctx, event.area, event.triggerType))
        {
            switch (event.eventType)
            {
            case DIALOGUE:
            {
                // 保护性检查：确保配置完整
                if (event.dialogue.empty())
                {
                    TraceLog(LOG_WARNING, "[Event] 对话事件未配置对话数据");
                    continue;
                }

                const std::string &scriptPath = event.dialogue.front().scriptPath;
                if (scriptPath.empty())
                {
                    TraceLog(LOG_WARNING, "[Event] 对话事件缺少脚本路径，事件无法触发");
                    continue;
                }

                // 将脚本加载为对话行
                auto script = LoadDialogueScript(scriptPath.c_str());
                if (script.empty())
                {
                    TraceLog(LOG_WARNING, "[Event] 对话脚本为空或加载失败: %s", scriptPath.c_str());
                    continue;
                }

                // 切换到对话状态并标记事件
                GameState *dialogueState = createDialogueState(script);
                if (!dialogueState)
                {
                    TraceLog(LOG_ERROR, "[Event] 无法创建对话状态: %s", scriptPath.c_str());
                    continue;
                }

                GameStateMachine_change(&ctx.state_machine, &ctx, dialogueState);
                event.isTrigged = true;
                continue;
            }

            case TELEPORT:
            {
                // 保护性检查：至少需要一个目的地配置
                if (event.portal.empty())
                {
                    TraceLog(LOG_WARNING, "[Event] 传送事件缺少目的地数据");
                    continue;
                }

                // 复制出第一个传送目的地，避免切图后引用失效
                const EventData_Portal portal = event.portal.front();
                if (portal.targetMap.empty())
                {
                    TraceLog(LOG_WARNING, "[Event] 传送事件缺少 targetMap 属性");
                    continue;
                }

                TraceLog(LOG_INFO, "[Event] 传送到地图 %s", portal.targetMap.c_str());

                if (!LoadLevelFromTiled(ctx, portal.targetMap.c_str()))
                {
                    TraceLog(LOG_ERROR, "[Event] 地图切换失败: %s", portal.targetMap.c_str());
                    continue;
                }

                // 落点优先使用显式坐标，缺少时取触发区域中心
                Vector2 teleportPos = portal.targetPosition;
                if (teleportPos.x == 0.0f && teleportPos.y == 0.0f)
                {
                    teleportPos = {
                        portal.bounds.x + portal.bounds.width * 0.5f,
                        portal.bounds.y + portal.bounds.height * 0.5f
                    };
                }

                // 更新玩家与相机状态
                float tile = ctx.tileSize > 0 ? static_cast<float>(ctx.tileSize) : 32.0f;
                ctx.player.visualPosition = teleportPos;
                ctx.player.moveTarget = teleportPos;
                ctx.player.gridX = static_cast<int>(teleportPos.x / tile);
                ctx.player.gridY = static_cast<int>(teleportPos.y / tile);
                ctx.camera.target = teleportPos;

                // 地图已经切换完毕，此帧不再继续处理旧事件
                return;
            }

            case TELEPORT_WITHOUT_LOADING:
            {
                // 保护性检查：至少需要一个目的地配置
                if (event.portal.empty())
                {
                    TraceLog(LOG_WARNING, "[Event] 传送事件缺少目的地数据");
                    continue;
                }

                // 复制出第一个传送目的地
                const EventData_Portal portal = event.portal.front();

                TraceLog(LOG_INFO, "[Event] 传送到位置 (%.2f, %.2f)", portal.targetPosition.x, portal.targetPosition.y);

                // 更新玩家与相机状态
                Vector2 teleportPos = portal.targetPosition;
                if (teleportPos.x == 0.0f && teleportPos.y == 0.0f)
                {
                    teleportPos = {
                        portal.bounds.x + portal.bounds.width * 0.5f,
                        portal.bounds.y + portal.bounds.height * 0.5f
                    };
                }

                float tile = ctx.tileSize > 0 ? static_cast<float>(ctx.tileSize) : 32.0f;
                ctx.player.visualPosition = teleportPos;
                ctx.player.moveTarget = teleportPos;
                ctx.player.gridX = static_cast<int>(teleportPos.x / tile);
                ctx.player.gridY = static_cast<int>(teleportPos.y / tile);
                ctx.camera.target = teleportPos;

                event.isTrigged = true;
                continue;
            }

            case RECOVER:{
                const std::string &scriptPath = "res/data/dialogue/Special/Recover.csv";
                auto script = LoadDialogueScript(scriptPath.c_str());
                if (script.empty())
                {
                    TraceLog(LOG_WARNING, "[Event] 恢复脚本为空或加载失败: %s", scriptPath.c_str());
                    continue;
                }
                // 恢复玩家生命值
                ctx.player.stats.hp = ctx.player.stats.maxHp;
                // 恢复玩家魔法值
                ctx.player.stats.mp = ctx.player.stats.maxMp;
                // 切换到对话状态并标记事件
                GameState *dialogueState = createDialogueState(script);
                if (!dialogueState)
                {
                    TraceLog(LOG_ERROR, "[Event] 无法创建对话状态: %s", scriptPath.c_str());
                    continue;
                }
                GameStateMachine_change(&ctx.state_machine, &ctx, dialogueState);
                event.isTrigged = true;
                continue;
            }

            case HIDDEN:
            {
                const std::string &scriptPath = "res/data/dialogue/Special/Hidden.csv";
                auto script = LoadDialogueScript(scriptPath.c_str());
                if (script.empty())
                {
                    TraceLog(LOG_WARNING, "[Event] 隐藏剧本为空或加载失败: %s", scriptPath.c_str());
                    continue;
                }
                ctx.player.stats.maxMp += 20;
                ctx.player.stats.mp = ctx.player.stats.maxMp;
                event.isTrigged = true;
                continue;
            }

            default:
                TraceLog(LOG_INFO, "[Event] 未实现的事件类型: %d", event.eventType);
                event.isTrigged = true;
                continue;
            }
        }
    }
}