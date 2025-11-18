#pragma once
#include "raylib.h"
#include "GameState.h"
#include "raygui.h"

struct dialogueData{
    float visibleChars;//可见字符数
    bool skipToEnd;//是否跳过逐字打字效果
};

void createDialogueState(const char* portrait_address, const char* lines);