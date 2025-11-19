#pragma once
#include "raylib.h"
#include "raygui.h"
#include <fstream>   // 文件流 (ifstream)，用来读取文件
#include <sstream>   // 字符串流 (stringstream)，用来在内存中处理字符串
#include <vector>
#include <string>
using namespace std;

// 前向声明
struct GameState;

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

GameState* createDialogueState(const vector<DialogueLine>& script);


vector<DialogueLine> LoadDialogueScript(const char* filepath);