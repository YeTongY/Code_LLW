//2025/11/17 Warning开始编写游戏菜单头文件

#ifndef TITLESCREENSTATE_H
#define TITLESCREENSTATE_H

#include "GameState.h"

struct TitleScreenData
{
    int TitleScreenOption; // 0：开始游戏 1：退出游戏
};

//========状态生命周期函数
void TitleScreen_enter(GameContext* ctx, void* data);
void TitleScreen_exit(GameContext* ctx, void* data);
void TitleScreen_update(GameContext* ctx, void* data);
void TitleScreen_render(GameContext* ctx, void* data);

GameState* CreateTitleScreenState();

//=========定义标题图片路径
extern const char* titleScreen_Address;

#endif //TITLESCREENSTATE_H