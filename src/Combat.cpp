#include "Combat.h"
#include "raylib.h"
#include "GameState.h"

/**
 * @brief (P0) 战斗状态的“更新”逻辑
 * @param engine 游戏引擎的引用
 */
void updateCombat(GameContext& ctx){
    if(IsKeyPressed(KEY_ENTER)){
        //TODO 在这里退出战斗状态进入大地图状态
    }
}

/**
 * @brief (P0) 战斗状态的“绘制”逻辑
 * @param engine 游戏引擎的引用 (const)
 */
void drawCombat(const GameContext& ctx){
    ClearBackground(BLACK);// 初始化屏幕为黑屏
    DrawTextEx(ctx.mainFont,"战斗开始！",{ctx.screenWidth/2.0f,ctx.screenHeight/2.0f},48,2,WHITE);
    DrawTextEx(ctx.mainFont,"按ENTER退出",{ctx.screenWidth/2.0f - 200.0f, ctx.screenHeight/2.0f + 100.0f},32,2,GRAY);
}
