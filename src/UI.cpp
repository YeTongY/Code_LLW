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



void DrawDialogueWithTemplate(const DialogueBoxTemplate_Normal& tpl, const string& text,const string& speakerName,Texture2D portrait,int charsToShow){
    //准备一些矩形
    Rectangle portrait_source = {0.0f, 0.0f, (float)portrait.width, (float)portrait.height};//不裁剪
    Rectangle portrait_dest = {tpl.portraitPosition.x, tpl.portraitPosition.y, tpl.portraitWidth, tpl.portraitHeight};


    //准备文本的位置
    Vector2 textPosition;

    //绘制对话框背景（使用 DrawTexturePro 按照模板尺寸缩放）
    Rectangle dialogueBoxSource = {0.0f, 0.0f, (float)tpl.dialogueBoxTexture.width, (float)tpl.dialogueBoxTexture.height};
    Rectangle dialogueBoxDest = {
        tpl.dialogueBoxPosition.x,
        tpl.dialogueBoxPosition.y,
        tpl.dialogueBoxWidth,
        tpl.dialogueBoxHeight
    };
    DrawTexturePro(tpl.dialogueBoxTexture, dialogueBoxSource, dialogueBoxDest, {0, 0}, 0.0f, WHITE);

    if (!speakerName.empty()) {//绘制名字
        // 计算名字的绝对屏幕位置
        Vector2 namePosition = { 
            tpl.speakerNameOffset.x, // 直接使用模板里定义的绝对坐标
            tpl.speakerNameOffset.y 
        };

        // 用不同的样式来绘制名字，以作区分
        DrawTextGlow(
            *tpl.font,
            speakerName.c_str(),
            namePosition,
            tpl.speakerNameFontSize, // 使用为名字专门定义的字号
            1,
            WHITE,
            CYBER_MAGENTA // 使用不同的辉光色，比如品红
        );
    }

    if(portrait.id != 0){//绘制头像
        
        //绘制头像框（使用 DrawTexturePro 按照模板尺寸缩放）
        Rectangle portraitFrameSource = {0.0f, 0.0f, (float)tpl.portraitFrameTexture.width, (float)tpl.portraitFrameTexture.height};
        Rectangle portraitFrameDest = {
            tpl.portraitFramePosition.x,
            tpl.portraitFramePosition.y,
            tpl.portraitFrameWidth,
            tpl.portraitFrameHeight
        };
        DrawTexturePro(tpl.portraitFrameTexture, portraitFrameSource, portraitFrameDest, {0, 0}, 0.0f, WHITE);

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
    // 安全的 UTF-8 字符截断（避免截断多字节字符）
    string textToDraw;
    int charCount = 0;
    
    for (size_t i = 0; i < text.length() && charCount < charsToShow; ) {
        unsigned char c = text[i];
        int charBytes = 1;
        
        // 判断 UTF-8 字符的字节数
        if ((c & 0x80) == 0) {
            // ASCII 字符 (0xxxxxxx)
            charBytes = 1;
        } else if ((c & 0xE0) == 0xC0) {
            // 2字节字符 (110xxxxx)
            charBytes = 2;
        } else if ((c & 0xF0) == 0xE0) {
            // 3字节字符 (1110xxxx) - 大部分中文
            charBytes = 3;
        } else if ((c & 0xF8) == 0xF0) {
            // 4字节字符 (11110xxx)
            charBytes = 4;
        }
        
        // 确保不会超出字符串范围
        if (i + charBytes > text.length()) break;
        
        // 添加完整字符
        textToDraw.append(text.substr(i, charBytes));
        i += charBytes;
        charCount++;
    }

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