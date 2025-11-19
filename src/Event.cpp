#include "Event.h"
#include "GameState.h"
#include "Dialogue.h"
#include "Map.h"

void CheckAndExecuteEvents(GameContext &ctx){
    for(const auto&event : ctx.gameEvents){
        if(!event.isTrigged){
            switch(event.eventType){
                case(DIALOGUE):{
                    LoadDialogueScript(event.dialogue.scriptPath.c_str());
                    break;
                }
                case(TELEPORT):{
                    TraceLog(LOG_INFO,"[Event] 传送到地图 %s",event.portal.targetMap.c_str());

                    // 1. 切换地图
                    LoadLevelFromTiled(ctx,event.portal.targetMap.c_str());

                    //2. 设置玩家位置
                    ctx.player.gridX = (int)(event.portal.targetPosition.x / 32);
                    ctx.player.gridY = (int)(event.portal.targetPosition.y / 32);
                    ctx.player.visualPosition = event.portal.targetPosition;

                    // 3. 重置摄像机 (可选，防止画面瞬间跳变)
                    ctx.camera.target = ctx.player.visualPosition;
                    break;
                }
            }
        }
    }

}