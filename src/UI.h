#pragma once

#include "raylib.h"
#include "GameState.h"
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
 * @param portrait_address 头像地址 
 */
void DrawDialogueWithTemplate(const DialogueBoxTemplate_Normal& tpl, const string& text, const char* portrait_address);

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