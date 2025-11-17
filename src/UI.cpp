#include "UI.h"

void LoadUIAssets(GameContext& ctx){
    const char* dialogue_box_address = "../res/graphics/ui/dialogue_box.png";                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
    const char* portrait_frame_address = "../res/graphics/ui/portrait_frame.png";
    
    //加载UI资产
    ctx.portraitFrameTexture = LoadTexture(portrait_frame_address);
    if(ctx.portraitFrameTexture.id == 0){
        TraceLog(LOG_WARNING,"[UI Loader] 头像框加载失败！");
    }
    else{
        TraceLog(LOG_INFO, "[UI Loader] 头像框加载成功");
    }

    ctx.dialogueBoxTexture = LoadTexture(dialogue_box_address);
    if(ctx.dialogueBoxTexture.id == 0){
        TraceLog(LOG_WARNING,"[UI Loader] 头像框加载失败！");
    }
    else{
        TraceLog(LOG_INFO, "[UI Loader] 头像框加载成功");
    }
}

void UnloadUIAssets(GameContext& ctx){
    UnloadTexture(ctx.dialogueBoxTexture);
    UnloadTexture(ctx.portraitFrameTexture);
}