#pragma once

#include "raylib.h"
#include <string>

struct GameContext; // 新增：使用前向声明避免循环依赖


struct DialogueBoxTemplate_Normal{//创建一般对话框模板
    //对话框样式设置
    Texture2D dialogueBoxTexture;
    float dialogueBoxWidth = 1800.0f;
    float dialogueBoxHeight = 280.0f;
    Vector2 dialogueBoxPosition = {60.0f,760.0f};

    //头像框样式设置
    Texture2D portraitFrameTexture;
    float portraitFrameWidth = 250.0f;
    float portraitFrameHeight = 250.0f;
    Vector2 portraitFramePosition = {100.0f,635.0f};

    //头像样式设置
    float portraitWidth = 210.0f;
    float portraitHeight = 210.0f;
    Vector2 portraitPosition = {120.0f,655.0f};

    //字体样式设置
    float fontSize = 48.0f;
    //textColor 直接用Gamestate.h宏定义的UI_TEXT
    //glowColor 直接用CYBER_CYAN

    Vector2 textOffset = {330.0f, 50.0f};//默认模板的字体偏移量
    Vector2 textOffset_noPortrait = {60.0f,50.0f};//默认模板没有头像的字体偏移量

    Font* font = nullptr; 

    float charsPerSecond = 25.0f;//每秒显示多少个字符

    //姓名样式设置
    Vector2 speakerNameOffset = {100.0f, 595.0f}; 
    float speakerNameFontSize = 32.0f; 

};

struct HUDTemplate {
    // --- 纹理 ---
    Texture2D avatarFrame;  // 头像框图片
    Texture2D hpBarEmpty;   // 血条底图（空的/黑底）
    Texture2D hpBarFill;    // 血条填充图（红条）
    // --- 位置与尺寸 (根据你的美术资源调整) ---
    Vector2 position = { 20.0f, 20.0f }; // 整体起始位置 (左上角)
    
    // 头像偏移
    Vector2 avatarOffset = { 10.0f, 10.0f }; 
    
    // 血条偏移
    Vector2 hpBarOffset = { 120.0f, 20.0f }; 
};


// 声明绘制函数
void DrawHUD(const GameContext& ctx);




/**
 * @brief 将资产填入模板
 * 
 * @param tpl 一般对话框模板
 * @param ctx 核心数据结构体
 */
void FillTemplateWithAssets(DialogueBoxTemplate_Normal& tpl, GameContext& ctx);

/**
 * @brief 根据模板绘制对话框
 * 
 * @param tpl 对话框模板
 * @param text 对话文本
 * @param speakerName 说话者名字
 * @param portrait 头像纹理
 * @param charsToShow 要显示的字符数量
 */
void DrawDialogueWithTemplate(const DialogueBoxTemplate_Normal& tpl, const std::string& text, const std::string& speakerName, Texture2D portrait, int charsToShow); // 修改：显式使用 std::string

/**
 * @brief 加载UI资产
 * 
 * @param ctx 核心数据结构体
 */
void LoadUIAssets(GameContext& ctx);



/**
 * @brief 卸载UI资产
 * 
 * @param ctx 核心数据结构体
 */
void UnloadUIAssets(GameContext& ctx);