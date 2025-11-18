#include "Dialogue.h"
#include "ExplorationState.h"
#include "UI.h"


    void dialogue_enter(GameContext* ctx, void* state_data) {
    // 1. 类型转换，获取我们的数据包
    DialogueData* data = static_cast<DialogueData*>(state_data);
    // 2. 安全检查
    if (data->script.empty()) {
        TraceLog(LOG_WARNING, "[Dialogue] 尝试开始一场没有内容的对话！");
        return; // 直接返回，不执行任何操作
    }
    //? 这上面不懂

     // 3. 初始化对话进度和打字机状态
    data->currentLineIndex = 0;
    data->visibleChars = 0.0f;
    data->skipToEnd = false;

     // 4. 加载第一句对话的头像
    const DialogueLine& firstLine = data->script[0];
    
    if (firstLine.portraitPath != "nullptr" && !firstLine.portraitPath.empty()) {
        // 如果路径不是"nullptr"且不为空
        data->currentPortrait = LoadTexture(firstLine.portraitPath.c_str());
        if (data->currentPortrait.id == 0) {
            // 如果加载失败，打印一条警告
            TraceLog(LOG_WARNING, "[Dialogue] 加载第一个头像失败，路径: %s", firstLine.portraitPath.c_str());
        }
    } else {
        // 如果路径是"nullptr"或空的，确保当前没有加载任何头像
        data->currentPortrait.id = 0;
    }

    TraceLog(LOG_INFO, "[Dialogue] 进入对话状态，准备播放第一句。");
    }

    void dialogue_exit(GameContext* ctx, void* state_data) {
    // 1. 获取数据包并进行安全检查
    DialogueData* data = static_cast<DialogueData*>(state_data);
    if (data == nullptr) {
        TraceLog(LOG_WARNING, "[Dialogue] 尝试退出一个空的对话状态。");
        return;
    }

    // 2. 清理本次对话中加载的头像贴图
    if (data->currentPortrait.id != 0) {
        UnloadTexture(data->currentPortrait);
        TraceLog(LOG_INFO, "[Dialogue] 已卸载当前对话头像。");
    }

    // 3. 释放为本次对话分配的数据包内存
    delete data;

    TraceLog(LOG_INFO, "[Dialogue] 退出对话状态，所有资源已清理。");
}

void dialogue_update(GameContext* ctx, void* state_data) {
    // 1. 获取数据，安全检查
    DialogueData* data = static_cast<DialogueData*>(state_data);
    if (!data || data->script.empty()) return;

    // 2. 准备当前行的数据
    const DialogueLine& currentLine = data->script[data->currentLineIndex];
    const int currentLineLength = currentLine.text.length();

    // 3. 处理用户输入
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if ((int)data->visibleChars < currentLineLength) {
            // 意图是“跳过”
            data->skipToEnd = true;
        } else {
            // 意图是“下一句”
            std::string oldPath = currentLine.portraitPath;
            data->currentLineIndex++;
            
            if (data->currentLineIndex >= data->script.size()) {
                // 对话结束，标记状态机需要退出对话
                ctx->isRunning = false; // TODO: 改为切换到探索状态
                return;
            }
            
            // 重置打字机
            data->visibleChars = 0;
            data->skipToEnd = false;
            
            // 处理头像切换
            const DialogueLine& nextLine = data->script[data->currentLineIndex];
            if (nextLine.portraitPath != oldPath) {
                // 卸载旧头像
                if (data->currentPortrait.id != 0) {
                    UnloadTexture(data->currentPortrait);
                    data->currentPortrait.id = 0;
                }
                
                // 加载新头像
                if (nextLine.portraitPath != "nullptr" && !nextLine.portraitPath.empty()) {
                    data->currentPortrait = LoadTexture(nextLine.portraitPath.c_str());
                    if (data->currentPortrait.id == 0) {
                        TraceLog(LOG_WARNING, "[Dialogue] 加载新头像失败，路径: %s", nextLine.portraitPath.c_str());
                    }
                }
            }
        }
    }

    // 4. 更新打字机动画
    if (data->skipToEnd) {
        data->visibleChars = currentLineLength;
    } else {
        if (data->visibleChars < currentLineLength) {
            data->visibleChars += GetFrameTime() * data->charsPerSecond;
            if (data->visibleChars > currentLineLength) {
                data->visibleChars = currentLineLength;
            }
        }
    }
}

// 在 Dialogue.cpp 中

void dialogue_render(GameContext* ctx, void* state_data) {
    // 1. 获取数据包并进行安全检查
    DialogueData* data = static_cast<DialogueData*>(state_data);

    // 2. 绘制游戏世界背景
    exploration_render(ctx, nullptr); // 直接调用，实现叠加效果

    // 如果没有有效的对话数据，就到此为止
    if (data == nullptr || data->script.empty()) {
        return;
    }

    // 3. 准备UI模板 (你的工具)
    DialogueBoxTemplate_Normal tpl;
    FillTemplateWithAssets(tpl, *ctx);

    // 4. 从 DialogueData 中准备好要绘制的所有“材料”
    const DialogueLine& currentLine = data->script[data->currentLineIndex];
    const std::string& textToDisplay = currentLine.text;
    const std::string& speakerToShow = currentLine.speaker;
    const Texture2D& portraitToShow = data->currentPortrait;
    const int charsToShow = (int)data->visibleChars;

    // 5. 调用你的"万能绘制工具"，完成所有UI的绘制
    DrawDialogueWithTemplate(
        tpl, 
        textToDisplay, 
        speakerToShow, 
        portraitToShow, 
        charsToShow
    );
}












GameState* createDialogueState(const vector<DialogueLine>& script){
    DialogueData* data = new DialogueData();

    data->script = script; // 把剧本复制进去

    // 初始化所有状态
    data->currentLineIndex = 0;
    data->visibleChars = 0.0f;
    data->skipToEnd = false;
    data->currentPortrait.id = 0; // 一开始没有任何头像被加载
    data->charsPerSecond = 25.0f; // 默认速度

    GameState* state = Gamestate_create(
    dialogue_enter,      // “进入”
    dialogue_exit,       // “退出”
    dialogue_update,     // “更新”
    dialogue_render,     // “渲染”
    data,                
    sizeof(DialogueData) 
    );

    //安全检查
    if (!state) {
        TraceLog(LOG_ERROR, "[Dialogue] 创建 GameState 失败");
        delete data; // 清理之前分配的内存
        return nullptr;
    }
    
    TraceLog(LOG_INFO, "[Dialogue] 对话状态创建成功");
    return state;

}


