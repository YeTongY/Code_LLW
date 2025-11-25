//2025/11/18 Warning开始编写标题实现逻辑

#include "GameState.h"
#include "raylib.h"
#include "ExplorationState.h"
#include "TitleScreenState.h"
#include <string>

void TitleScreen_enter(GameContext* ctx, void* data)
{
    TitleScreenData* state_data = static_cast<TitleScreenData*>(data);
    state_data->TitleScreenOption = 0;

    //===============加载标题背景图
    ctx->titleScreen = LoadTexture(titleScreen_Address);
    if(ctx->titleScreen.id == 0){
        TraceLog(LOG_ERROR,"[Title] 加载标题背景图出错！");
    }
    else{
        TraceLog(LOG_INFO,"[Title] 加载标题背景图成功");
    }
    //===============加载结束

    TraceLog(LOG_INFO,"[Title] 进入标题状态"); 
}

void TitleScreen_exit(GameContext* ctx, void* data)
{
    TraceLog(LOG_INFO,"[Title] 退出标题状态");
    //==========后续标题界面的资源在此处卸载
    UnloadTexture(ctx->titleScreen);
}

void TitleScreen_render(GameContext* ctx, void* data)
{
    TitleScreenData* state_data = static_cast<TitleScreenData*>(data);

    ClearBackground(BLACK);//=======黑色背景 

    //=============绘制标题背景图
    Rectangle titlescreen_source ={0,0,2731,1536};
    Rectangle titlescreen_dest = {0,0,1920,1080};

    DrawTexturePro(ctx->titleScreen,titlescreen_source,titlescreen_dest,{0,0},0.0f,WHITE);
    

    //=============绘制游戏标题

    const char* Title = "代号:LLW";

    Vector2 titlesize = MeasureTextEx(ctx->mainFont, Title, 80, 2);//测量标题大小
    DrawTextEx(ctx->mainFont, Title, {(ctx->screenWidth - titlesize.x) / 2.0f, ctx->screenHeight * 0.25f}, 80, 2, RAYWHITE);


    //=============绘制菜单选项
    const char* option1 = "开始游戏";
    const char* option2 = "退出游戏";

    //=============根据是否选中改变标题颜色
    Color color1 = (state_data->TitleScreenOption == 0) ? YELLOW : WHITE;
    Color color2 = (state_data->TitleScreenOption == 1) ? YELLOW : WHITE;

    Vector2 option1Size = MeasureTextEx(ctx->mainFont, option1, 40, 1);
    Vector2 option2Size = MeasureTextEx(ctx->mainFont, option2, 40, 1);

    DrawTextEx(ctx->mainFont, option1, {(ctx->screenWidth - titlesize.x) / 2.0f, ctx->screenHeight * 0.55f}, 40, 1, color1);
    DrawTextEx(ctx->mainFont, option2, {(ctx->screenWidth - titlesize.x) / 2.0f, ctx->screenHeight * 0.65f}, 40, 1, color2);

    //=============绘制提示词
    const char* hint = "按下 [Enter] 确认";

    Vector2 hintSize = MeasureTextEx(ctx->mainFont, hint, 20, 1);
    DrawTextEx(ctx->mainFont, hint, { (ctx->screenWidth - hintSize.x) / 2.0f, ctx->screenHeight - 50.0f }, 20, 1, GRAY);

}

void TitleScreen_update(GameContext* ctx, void* data)
{
    TitleScreenData* state_data = static_cast<TitleScreenData*>(data);
    
    //==========检测按键
    if (IsKeyPressed(KEY_DOWN))//按“下方向”选择
    {
        state_data->TitleScreenOption++;
        if (state_data->TitleScreenOption < 0)
        {
            state_data->TitleScreenOption = 1;//循环检测
        }

    }
    else if (IsKeyPressed(KEY_UP))
    {
        state_data->TitleScreenOption--;
        if (state_data->TitleScreenOption < 0)
        {
            state_data->TitleScreenOption = 1;//循环检测
        }
    }
    //==========处理确认操作
    if (IsKeyPressed(KEY_ENTER))
    {
        switch (state_data->TitleScreenOption)
        {
            case 0://选择开始游戏
            {
                TraceLog(LOG_INFO, "[TitleScreen] 选择 ‘开始游戏’ 切换到探索模式");
                //创建并切换到探索状态
                GameState* ExplorationState = createExplorationState();
                GameStateMachine_change(&ctx->state_machine, ctx, ExplorationState);
                break;
            }
            case 1://选择退出游戏
            {
                TraceLog(LOG_INFO, "[TitleScreen] 选择 ‘退出游戏’");
                ctx->isRunning = false;
                break;
            }
        }
    }
}

//=================状态创建
GameState* CreateTitleScreenState()
{

    TitleScreenData* data = new TitleScreenData();
    if (!data)
    {
        TraceLog(LOG_INFO, "[TitleScreen] 内存分配失败");
        return nullptr;
    }

    GameState* state = Gamestate_create(
        TitleScreen_enter,
        TitleScreen_exit,
        TitleScreen_update,
        TitleScreen_render,
        data,
        sizeof(TitleScreenData)
    );

    if (!state) {
        TraceLog(LOG_ERROR, "[TitleScreen] 创建状态失败");
        delete data; // 清理内存
        return nullptr;
    }

    TraceLog(LOG_INFO, "[TitleScreen] 标题屏幕状态创建成功");
    return state;
}