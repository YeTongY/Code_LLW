#include "Event.h"
#include "GameState.h"
#include "Dialogue.h"
#include "Map.h"

EventTriggerType CheckEvents(GameContext &ctx, Rectangle area, EventTriggerType type)
{
    float tileSize = ctx.tileSize > 0 ? static_cast<float>(ctx.tileSize) : 32.0f;

    Rectangle playerBounds = {
        ctx.player.visualPosition.x,
        ctx.player.visualPosition.y - tileSize,
        tileSize,
        tileSize};

    switch (type)
    {
    case ON_INTERACT:
    {
        if (CheckCollisionRecs(playerBounds, area) && IsKeyPressed(KEY_E))
        {
            return ON_INTERACT;
        }
        break;
    }
    case ON_AUTO_START:
    {
        return ON_AUTO_START;
        break;
    }
    case ON_ENTER_ZONE:
    {
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

                auto script = LoadDialogueScript(scriptPath.c_str());
                if (script.empty())
                {
                    TraceLog(LOG_WARNING, "[Event] 对话脚本为空或加载失败: %s", scriptPath.c_str());
                    continue;
                }

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
                if (event.portal.empty())
                {
                    TraceLog(LOG_WARNING, "[Event] 传送事件缺少目的地数据");
                    continue;
                }

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

                return;
            }

            default:
                TraceLog(LOG_INFO, "[Event] 未实现的事件类型: %d", event.eventType);
                event.isTrigged = true;
                continue;
            }
        }
    }
}