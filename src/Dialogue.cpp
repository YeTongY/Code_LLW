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

     // 3. 初始化对话进度和打字机状态
    data->currentLineIndex = 0;
    data->visibleChars = 0.0f;
    data->skipToEnd = false;
    //==============修改bgm音量==============
    SetMusicVolume(ctx->explorationBGM, 0.07f);

     // 4. 加载第一句对话的头像
    const DialogueLine& firstLine = data->script[0];
    
    if (firstLine.portraitPath != "nullptr" && !firstLine.portraitPath.empty()) {
        // 尝试多个路径加载头像
        string paths[] = {
            "../" + firstLine.portraitPath,
            firstLine.portraitPath,
            "../../" + firstLine.portraitPath
        };
        
        data->currentPortrait.id = 0;
        for (const auto& path : paths) {
            data->currentPortrait = LoadTexture(path.c_str());
            if (data->currentPortrait.id != 0) {
                TraceLog(LOG_INFO, "[Dialogue] 成功加载头像: %s", path.c_str());
                break;
            }
        }
        
        if (data->currentPortrait.id == 0) {
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

    //==============恢复bgm音量==============
    SetMusicVolume(ctx->explorationBGM, 0.1f);

    TraceLog(LOG_INFO, "[Dialogue] 退出对话状态，所有资源已清理。");
}

void dialogue_update(GameContext* ctx, void* state_data) {
    // 1. 获取数据，安全检查
    DialogueData* data = static_cast<DialogueData*>(state_data);
    if (!data || data->script.empty()) return;

    // 2. 准备当前行的数据
    const DialogueLine& currentLine = data->script[data->currentLineIndex];
    const size_t currentLineLength = currentLine.text.length();

    //============更新音频流============
    UpdateMusicStream(ctx->explorationBGM);

    // 3. 处理用户输入
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_E)) {
        if (static_cast<size_t>(data->visibleChars) < currentLineLength) {
            //“跳过”
            data->skipToEnd = true;
        } else {
            // 意图是“下一句”
            string oldPath = currentLine.portraitPath;
            data->currentLineIndex++;
            
            if (static_cast<size_t>(data->currentLineIndex) >= data->script.size()) {
                // 对话结束，切换回探索状态
                TraceLog(LOG_INFO, "[Dialogue] 对话结束，准备返回探索状态");
                GameState* explorationState = createExplorationState();
                if (explorationState) {
                    GameStateMachine_change(&ctx->state_machine, ctx, explorationState);
                } else {
                    TraceLog(LOG_ERROR, "[Dialogue] 创建探索状态失败，退出游戏");
                    ctx->isRunning = false;
                }
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
                
                // 加载新头像（尝试多个路径）
                if (nextLine.portraitPath != "nullptr" && !nextLine.portraitPath.empty()) {
                    std::string paths[] = {
                        "../" + nextLine.portraitPath,
                        nextLine.portraitPath,
                        "../../" + nextLine.portraitPath
                    };
                    
                    for (const auto& path : paths) {
                        data->currentPortrait = LoadTexture(path.c_str());
                        if (data->currentPortrait.id != 0) {
                            TraceLog(LOG_INFO, "[Dialogue] 切换头像成功: %s", path.c_str());
                            break;
                        }
                    }
                    
                    if (data->currentPortrait.id == 0) {
                        TraceLog(LOG_WARNING, "[Dialogue] 加载新头像失败，路径: %s", nextLine.portraitPath.c_str());
                    }
                }
            }
        }
    }

    // 4. 更新打字机动画
    if (data->skipToEnd) {
        data->visibleChars = static_cast<float>(currentLineLength);
    } else {
        if (static_cast<size_t>(data->visibleChars) < currentLineLength) {
            data->visibleChars += GetFrameTime() * data->charsPerSecond;
            if (static_cast<size_t>(data->visibleChars) > currentLineLength) {
                data->visibleChars = static_cast<float>(currentLineLength);
            }
        }
    }
}


void dialogue_render(GameContext* ctx, void* state_data) {
    static int renderCount = 0;
    renderCount++;
    if (renderCount % 60 == 0) {
        TraceLog(LOG_INFO, "[Dialogue] 渲染帧 %d", renderCount);
    }
    
    // 1. 获取数据包并进行安全检查
    DialogueData* data = static_cast<DialogueData*>(state_data);

    // 2. 绘制游戏世界背景
    exploration_render(ctx, nullptr); // 直接调用，实现叠加效果

    // 如果没有有效的对话数据，就到此为止
    if (data == nullptr || data->script.empty()) {
        return;
    }

    // 3. 准备UI模板
    DialogueBoxTemplate_Normal tpl;
    FillTemplateWithAssets(tpl, *ctx);

    // 4. 从 DialogueData 中准备好要绘制的所有“材料”
    const DialogueLine& currentLine = data->script[data->currentLineIndex];
    const string& textToDisplay = currentLine.text;
    const string& speakerToShow = currentLine.speaker;
    const Texture2D& portraitToShow = data->currentPortrait;
    const int charsToShow = static_cast<int>(data->visibleChars);

    // 5. 调用"万能绘制工具"，完成所有UI的绘制
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


vector<DialogueLine> LoadDialogueScript(const char* filepath) {
    vector<DialogueLine> script;
    
    // 从 build 目录运行，优先尝试带 ../ 的路径
    const char* possiblePaths[] = {
        filepath,
        nullptr,
        nullptr
    };
    
    // 构建备用路径
    string path1 = string("../") + filepath;
    string path2 = string("../../") + filepath;
    possiblePaths[1] = path1.c_str();
    possiblePaths[2] = path2.c_str();
    
    ifstream file;
    const char* actualPath = nullptr;
    
    for (int i = 0; i < 3; i++) {
        file.open(possiblePaths[i]);
        if (file.is_open()) {
            actualPath = possiblePaths[i];
            TraceLog(LOG_INFO, "[DialogueLoader] 成功打开剧本文件: %s", actualPath);
            break;
        }
        file.clear();
    }

    if (!file.is_open()) {
        TraceLog(LOG_ERROR, "[DialogueLoader] 无法打开剧本文件，尝试了以下路径:");
        TraceLog(LOG_ERROR, "  - %s", filepath);
        TraceLog(LOG_ERROR, "  - %s", path1.c_str());
        TraceLog(LOG_ERROR, "  - %s", path2.c_str());
        TraceLog(LOG_ERROR, "[DialogueLoader] 当前工作目录: %s", GetWorkingDirectory());
        return script;
    }

    string line;
    while (getline(file, line)) {
        // 跳过空行或注释行 
        if (line.empty() || line[0] == '#') {
            continue;
        }

        stringstream ss(line);
        string speaker;
        string path;
        string text;

        getline(ss, speaker, ',');
        getline(ss, path, ',');
        getline(ss, text);

        if (!text.empty() && text.front() == '"') {
            text.erase(0, 1);
        }
        if (!text.empty() && text.back() == '"') {
            text.pop_back();
        }

        script.push_back({speaker, path, text}); // 使用C++11的列表初始化，更简洁
    }

    file.close();
    TraceLog(LOG_INFO, "[DialogueLoader] 成功加载剧本 '%s'，共 %zu 行对话。", filepath, script.size());
    return script;
}