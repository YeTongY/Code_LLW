#include "GameState.h"
#include <cstdlib>
#include <cstring>

//===================状态机管理器实现（C++ 风格）
void GameStateMachine_init(GameStateMachine* sm)
{
    sm->current_state = nullptr;
    sm->pending_state = nullptr;
}

void GameStateMachine_change(GameStateMachine* sm, GameContext* ctx, GameState* newstate)
{
    //退出当前状态
    if (sm->current_state && sm->current_state->exit)
    {
        sm->current_state->exit(ctx, sm->current_state->data);
    }

    sm->current_state = newstate;

    //进入新状态
    if (sm->current_state && sm->current_state->enter)
    {
        sm->current_state->enter(ctx, sm->current_state->data);
    }
}

void GameStateMachine_update(GameStateMachine* sm, GameContext* ctx, float dt)
{
    //处理延迟状态切换
    if (sm->pending_state)
    {
        GameStateMachine_change(sm, ctx, sm->pending_state);
        sm->pending_state = nullptr;
    }

    //更新当前状态
    if (sm->current_state && sm->current_state->update)
    {
        sm->current_state->update(ctx, sm->current_state->data);
    }
}

void GameStateMachine_render(GameStateMachine* sm, GameContext* ctx)
{
     if (sm->current_state && sm->current_state->render)
     {
        sm->current_state->render(ctx, sm->current_state->data);
     }
}

void GameStateMachine_shutdown(GameStateMachine* sm, GameContext* ctx)
{
    if (sm->current_state)
    {
        GameStateMachine_change(sm, ctx, nullptr);
    }
}

//=============================状态创建
GameState* Gamestate_create(
    StateEnterFunc enter,
    StateExitFunc exit,
    StateUpdateFunc update,
    StateRenderFunc render,
    void* data,
    size_t data_size
)
{
    GameState* state = new GameState();
    if (!state) return nullptr;

    state->enter = enter;
    state->exit = exit;
    state->update = update;
    state->render = render;
    state->data = data;
    state->datasize = data_size;

    return state;
}

//=========================状态销毁（C++ 风格）
void GameState_destory(GameState* state)
{
    if (state)
    {
        // 注意：data 的释放由各个状态自己负责（new 分配的用 delete）
        delete state;
    }
}
