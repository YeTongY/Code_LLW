#include "Combat.h"
#include "ExplorationState.h"
#include "raylib.h"
#include "GameState.h"
#include "Audio.h"
#include "Dialogue.h"
#include <cstdio>
#include <algorithm>

static GameState* CreateDialogueStateForScript(const std::string& scriptPath, GameState* nextState, bool destroyNextOnFailure)
{
    if (scriptPath.empty() || !nextState) {
        return nullptr;
    }

    std::vector<DialogueLine> script = LoadDialogueScript(scriptPath.c_str());
    if (script.empty()) {
        if (destroyNextOnFailure) {
            GameState_destory(nextState);
        }
        return nullptr;
    }

    GameState* dialogueState = createDialogueState(script, nextState);
    if (!dialogueState && destroyNextOnFailure) {
        GameState_destory(nextState);
    }
    return dialogueState;
}

//=========================战斗状态实现=========================

void combat_enter(GameContext* ctx, void* state_data)
{
    CombatData* data = static_cast<CombatData*>(state_data); // 当前战斗状态数据
    
    data->isActive = true;
    data->currentPhase = COMBAT_PHASE_START;
    data->selectedAction = 0;
    data->turnCount = 0;
    data->messageTimer = 0.0f;
    data->animationTimer = 0.0f;
    data->playerAnimating = false;
    data->enemyAnimating = false;
    data->damageDealt = 0;
    data->playerDefending = false;
    data->skillMenuActive = false;
    data->selectedSkillIndex = 0;
    data->skillNames = {"代码风暴", "纳米注入"};
    
    // 初始化行动选项
    data->actionNames.clear();
    data->actionNames.push_back("攻击");   // 行动列表[0]
    data->actionNames.push_back("技能");   // 行动列表[1]
    data->actionNames.push_back("防御");   // 行动列表[2]
    data->actionNames.push_back("逃跑");   // 行动列表[3]
    
    // 设置初始消息
    if (data->currentEnemy) {
        data->battleMessage = "敌人出现了！";
        data->messageTimer = 2.0f; // 显示2秒
    }
    
    ctx->enableFootstepAudio = false;
    ctx->player.isMoving = false;
    StopFootstepSound(*ctx);
    StopExplorationBGM(*ctx);
    PlayCombatBGM(*ctx);

    TraceLog(LOG_INFO, "[Combat] 进入战斗状态");
}

void combat_exit(GameContext* ctx, void* state_data)
{
    CombatData* data = static_cast<CombatData*>(state_data); // 待释放的战斗数据
    
    if (data) {
        data->isActive = false;
        ctx->currentCombatant = nullptr;
        delete data;
    }

    ctx->enableFootstepAudio = false;
    ctx->player.isMoving = false;
    StopFootstepSound(*ctx);
    StopCombatBGM(*ctx);
    
    TraceLog(LOG_INFO, "[Combat] 退出战斗状态");
}

void combat_update(GameContext* ctx, void* state_data)
{
    CombatData* data = static_cast<CombatData*>(state_data); // 当前帧使用的战斗数据
    
    if (!data->isActive) return;
    
    if (ctx->isCombatBGMPlaying) {
        UpdateMusicStream(ctx->combatBGM); // 持续刷新战斗BGM，防止播放中断
    }

    float deltaTime = GetFrameTime(); // 本帧耗时（秒）
    
    // 更新计时器
    if (data->messageTimer > 0.0f) {
        data->messageTimer -= deltaTime;
    }
    
    if (data->animationTimer > 0.0f) {
        data->animationTimer -= deltaTime;
    }
    
    switch (data->currentPhase)
    {
        case COMBAT_PHASE_START:
            // 战斗开始，等待消息显示完毕
            if (data->messageTimer <= 0.0f) {
                data->currentPhase = COMBAT_PHASE_PLAYER_TURN;
                data->battleMessage = "选择你的行动";
            }
            break;
            
        case COMBAT_PHASE_PLAYER_TURN:
            // 玩家回合 - 处理输入
            if (data->skillMenuActive) {
                if (IsKeyPressed(KEY_DOWN)) {
                    data->selectedSkillIndex = (data->selectedSkillIndex + 1) % static_cast<int>(data->skillNames.size());
                }
                else if (IsKeyPressed(KEY_UP)) {
                    int total = static_cast<int>(data->skillNames.size());
                    data->selectedSkillIndex = (data->selectedSkillIndex - 1 + total) % total;
                }
                else if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE)) {
                    data->skillMenuActive = false;
                    data->battleMessage = "选择你的行动";
                }
                else if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    UseSkill(ctx, data, data->selectedSkillIndex);
                }
            } else {
                if (IsKeyPressed(KEY_DOWN)) {
                    data->selectedAction = (data->selectedAction + 1) % 4;
                }
                else if (IsKeyPressed(KEY_UP)) {
                    data->selectedAction = (data->selectedAction - 1 + 4) % 4;
                }
                else if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    ProcessPlayerAction(ctx, data, static_cast<CombatAction>(data->selectedAction));
                }
            }
            break;
            
        case COMBAT_PHASE_ENEMY_TURN:
            // 敌人回合
            if (data->messageTimer <= 0.0f && data->animationTimer <= 0.0f) {
                ProcessEnemyTurn(ctx, data);
            }
            break;
            
        case COMBAT_PHASE_ANIMATION:
            // 动画播放中，等待完成
            if (data->animationTimer <= 0.0f) {
                CheckBattleEnd(ctx, data);
            }
            break;
            
        case COMBAT_PHASE_VICTORY:
            // 胜利结算
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                // 返回探索状态
                GameState* explorationState = createExplorationState();
                if (explorationState) {
                    GameStateMachine_change(&ctx->state_machine, ctx, explorationState);
                }
            }
            break;
            
        case COMBAT_PHASE_DEFEAT:
            // 失败结算
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                // 可以选择重新开始或回到标题
                ctx->isRunning = false; // 暂时直接退出
            }
            break;
    }
}

void combat_render(GameContext* ctx, void* state_data)
{
    CombatData* data = static_cast<CombatData*>(state_data); // 当前绘制所需的战斗数据
    
    if (!data->isActive) return;
    
    ClearBackground(BLACK);
    
    // 绘制战斗背景
    if (ctx->combatBackground.id != 0) {
        Rectangle src = {0.0f, 0.0f, (float)ctx->combatBackground.width, (float)ctx->combatBackground.height};
        Rectangle dest = {0.0f, 0.0f, ctx->screenWidth, ctx->screenHeight};
        DrawTexturePro(ctx->combatBackground, src, dest, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
    } else {
        DrawRectangle(0, 0, ctx->screenWidth, ctx->screenHeight, (Color){10, 10, 20, 255});
    }
    
    // 绘制标题
    DrawTextEx(ctx->mainFont, "战斗模式", {20, 20}, 32, 1, CYBER_CYAN);

    // 操作指南（右上角，与探索界面风格一致）
    const float panelWidth = 360.0f;
    const float panelHeight = 150.0f;
    Vector2 panelPos = {ctx->screenWidth - panelWidth - 40.0f, 20.0f};
    DrawRectangleV(panelPos, {panelWidth, panelHeight}, Fade(CYBER_CYAN, 0.25f));
    DrawRectangleLines((int)panelPos.x, (int)panelPos.y, (int)panelWidth, (int)panelHeight, CYBER_CYAN);
    Vector2 textPos = {panelPos.x + 20.0f, panelPos.y + 10.0f};
    DrawTextEx(ctx->mainFont, "操作指南", textPos, 30, 1, CYBER_CYAN);
    DrawTextEx(ctx->mainFont, "↑↓ 选择行动", {textPos.x, textPos.y + 40.0f}, 26, 1, WHITE);
    DrawTextEx(ctx->mainFont, "Enter 确认", {textPos.x, textPos.y + 70.0f}, 26, 1, WHITE);
    DrawTextEx(ctx->mainFont, "Esc 返回/取消", {textPos.x, textPos.y + 100.0f}, 24, 1, WHITE);
    
    // 绘制玩家信息（左侧）
    float playerX = 100; // 玩家信息区域左上角 X
    float playerY = ctx->screenHeight / 2 - 100; // 玩家信息区域左上角 Y
    
    DrawTextEx(ctx->mainFont, "Taffy", {playerX, playerY}, 36, 1, WHITE);
    
    char hpText[64]; // 复用的数值文本缓冲
    std::sprintf(hpText, "HP: %d/%d", ctx->player.stats.hp, ctx->player.stats.maxHp);
    DrawTextEx(ctx->mainFont, hpText, {playerX, playerY + 50}, 28, 1, CYBER_LIME);
    
    // 绘制HP条
    float hpBarWidth = 200; // 玩家血条宽度
    float hpBarHeight = 20; // 玩家血条高度
    float hpPercent = (float)ctx->player.stats.hp / ctx->player.stats.maxHp; // 百分比
    DrawRectangle(playerX, playerY + 90, hpBarWidth, hpBarHeight, DARKGRAY);
    DrawRectangle(playerX, playerY + 90, hpBarWidth * hpPercent, hpBarHeight, GREEN);
    
    // 绘制MP文本
    char mpText[64];
    std::sprintf(mpText, "MP: %d/%d", ctx->player.stats.mp, ctx->player.stats.maxMp);
    DrawTextEx(ctx->mainFont, mpText, {playerX, playerY + 120}, 28, 1, SKYBLUE);
    
    // 绘制MP条
    float mpPercent = (float)ctx->player.stats.mp / ctx->player.stats.maxMp;
    DrawRectangle(playerX, playerY + 160, hpBarWidth, hpBarHeight, DARKGRAY);
    DrawRectangle(playerX, playerY + 160, hpBarWidth * mpPercent, hpBarHeight, BLUE);
    
    // 绘制敌人信息（右侧）
    if (data->currentEnemy) {
        float enemyX = ctx->screenWidth - 400; // 敌人信息区域 X
        float enemyY = ctx->screenHeight / 2 - 100; // 敌人信息区域 Y
        
        DrawTextEx(ctx->mainFont, "敌人", {enemyX, enemyY}, 36, 1, CYBER_RED);
        
        std::sprintf(hpText, "HP: %d/%d", data->currentEnemy->stats.hp, data->currentEnemy->stats.maxHp);
        DrawTextEx(ctx->mainFont, hpText, {enemyX, enemyY + 50}, 28, 1, CYBER_MAGENTA);
        
        // 绘制敌人HP条
        float enemyHpPercent = (float)data->currentEnemy->stats.hp / data->currentEnemy->stats.maxHp; // 敌人血量百分比
        DrawRectangle(enemyX, enemyY + 90, hpBarWidth, hpBarHeight, DARKGRAY);
        DrawRectangle(enemyX, enemyY + 90, hpBarWidth * enemyHpPercent, hpBarHeight, RED);
    }
    
    // 绘制战斗消息
    if (!data->battleMessage.empty()) {
        float msgY = ctx->screenHeight - 200; // 信息框顶部 Y
        DrawRectangle(50, msgY - 20, ctx->screenWidth - 100, 80, UI_BACKGROUND);
        DrawRectangleLines(50, msgY - 20, ctx->screenWidth - 100, 80, CYBER_CYAN);
        DrawTextEx(ctx->mainFont, data->battleMessage.c_str(), {70, msgY}, 32, 1, UI_TEXT);
    }
    
    // 绘制行动菜单（仅在玩家回合）
    if (data->currentPhase == COMBAT_PHASE_PLAYER_TURN) {
        float menuX = ctx->screenWidth / 2 - 200; // 行动菜单左上角 X
        float menuY = ctx->screenHeight - 300;    // 行动菜单左上角 Y
        
        DrawRectangle(menuX - 20, menuY - 20, 400, 200, UI_BACKGROUND);
        DrawRectangleLines(menuX - 20, menuY - 20, 400, 200, CYBER_CYAN);
        
        for (size_t i = 0; i < data->actionNames.size(); i++) {
            Color textColor = ((int)i == data->selectedAction) ? CYBER_CYAN : WHITE;
            const char* prefix = ((int)i == data->selectedAction) ? "> " : "  ";
            char optionText[128]; // 逐项绘制的菜单文字缓冲
            std::sprintf(optionText, "%s%s", prefix, data->actionNames[i].c_str());
            DrawTextEx(ctx->mainFont, optionText, {menuX, menuY + i * 40}, 32, 1, textColor);
        }

        if (data->skillMenuActive) {
            float skillX = menuX + 420;
            float skillY = menuY;
            DrawRectangle(skillX - 20, skillY - 20, 320, 160, UI_BACKGROUND);
            DrawRectangleLines(skillX - 20, skillY - 20, 320, 160, CYBER_LIME);
            DrawTextEx(ctx->mainFont, "选择技能", {skillX, skillY - 10}, 24, 1, CYBER_LIME);
            for (size_t i = 0; i < data->skillNames.size(); ++i) {
                Color textColor = ((int)i == data->selectedSkillIndex) ? CYBER_LIME : WHITE;
                const char* prefix = ((int)i == data->selectedSkillIndex) ? "> " : "  ";
                char skillText[128];
                std::sprintf(skillText, "%s%s", prefix, data->skillNames[i].c_str());
                DrawTextEx(ctx->mainFont, skillText, {skillX, skillY + 30 + (float)i * 30}, 24, 1, textColor);
            }
            DrawTextEx(ctx->mainFont, "Esc 返回", {skillX, skillY + 120}, 20, 1, GRAY);
        }
    }
    
    // 绘制回合数
    char turnText[64]; // 回合数显示缓冲
    std::sprintf(turnText, "回合: %d", data->turnCount);
    DrawTextEx(ctx->mainFont, turnText, {ctx->screenWidth - 200, 20}, 24, 1, GRAY);
    
    // 绘制提示信息
    if (data->currentPhase == COMBAT_PHASE_VICTORY || data->currentPhase == COMBAT_PHASE_DEFEAT) {
        DrawTextEx(ctx->mainFont, "按 ENTER 继续", {ctx->screenWidth / 2 - 150, ctx->screenHeight - 50}, 28, 1, YELLOW);
    }
}

//=========================战斗逻辑辅助函数=========================

void ProcessPlayerAction(GameContext* ctx, CombatData* data, CombatAction action)
{
    char msg[256]; // 行动阶段临时消息缓冲
    if(action != ACTION_DEFEND)
    {
        data->playerDefending = false;
    }
    bool consumesTurn = true;
    
    switch (action)
    {
        case ACTION_ATTACK:
        {
            // 计算伤害（包含浮动与暴击）
            int baseDamage = std::max(1, ctx->player.stats.attack - data->currentEnemy->stats.defense);
            int variance = GetRandomValue(-2, 2); // ±2 点浮动
            bool isCrit = GetRandomValue(0, 100) < 15; // 15% 暴击概率
            int damage = std::max(1, baseDamage + variance);
            if (isCrit) {
                damage *= 2;
            }
            data->currentEnemy->stats.hp -= damage;
            data->damageDealt = damage;
            
            if (isCrit) {
                std::sprintf(msg, "Taffy 暴击造成 %d 点伤害！", damage);
            } else {
                std::sprintf(msg, "Taffy 对 敌人 造成了 %d 点伤害！", damage);
            }
            data->battleMessage = msg;
            data->messageTimer = 2.0f;
            data->animationTimer = 1.0f;
            data->playerAnimating = true;
            
            data->currentPhase = COMBAT_PHASE_ANIMATION;
            break;
        }
        
        case ACTION_SKILL:
        {
            if (!data->skillMenuActive) {
                data->skillMenuActive = true;
                data->selectedSkillIndex = 0;
                data->battleMessage = "选择技能";
                data->messageTimer = 0.0f;
            }
            consumesTurn = false;
            break;
        }
        
        case ACTION_DEFEND:
        {
            data->battleMessage = "Taffy 进入了防御姿态！";
            data->messageTimer = 1.5f;
            data->playerDefending = true;
            // 下回合受到的伤害减半（需要在敌人回合实现）
            data->currentPhase = COMBAT_PHASE_ENEMY_TURN;
            break;
        }
        
        case ACTION_ESCAPE:
        {
            // 逃跑成功率判断（简单实现：50%概率）
            if (GetRandomValue(0, 100) < 50) {
                data->battleMessage = "成功逃脱了！";
                data->messageTimer = 2.0f;
                data->currentPhase = COMBAT_PHASE_VICTORY; // 借用胜利流程返回
            } else {
                data->battleMessage = "逃跑失败了！";
                data->messageTimer = 1.5f;
                data->currentPhase = COMBAT_PHASE_ENEMY_TURN;
            }
            break;
        }
    }

    if (consumesTurn) {
        data->turnCount++;
    }
}

void UseSkill(GameContext* ctx, CombatData* data, int skillIndex)
{
    if (!ctx || !data) return;

    data->skillMenuActive = false;
    char msg[256];
//=================================12/6 Warning新增简单技能系统
    switch (skillIndex)
    {
        case 0: // 代码风暴：高伤害输出
        {
            if (!data->currentEnemy) return;
            int baseDamage = std::max(1, static_cast<int>(ctx->player.stats.attack * 2.5f) - data->currentEnemy->stats.defense / 2);
            int variance = GetRandomValue(-3, 3);
            int damage = std::max(1, baseDamage + variance);
            bool overload = GetRandomValue(0, 100) < 10; // 小概率额外超载
            if (overload) {
                damage = static_cast<int>(damage * 1.3f);
            }
            data->currentEnemy->stats.hp -= damage;
            data->damageDealt = damage;
            if (overload) {
                std::snprintf(msg, sizeof(msg), "代码风暴超载，对敌人造成 %d 点伤害！", damage);
            } else {
                std::snprintf(msg, sizeof(msg), "代码风暴命中，造成 %d 点伤害！", damage);
            }
            data->battleMessage = msg;
            data->messageTimer = 2.0f;
            data->animationTimer = 1.2f;
            data->playerAnimating = true;
            data->currentPhase = COMBAT_PHASE_ANIMATION;
            data->turnCount++;
            break;
        }
        case 1: // 纳米注入：回复生命
        {
            int baseHeal = static_cast<int>(ctx->player.stats.maxHp * 0.1f);
            baseHeal += GetRandomValue(0, 8);
            ctx->player.stats.hp = std::min(ctx->player.stats.maxHp, ctx->player.stats.hp + baseHeal);
            std::snprintf(msg, sizeof(msg), "纳米注入恢复 %d 点生命！", baseHeal);
            data->battleMessage = msg;
            data->messageTimer = 1.8f;
            data->animationTimer = 0.0f;
            data->playerAnimating = false;
            data->currentPhase = COMBAT_PHASE_ENEMY_TURN;
            data->turnCount++;
            break;
        }
        default:
        {
            data->battleMessage = "未知技能";
            data->messageTimer = 1.0f;
            data->currentPhase = COMBAT_PHASE_PLAYER_TURN;
            break;
        }
    }
}

void ProcessEnemyTurn(GameContext* ctx, CombatData* data)
{
    if (!data->currentEnemy) return;
    
    // 简单的敌人AI：直接攻击
    int baseDamage = std::max(1, data->currentEnemy->stats.attack - ctx->player.stats.defense);
    int variance = GetRandomValue(-1, 2); // 敌人浮动略小
    bool isCrit = GetRandomValue(0, 100) < 10; // 10% 暴击概率
    int damage = std::max(1, baseDamage + variance);
    if (isCrit) {
        damage = std::max(1, damage * 2);
    }
    bool defended = data->playerDefending; // 记录玩家是否防御，用于减伤与提示
    if(defended)
    {
        damage = std::max(1, damage / 2);
        data->playerDefending = false;
    }
    ctx->player.stats.hp -= damage;
    
    char msg[256]; // 敌人回合消息缓冲
    if(defended)
    {
        if (isCrit) {
            std::sprintf(msg, "敌人暴击被格挡，对 Taffy 造成 %d 点伤害！", damage);
        } else {
            std::sprintf(msg, "敌人 对 Taffy 造成了 %d 点伤害！（防御减伤）", damage);
        }
    }
    else
    {
        if (isCrit) {
            std::sprintf(msg, "敌人暴击，对 Taffy 造成了 %d 点伤害！", damage);
        } else {
            std::sprintf(msg, "敌人 对 Taffy 造成了 %d 点伤害！", damage);
        }
    }
    data->battleMessage = msg;
    data->messageTimer = 2.0f;
    data->animationTimer = 1.0f;
    data->enemyAnimating = true;
    
    data->currentPhase = COMBAT_PHASE_ANIMATION;
}

void CheckBattleEnd(GameContext* ctx, CombatData* data)
{
    // 检查敌人是否被击败
    if (data->currentEnemy && data->currentEnemy->stats.hp <= 0) {
        data->currentEnemy->isActive = false;

        // 战斗胜利时恢复 40% 已损失的生命值（只执行一次）
        int missingHp = ctx->player.stats.maxHp - ctx->player.stats.hp;
        if (missingHp > 0) {
            int healAmount = static_cast<int>(0.4f * missingHp);
            if (healAmount > 0) {
                ctx->player.stats.hp = std::min(ctx->player.stats.hp + healAmount, ctx->player.stats.maxHp);
            }
        }

        if (!data->postDialogueScript.empty() && !data->postDialogueQueued) {
            GameState* nextState = createExplorationState();
            if (nextState) {
                GameState* dialogueState = CreateDialogueStateForScript(
                    data->postDialogueScript,
                    nextState,
                    true);
                if (dialogueState) {
                    data->postDialogueQueued = true;
                    TraceLog(LOG_INFO, "[Combat] 触发战后对话：%s", data->postDialogueScript.c_str());
                    GameStateMachine_change(&ctx->state_machine, ctx, dialogueState);
                    return;
                }
            }
        }

        data->battleMessage = "战斗胜利！";
        data->currentPhase = COMBAT_PHASE_VICTORY;
        return;
    }
    
    // 检查玩家是否被击败
    if (ctx->player.stats.hp <= 0) {
        data->battleMessage = "战斗失败...";
        data->currentPhase = COMBAT_PHASE_DEFEAT;
        return;
    }
    
    // 战斗继续，根据上一个阶段决定下一个回合
    if (data->playerAnimating) {
        data->playerAnimating = false;
        data->currentPhase = COMBAT_PHASE_ENEMY_TURN;
    } else if (data->enemyAnimating) {
        data->enemyAnimating = false;
        data->currentPhase = COMBAT_PHASE_PLAYER_TURN;
        data->battleMessage = "选择你的行动";
    }
}

//=========================战斗状态创建=========================

GameState* CreateCombatState(GameContext* ctx, Enemy* targetEnemy)
{
    if (!targetEnemy) {
        TraceLog(LOG_ERROR, "[Combat] 创建失败：缺少敌人目标");
        return nullptr;
    }

    CombatData* data = new CombatData(); // 为战斗状态分配的数据块
    if (!data) {
        TraceLog(LOG_ERROR, "[Combat] 内存分配失败");
        return nullptr;
    }
    
    data->currentEnemy = targetEnemy;
    data->postDialogueQueued = false;

    if (ctx) {
        data->preDialogueScript = ctx->pendingPreCombatDialogue;
        data->postDialogueScript = ctx->pendingPostCombatDialogue;
        ctx->pendingPreCombatDialogue.clear();
        ctx->pendingPostCombatDialogue.clear();
    } else {
        data->preDialogueScript.clear();
        data->postDialogueScript.clear();
    }
    
    GameState* combatState = Gamestate_create(
        combat_enter,
        combat_exit,
        combat_update,
        combat_render,
        data,
        sizeof(CombatData)
    );
    // combatState 即注册到状态机的战斗 GameState
    
    if (!combatState) {
        TraceLog(LOG_ERROR, "[Combat] 创建状态失败");
        delete data;
        return nullptr;
    }

    if (!data->preDialogueScript.empty()) {
        GameState* dialogueState = CreateDialogueStateForScript(
            data->preDialogueScript,
            combatState,
            false);
        if (dialogueState) {
            TraceLog(LOG_INFO, "[Combat] 触发战前对话：%s", data->preDialogueScript.c_str());
            data->preDialogueScript.clear();
            return dialogueState;
        }
    }
    
    TraceLog(LOG_INFO, "[Combat] 战斗状态创建成功");
    return combatState;
}
