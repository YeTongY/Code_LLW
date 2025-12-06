//2025/11/19 Warning开始编写战斗模块
#pragma once
#include "GameState.h" 
#include <string>
#include <vector>

// 战斗阶段枚举
enum CombatPhase
{
    COMBAT_PHASE_START,      // 战斗开始动画
    COMBAT_PHASE_PLAYER_TURN,// 玩家回合（选择行动）
    COMBAT_PHASE_ENEMY_TURN, // 敌人回合
    COMBAT_PHASE_ANIMATION,  // 动画播放中
    COMBAT_PHASE_VICTORY,    // 胜利结算
    COMBAT_PHASE_DEFEAT      // 失败结算
};

// 战斗行动类型
enum CombatAction
{
    ACTION_ATTACK,    // 普通攻击
    ACTION_SKILL,     // 使用技能
    ACTION_DEFEND,    // 防御
    ACTION_ESCAPE     // 逃跑
};

// 战斗数据
struct CombatData
{
    bool isActive;              // 战斗是否激活
    CombatPhase currentPhase;   // 当前阶段
    
    // 参战单位（引用 GameContext 中的数据）
    Enemy* currentEnemy;        // 当前战斗的敌人指针
    bool playerDefending;       // 玩家是否处于防御姿态
    
    // UI 状态
    int selectedAction;         // 当前选中的行动（0-3对应上面的枚举）
    std::vector<std::string> actionNames; // 行动名称列表
    
    // 战斗消息
    std::string battleMessage;  // 当前显示的战斗信息
    float messageTimer;         // 消息显示计时器
    
    // 动画/效果
    float animationTimer;       // 动画计时器
    bool playerAnimating;       // 玩家是否在播放动画
    bool enemyAnimating;        // 敌人是否在播放动画
    
    // 战斗统计
    int turnCount;              // 回合数
    int damageDealt;            // 本回合造成的伤害

    // 对话脚本
    std::string preDialogueScript;   // 战前的对话脚本路径
    std::string postDialogueScript;  // 战后的对话脚本路径
    bool postDialogueQueued;         // 是否已经触发过战后对话

    // 技能菜单
    bool skillMenuActive;            // 是否正在选择技能
    int selectedSkillIndex;          // 当前高亮的技能
    std::vector<std::string> skillNames; // 可用技能列表
};

// 战斗状态生命周期函数
void combat_enter(GameContext* ctx, void* state_data);
void combat_exit(GameContext* ctx, void* state_data);
void combat_update(GameContext* ctx, void* state_data);
void combat_render(GameContext* ctx, void* state_data);

// 战斗状态创建函数
GameState* CreateCombatState(GameContext* ctx, Enemy* targetEnemy);

// 战斗逻辑辅助函数
void ProcessPlayerAction(GameContext* ctx, CombatData* data, CombatAction action);
void ProcessEnemyTurn(GameContext* ctx, CombatData* data);
void CheckBattleEnd(GameContext* ctx, CombatData* data);
bool UseSkill(GameContext* ctx, CombatData* data, int skillIndex);