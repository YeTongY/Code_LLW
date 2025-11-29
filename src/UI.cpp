#include "UI.h"
#include "GameState.h"
#include "FontLoader.h"

void LoadUIAssets(GameContext& ctx){
    // --- 1. 定义资源路径 ---
    const char* dialogue_box_path = "../res/graphics/ui/dialogue_box.png";
    const char* portrait_frame_path = "../res/graphics/ui/portrait_frame.png";
    const char* player_portrait_path = "../res/graphics/portraits/Taffy.png";
    const char* hp_bar_bg_path = "../res/graphics/ui/ProgressBar Background_Black.png";
    const char* hp_bar_fill_path = "../res/graphics/ui/ProgressBarForeground_Black.png";

    TraceLog(LOG_INFO, "================ [UI] 开始加载资源 ================");

    // --- 2. 加载基础 UI 纹理 ---
    
    // 对话框背景
    ctx.dialogueBoxTexture = LoadTexture(dialogue_box_path);
    if(ctx.dialogueBoxTexture.id == 0) TraceLog(LOG_WARNING, "[UI] 加载失败: %s", dialogue_box_path);
    else TraceLog(LOG_INFO, "[UI] 加载成功: 对话框背景");

    // 头像框 (通用)
    ctx.portraitFrameTexture = LoadTexture(portrait_frame_path);
    if(ctx.portraitFrameTexture.id == 0) TraceLog(LOG_WARNING, "[UI] 加载失败: %s", portrait_frame_path);
    else TraceLog(LOG_INFO, "[UI] 加载成功: 头像框");

    // 玩家头像
    ctx.playerPortrait = LoadTexture(player_portrait_path);
    if(ctx.playerPortrait.id == 0) TraceLog(LOG_WARNING, "[UI] 加载失败: %s", player_portrait_path);
    else TraceLog(LOG_INFO, "[UI] 加载成功: 玩家头像");

    // --- 3. 配置 HUD 模板 (复用 + 新加载) ---
    
    // 复用已加载的头像框
    ctx.hudTemplate.avatarFrame = ctx.portraitFrameTexture; 
    
    // 加载血条资源直接到模板中
    ctx.hudTemplate.hpBarEmpty = LoadTexture(hp_bar_bg_path);
    if(ctx.hudTemplate.hpBarEmpty.id == 0) TraceLog(LOG_WARNING, "[UI] 加载失败: %s", hp_bar_bg_path);
    else TraceLog(LOG_INFO, "[UI] 加载成功: 血条背景");

    ctx.hudTemplate.hpBarFill = LoadTexture(hp_bar_fill_path);
    if(ctx.hudTemplate.hpBarFill.id == 0) TraceLog(LOG_WARNING, "[UI] 加载失败: %s", hp_bar_fill_path);
    else TraceLog(LOG_INFO, "[UI] 加载成功: 血条填充");

    // --- 4. 其他兼容性设置 ---
    ctx.playerPortraitFrame = ctx.portraitFrameTexture; // 保持兼容

    TraceLog(LOG_INFO, "================ [UI] 资源加载结束 ================");
}

void UnloadUIAssets(GameContext& ctx){
    TraceLog(LOG_INFO, "================ [UI] 开始卸载资源 ================");

    // 卸载基础纹理
    if(ctx.dialogueBoxTexture.id != 0) UnloadTexture(ctx.dialogueBoxTexture);
    if(ctx.portraitFrameTexture.id != 0) UnloadTexture(ctx.portraitFrameTexture);
    if(ctx.playerPortrait.id != 0) UnloadTexture(ctx.playerPortrait);

    // 卸载 HUD 独有纹理
    // 注意：avatarFrame 是复用的 portraitFrameTexture，不要重复卸载！
    if(ctx.hudTemplate.hpBarEmpty.id != 0) UnloadTexture(ctx.hudTemplate.hpBarEmpty);
    if(ctx.hudTemplate.hpBarFill.id != 0) UnloadTexture(ctx.hudTemplate.hpBarFill);

    TraceLog(LOG_INFO, "================ [UI] 资源卸载结束 ================");
}

void FillTemplateWithAssets(DialogueBoxTemplate_Normal& tpl, GameContext& ctx){
    tpl.font = &ctx.mainFont;//填充主字体
    tpl.dialogueBoxTexture = ctx.dialogueBoxTexture;//填充对话框图片
    tpl.portraitFrameTexture = ctx.portraitFrameTexture;//填充头像框图片
}

// ====== HUD 绘制函数 ======
void DrawHUD(const GameContext& ctx) {
    const HUDTemplate& tpl = ctx.hudTemplate;
    
    // 1. 获取玩家当前 HP 比例
    float hp = (float)ctx.player.stats.hp;
    float maxHp = (float)ctx.player.stats.maxHp;
    float hpRatio = (maxHp > 0) ? (hp / maxHp) : 0.0f;
    
    // 安全限制比例在 [0, 1] 范围内
    if (hpRatio < 0.0f) hpRatio = 0.0f;
    if (hpRatio > 1.0f) hpRatio = 1.0f;

    // 2. 绘制头像（底层）
    if (ctx.playerPortrait.id != 0) {
        Rectangle portraitSource = {0, 0, (float)ctx.playerPortrait.width, (float)ctx.playerPortrait.height};
        Rectangle portraitDest = {
            tpl.position.x + tpl.avatarOffset.x,
            tpl.position.y + tpl.avatarOffset.y,
            80.0f, // 头像尺寸
            80.0f
        };
        DrawTexturePro(ctx.playerPortrait, portraitSource, portraitDest, {0, 0}, 0.0f, WHITE);
    }

    // 3. 绘制头像框（覆盖在头像上）
    if (tpl.avatarFrame.id != 0) {
        Rectangle frameSource = {0, 0, (float)tpl.avatarFrame.width, (float)tpl.avatarFrame.height};
        Rectangle frameDest = {
            tpl.position.x,
            tpl.position.y,
            100.0f, // 框的尺寸
            100.0f
        };
        DrawTexturePro(tpl.avatarFrame, frameSource, frameDest, {0, 0}, 0.0f, WHITE);
    }

    // 4. 绘制血条背景（完整长度）
    float barWidth = 200.0f;
    float barHeight = 20.0f;
    Vector2 barPos = {
        tpl.position.x + tpl.hpBarOffset.x,
        tpl.position.y + tpl.hpBarOffset.y
    };

    if (tpl.hpBarEmpty.id != 0) {
        Rectangle bgSource = {0, 0, (float)tpl.hpBarEmpty.width, (float)tpl.hpBarEmpty.height};
        Rectangle bgDest = {barPos.x, barPos.y, barWidth, barHeight};
        DrawTexturePro(tpl.hpBarEmpty, bgSource, bgDest, {0, 0}, 0.0f, WHITE);
    }

    // 5. 绘制血条填充（按比例裁剪 + 染红色）
    if (tpl.hpBarFill.id != 0 && hpRatio > 0) {
        // 源矩形：裁剪图片的左侧部分（宽度 × 比例）
        Rectangle fillSource = {
            0, 0,
            (float)tpl.hpBarFill.width * hpRatio,
            (float)tpl.hpBarFill.height
        };
        
        // 目标矩形：屏幕上显示的宽度也要对应缩放
        Rectangle fillDest = {
            barPos.x,
            barPos.y,
            barWidth * hpRatio,
            barHeight
        };
        
        DrawTexturePro(tpl.hpBarFill, fillSource, fillDest, {0, 0}, 0.0f, RED);
    }

    // 6. 绘制 HP 文字数值
    char hpText[32];
    std::sprintf(hpText, "%d/%d", ctx.player.stats.hp, ctx.player.stats.maxHp);
    DrawTextEx(ctx.mainFont, hpText, 
               {barPos.x + 5, barPos.y - 25}, 
               20, 1, WHITE);
    
    DrawTextEx(ctx.mainFont, "HP", 
               {barPos.x - 35, barPos.y}, 
               20, 1, RED);
}

void DrawDialogueWithTemplate(const DialogueBoxTemplate_Normal& tpl, const std::string& text,const std::string& speakerName,Texture2D portrait,int charsToShow){ // 修改：参数改用 std::string
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
    std::string textToDraw; // 修改：显式使用 std::string
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