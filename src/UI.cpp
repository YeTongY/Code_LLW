#include "UI.h"
#include "GameState.h"

struct DialogueBoxTemplate_Normal{//创建一般对话框模板
    //对话框样式设置
    Texture2D dialogueBoxTexture;
    float dialogueBoxWidth = 1800.0f;
    float dialogueBoxHeight = 280.0f;
    Vector2 dialogueBoxPosition = {60.0f,760.0f};

    //头像框样式设置
    Texture2D potraitFrameTexture;
    float potraitFrameWidth = 250.0f;
    float potraitFrameHeight = 250.0f;
    Vector2 potraitFramePosition = {100.0f,635.0f};

    //头像样式设置
    float potraitWidth = 210.0f;
    float potraitHeight = 210.0f;
    Vector2 potraitPosition = {120.0f,655.0f};

    //字体样式设置
    float fontSize = 48.0f;
    //textColor 直接用Gamestate.h宏定义的UI_TEXT
    //glowColor 直接用CYBER_CYAN
    Vector2 textOffset = {330.0f, 50.0f};
    Font* font = nullptr; // 使用指针，可以指向 GameContext.mainFont 等



};


void LoadUIAssets(GameContext& ctx){
    const char* dialogue_box_address = "../res/graphics/ui/dialogue_box.png";                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
    const char* portrait_frame_address = "../res/graphics/ui/portrait_frame.png";
    
    //加载UI资产
    ctx.portraitFrameTexture = LoadTexture(portrait_frame_address);
    if(ctx.portraitFrameTexture.id == 0){
        TraceLog(LOG_WARNING,"[UI Loader] 头像框UI加载失败！");
    }
    else{
        TraceLog(LOG_INFO, "[UI Loader] 头像框UI加载成功");
    }

    ctx.dialogueBoxTexture = LoadTexture(dialogue_box_address);
    if(ctx.dialogueBoxTexture.id == 0){
        TraceLog(LOG_WARNING,"[UI Loader] 头像框UI加载失败！");
    }
    else{
        TraceLog(LOG_INFO, "[UI Loader] 头像框UI加载成功");
    }
}

void UnloadUIAssets(GameContext& ctx){
    
    if(ctx.dialogueBoxTexture.id != 0){
        UnloadTexture(ctx.dialogueBoxTexture);
        TraceLog(LOG_INFO,"[UI Loader] 卸载对话框UI成功");
    }
    else{
        TraceLog(LOG_ERROR,"[UI Loader] 尝试卸载一个空的UI纹理!");
    }
    if(ctx.portraitFrameTexture.id != 0){
        UnloadTexture(ctx.portraitFrameTexture);
        TraceLog(LOG_INFO,"[UI Loader] 卸载头像框UI成功");
    }
    else{
        TraceLog(LOG_ERROR,"[UI Loader] 尝试卸载一个空的UI纹理!");
    }
}