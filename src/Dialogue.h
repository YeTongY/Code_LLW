#pragma once
#include "raylib.h"
#include "GameState.h"
#include "raygui.h"

struct DialogueLine{
    string speaker;
    string portraitPath;
    string text;
};


struct DialogueData {
    //存储整个剧本
    vector<DialogueLine> script; 
    
    //存储当前已加载的头像贴图
    Texture2D currentPortrait;

    //追踪播放进度
    int currentLineIndex;           
    float visibleChars; 
    bool skipToEnd;
    float charsPerSecond;
};

void createDialogueState(const char* portrait_address, const char* lines);