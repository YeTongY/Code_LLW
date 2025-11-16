//====2025/11/16====Warning编写“探索状态”
#ifndef EXPLORATIONSTATE_H
#define EXPLORATIONSTATE_H

#include "GameState.h"

//探索状态数据
struct ExplorationData 
{
    bool isActive;       //是否激活
    float elapsedTime;   //已运行时间
};

//状态生命周期函数
void exploration_enter(GameContext* ctx, void* state_data);
void exploration_exit(GameContext* ctx, void* state_data);
void exploration_update(GameContext* ctx, void* state_data);
void exploration_render(GameContext* ctx, void* state_data);

//探索状态创建函数
GameState* createExplorationState();

#endif //EXPLORATIONSTATE_H