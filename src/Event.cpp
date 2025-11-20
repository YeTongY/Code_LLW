#include "Event.h"
#include "GameState.h"
#include "Dialogue.h"
#include "Map.h"


EventTriggerType CheckEvents(GameContext &ctx, Rectangle area, EventTriggerType type){
    Rectangle playerBounds = {
        ctx.player.visualPosition.x,
        ctx.player.visualPosition.y - 32,
        static_cast<float>(32),
        static_cast<float>(32)
    };

    switch (type)
    {
    case ON_INTERACT:{
        if(CheckCollisionRecs(playerBounds, area) && IsKeyPressed(KEY_E)){
            return ON_INTERACT;
        }
        break;
    }
    case ON_AUTO_START:{
        return ON_AUTO_START;
        break;
    }
    case ON_ENTER_ZONE:{
        if(CheckCollisionRecs(playerBounds, area)){
            return ON_ENTER_ZONE;
        }
        break;
    }
    default:
        TraceLog(LOG_WARNING,"[Event Checker] 传入的触发类型无效！");
        break;
    }
}











void ExecuteEvents(GameContext& ctx)
{
    for (auto& event : ctx.gameEvents)
    {
        if (event.isTrigged)
        {
            continue;
        }

        switch (event.eventType)
        {
            case DIALOGUE:
            {
                if (event.dialogue.empty())
                {
                    TraceLog(LOG_WARNING, "[Event] 对话事件未配置对话数据");
                    break;
                }

                const std::string& scriptPath = event.dialogue.front().scriptPath;
                if (scriptPath.empty())
                {
                    TraceLog(LOG_WARNING, "[Event] 对话事件缺少脚本路径，事件无法触发");
                    break;
                }

                auto script = LoadDialogueScript(scriptPath.c_str());
                if (script.empty())
                {
                    TraceLog(LOG_WARNING, "[Event] 对话脚本为空或加载失败: %s", scriptPath.c_str());
                    break;
                }

                GameState* dialogueState = createDialogueState(script);
                if (!dialogueState)
                {
                    TraceLog(LOG_ERROR, "[Event] 无法创建对话状态: %s", scriptPath.c_str());
                    break;
                }

                






                
                event.isTrigged = true;
                break;
            }

            case TELEPORT:
            {
                if (event.portal.empty())
                {
                    TraceLog(LOG_WARNING, "[Event] 传送事件缺少目的地数据");
                    break;
                }

                const EventData_Portal& portal = event.portal.front();
                if (portal.targetMap.empty())
                {
                    TraceLog(LOG_WARNING, "[Event] 传送事件缺少 targetMap 属性");
                    break;
                }

                TraceLog(LOG_INFO, "[Event] 传送到地图 %s", portal.targetMap.c_str());

                if (!LoadLevelFromTiled(ctx, portal.targetMap.c_str()))
                {
                    TraceLog(LOG_ERROR, "[Event] 地图切换失败: %s", portal.targetMap.c_str());
                    break;
                }

                Vector2 teleportPos = portal.targetPosition;
                if (teleportPos.x == 0.0f && teleportPos.y == 0.0f)
                {
                    TraceLog(LOG_WARNING, "[Event] 传送事件未指定 targetPosition，使用玩家当前位置作为兜底");
                    teleportPos = ctx.player.visualPosition;
                }

                int tileSize = ctx.tileSize > 0 ? ctx.tileSize : 32;
                ctx.player.visualPosition = teleportPos;
                ctx.player.moveTarget = teleportPos;
                ctx.player.gridX = static_cast<int>(teleportPos.x / tileSize);
                ctx.player.gridY = static_cast<int>(teleportPos.y / tileSize);
                ctx.camera.target = teleportPos;

                event.isTrigged = true;
                break;
            }

            default:
                TraceLog(LOG_INFO, "[Event] 未实现的事件类型: %d", event.eventType);
                break;
        }
    }
}