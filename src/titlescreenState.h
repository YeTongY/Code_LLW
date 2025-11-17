//2025/11/17 Warning开始编写游戏菜单头文件

#ifndef TITLESCREENSTATE_H
#define TITLESCREENSTATE_H

#include "GameState.h"

struct titlescreendata
{
    int titlescreenOption; // 0：开始游戏 1：退出游戏
};

//========状态生命周期函数
void titlescreen_enter(GameContext* ctx, void* data);
void titlescreen_exit(GameContext* ctx, void* data);
void titlescreen_update(GameContext* ctx, void* data);
void titlescreen_render(GameContext* ctx, void* data);

GameState* CreateTitleScreenState();

#endif //TITLESCREENSTATE_H