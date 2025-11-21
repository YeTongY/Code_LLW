#include "Combat.h"
#include "ExplorationState.h"
#include "raylib.h"
#include "GameState.h"
#include <cstdio>
#include <algorithm>

//=========================战斗状态实现=========================

void combat_enter(GameContext* ctx, void* state_data)
{
    CombatData* data = static_cast<CombatData*>(state_data);
    
    data->isActive = true;
    data->currentPhase = COMBAT_PHASE_START;
    data->selectedAction = 0;
    data->turnCount = 0;
    data->messageTimer = 0.0f;
    data->animationTimer = 0.0f;
    data->playerAnimating = false;
    data->enemyAnimating = false;
    data->damageDealt = 0;
    
    // 初始化行动选项
    data->actionNames.clear();
    data->actionNames.push_back("攻击");
    data->actionNames.push_back("技能");
    data->actionNames.push_back("防御");
    data->actionNames.push_back("逃跑");
    
    // 设置初始消息
    if (data->currentEnemy) {
        data->battleMessage = "敌人出现了！";
        data->messageTimer = 2.0f; // 显示2秒
    }
    
    TraceLog(LOG_INFO, "[Combat] 进入战斗状态");
}

void combat_exit(GameContext* ctx, void* state_data)
{
    CombatData* data = static_cast<CombatData*>(state_data);
    
    if (data) {
        data->isActive = false;
        delete data;
    }
    
    TraceLog(LOG_INFO, "[Combat] 退出战斗状态");
}

void combat_update(GameContext* ctx, void* state_data)
{
    CombatData* data = static_cast<CombatData*>(state_data);
    
    if (!data->isActive) return;
    
    float deltaTime = GetFrameTime();
    
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
            if (IsKeyPressed(KEY_DOWN)) {
                data->selectedAction = (data->selectedAction + 1) % 4;
            }
            else if (IsKeyPressed(KEY_UP)) {
                data->selectedAction = (data->selectedAction - 1 + 4) % 4;
            }
            else if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                ProcessPlayerAction(ctx, data, static_cast<CombatAction>(data->selectedAction));
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
                // 标记敌人为已击败
                if (data->currentEnemy) {
                    data->currentEnemy->isActive = false;
                }
                
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
    CombatData* data = static_cast<CombatData*>(state_data);
    
    if (!data->isActive) return;
    
    ClearBackground(BLACK);
    
    // 绘制战斗背景
    DrawRectangle(0, 0, ctx->screenWidth, ctx->screenHeight, (Color){10, 10, 20, 255});
    
    // 绘制标题
    DrawTextEx(ctx->mainFont, "战斗模式", {20, 20}, 32, 1, CYBER_CYAN);
    
    // 绘制玩家信息（左侧）
    float playerX = 100;
    float playerY = ctx->screenHeight / 2 - 100;
    
    DrawTextEx(ctx->mainFont, "Taffy", {playerX, playerY}, 36, 1, WHITE);
    
    char hpText[64];
    std::sprintf(hpText, "HP: %d/%d", ctx->player.stats.hp, ctx->player.stats.maxHp);
    DrawTextEx(ctx->mainFont, hpText, {playerX, playerY + 50}, 28, 1, CYBER_LIME);
    
    // 绘制HP条
    float hpBarWidth = 200;
    float hpBarHeight = 20;
    float hpPercent = (float)ctx->player.stats.hp / ctx->player.stats.maxHp;
    DrawRectangle(playerX, playerY + 90, hpBarWidth, hpBarHeight, DARKGRAY);
    DrawRectangle(playerX, playerY + 90, hpBarWidth * hpPercent, hpBarHeight, GREEN);
    
    // 绘制敌人信息（右侧）
    if (data->currentEnemy) {
        float enemyX = ctx->screenWidth - 400;
        float enemyY = ctx->screenHeight / 2 - 100;
        
        DrawTextEx(ctx->mainFont, "敌人", {enemyX, enemyY}, 36, 1, CYBER_RED);
        
        std::sprintf(hpText, "HP: %d/%d", data->currentEnemy->stats.hp, data->currentEnemy->stats.maxHp);
        DrawTextEx(ctx->mainFont, hpText, {enemyX, enemyY + 50}, 28, 1, CYBER_MAGENTA);
        
        // 绘制敌人HP条
        float enemyHpPercent = (float)data->currentEnemy->stats.hp / data->currentEnemy->stats.maxHp;
        DrawRectangle(enemyX, enemyY + 90, hpBarWidth, hpBarHeight, DARKGRAY);
        DrawRectangle(enemyX, enemyY + 90, hpBarWidth * enemyHpPercent, hpBarHeight, RED);
    }
    
    // 绘制战斗消息
    if (!data->battleMessage.empty()) {
        float msgY = ctx->screenHeight - 200;
        DrawRectangle(50, msgY - 20, ctx->screenWidth - 100, 80, UI_BACKGROUND);
        DrawRectangleLines(50, msgY - 20, ctx->screenWidth - 100, 80, CYBER_CYAN);
        DrawTextEx(ctx->mainFont, data->battleMessage.c_str(), {70, msgY}, 32, 1, UI_TEXT);
    }
    
    // 绘制行动菜单（仅在玩家回合）
    if (data->currentPhase == COMBAT_PHASE_PLAYER_TURN) {
        float menuX = ctx->screenWidth / 2 - 200;
        float menuY = ctx->screenHeight - 300;
        
        DrawRectangle(menuX - 20, menuY - 20, 400, 200, UI_BACKGROUND);
        DrawRectangleLines(menuX - 20, menuY - 20, 400, 200, CYBER_CYAN);
        
        for (size_t i = 0; i < data->actionNames.size(); i++) {
            Color textColor = ((int)i == data->selectedAction) ? CYBER_CYAN : WHITE;
            const char* prefix = ((int)i == data->selectedAction) ? "> " : "  ";
            char optionText[128];
            std::sprintf(optionText, "%s%s", prefix, data->actionNames[i].c_str());
            DrawTextEx(ctx->mainFont, optionText, {menuX, menuY + i * 40}, 32, 1, textColor);
        }
    }
    
    // 绘制回合数
    char turnText[64];
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
    char msg[256];
    
    switch (action)
    {
        case ACTION_ATTACK:
        {
            // 计算伤害
            int damage = std::max(1, ctx->player.stats.attack - data->currentEnemy->stats.defense);
            data->currentEnemy->stats.hp -= damage;
            data->damageDealt = damage;
            
            std::sprintf(msg, "Taffy 对 敌人 造成了 %d 点伤害！", damage);
            data->battleMessage = msg;
            data->messageTimer = 2.0f;
            data->animationTimer = 1.0f;
            data->playerAnimating = true;
            
            data->currentPhase = COMBAT_PHASE_ANIMATION;
            break;
        }
        
        case ACTION_SKILL:
        {
            // 暂未实现技能系统
            data->battleMessage = "技能系统尚未实现！";
            data->messageTimer = 1.5f;
            break;
        }
        
        case ACTION_DEFEND:
        {
            data->battleMessage = "Taffy 进入了防御姿态！";
            data->messageTimer = 1.5f;
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
    
    data->turnCount++;
}

void ProcessEnemyTurn(GameContext* ctx, CombatData* data)
{
    if (!data->currentEnemy) return;
    
    // 简单的敌人AI：直接攻击
    int damage = std::max(1, data->currentEnemy->stats.attack - ctx->player.stats.defense);
    ctx->player.stats.hp -= damage;
    
    char msg[256];
    std::sprintf(msg, "敌人 对 Taffy 造成了 %d 点伤害！", damage);
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

GameState* CreateCombatState(Enemy* targetEnemy)
{
    CombatData* data = new CombatData();
    if (!data) {
        TraceLog(LOG_ERROR, "[Combat] 内存分配失败");
        return nullptr;
    }
    
    data->currentEnemy = targetEnemy;
    
    GameState* state = Gamestate_create(
        combat_enter,
        combat_exit,
        combat_update,
        combat_render,
        data,
        sizeof(CombatData)
    );
    
    if (!state) {
        TraceLog(LOG_ERROR, "[Combat] 创建状态失败");
        delete data;
        return nullptr;
    }
    
    TraceLog(LOG_INFO, "[Combat] 战斗状态创建成功");
    return state;
}
