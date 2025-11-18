#include "UI.h"
#include "GameState.h"
#include "FontLoader.h"



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



void FillTemplateWithAssets(DialogueBoxTemplate_Normal& tpl, GameContext& ctx){
    tpl.font = &ctx.mainFont;//填充主字体
    tpl.dialogueBoxTexture = ctx.dialogueBoxTexture;//填充对话框图片
    tpl.portraitFrameTexture = ctx.portraitFrameTexture;//填充头像框图片
}



void DrawDialogueWithTemplate(const DialogueBoxTemplate_Normal& tpl, const string& text,Texture2D portrait,int charsToShow){
    Rectangle portrait_source = {0.0f, 0.0f, (float)portrait.width, (float)portrait.height};//不裁剪
    Rectangle portrait_dest = {tpl.portraitPosition.x, tpl.portraitPosition.y, tpl.portraitWidth, tpl.portraitHeight};


    //准备文本的位置
    Vector2 textPosition;

    //绘制对话框背景
    DrawTexture(tpl.dialogueBoxTexture,tpl.dialogueBoxPosition.x,tpl.dialogueBoxPosition.y,WHITE);

    if(portrait.id != 0){
        
        //绘制头像框
        DrawTexture(tpl.portraitFrameTexture,tpl.portraitFramePosition.x,tpl.portraitFramePosition.y,WHITE);

        //绘制头像
        DrawTexturePro(portrait,portrait_source,portrait_dest,
        { 0, 0 },//不需要旋转变化
        0.0f,
        WHITE//不需要色调
        );

        //设置文本位置（靠右）
        textPosition = { tpl.dialogueBoxPosition.x + tpl.textOffset.x, tpl.dialogueBoxPosition.y + tpl.textOffset.y };

    }
     else {// --- 没有头像的情况 ---
        
        
        // 设置文本位置（靠左）
        textPosition = { tpl.dialogueBoxPosition.x + tpl.textOffset_noPortrait.x, tpl.dialogueBoxPosition.y + tpl.textOffset_noPortrait.y };
    }

    //准备要绘制的文本,以实现流式文字绘制
    string textToDraw = text.substr(0, charsToShow);

    //绘制辉光文本
    DrawTextGlow(
        *tpl.font,
        textToDraw.c_str(),
        textPosition,
        tpl.fontSize,
        2,          // 间距
        UI_TEXT,    // 使用宏定义
        CYBER_CYAN  // 使用宏定义
    );

}