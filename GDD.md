仅供参考，仍需修改

-----

# Code LLW

## 1\. 概览 (Project Overview)

  * **游戏代号:** `LLW`
  * **游戏类型:** 顶视图 (Top-Down) JRPG（回合制战斗）
  * **目标平台:** Windows (PC)
  * **团队:** 3 人
  * **开发周期:** 25 天
  * **核心 Logline:** 一种神秘的数据病毒“刘立伟”席卷了第25届科协，唯一的“免疫者”Taffy必须深入沦陷的总部，在“净化”被感染同伴的同时，揭开病毒背后隐藏的，关于科协自身的最大秘密。

## 2\. 核心原则与强制限制

这是本项目的**最高法则**，必须无条件遵守，以防止项目在25天内失败。

1.  **【引擎先行，内容其次】:** 我们的首要目标是构建一个**可运行的核心JRPG引擎**（地图移动、战斗触发、回合制战斗、胜利/失败循环）。**剧情、关卡、Boss**都是在此引擎上“填充”的内容，做不完就砍，但引擎必须完整。
2.  **【“C风格”的C++】:** 必须使用 C++ 语言。
      * **允许 (Allowed):** `std::vector`, `std::map`, `struct`, `enum`, `<fstream>`, `std::string` 等 C++ 提供的便捷**数据结构**和工具。
      * **禁止 (BANNED):** **绝对禁止**使用 C++ 的**面向对象 (OOP) 特性**。包括 `class` 关键字、继承、虚函数、`public/private` 访问修饰符等。所有逻辑必须通过 `struct` 和 `function`（面向过程）实现。
3.  **【“格子”是唯一的逻辑】:**
      * **地图逻辑:** 游戏世界**必须**基于一个二维数组（网格）。
      * **地图渲染:** 可以使用“Zelda/宝可梦”风格的 2.5D 质感美术资源，**但代码逻辑严禁**偏离“顶视走格子”。
      * **物理:** **严禁**任何形式的实时物理、重力、跳跃或实时碰撞。
4.  **【范围控制 (Scope Control)】:** 任何一个新功能（如“一个新Boss的独特AI”）如果预估耗时超过2天，应立即砍掉或简化。

## 3\. 25天“垂直切片”目标 (MVP)

我们的“完成”标准，是交付一个包含以下所有要素的\*\*“第1关演示版”\*\*：

1.  **[状态: 菜单]** 一个简单的标题画面，按回车键开始。
2.  **[状态: 地图]** 玩家 Taffy 出现在一个**单屏**地图（用 `Raylib` 绘制）。
3.  **[地图逻辑]** 玩家可以“按住”方向键，在网格上“哒哒哒”地**平滑移动**（采用“按住+冷却”的方案）。
4.  **[地图逻辑]** 地图上有一个可见的“杂兵”（被感染的成员）。
5.  **[状态切换: 战斗]** 玩家**触碰**杂兵，游戏**切换**到“战斗状态”。
6.  **[状态: 战斗]** 屏幕上出现 Taffy 和敌人。
7.  **[战斗逻辑]** 出现“1. 攻击 2. 逃跑”菜单。
8.  **[战斗逻辑]** 玩家选择“攻击”，双方（玩家、AI）各行动一次。
9.  **[战斗逻辑]** 战斗直到一方 HP 归零。
10. **[状态切换: 地图]** 战斗胜利或失败后，**切换**回“地图状态”。如果胜利，杂兵消失。
11. **[剧情/结局]** 地图的尽头站着“mmk”，触碰后显示几行对话（用`DrawText`），然后显示 “To Be Continued...”。

## 4\. 技术栈 (Technical Stack)

  * **语言:** C++ (使用 C++17 或更高标准)
  * **图形/音频/输入库:** **Raylib** (纯 C 库，完美契合“No-OOP”规则)
  * **版本控制:** **Git** (3人协作必须使用)
  * **编译器:** G++ (MinGW) / MSVC (Visual Studio)

## 5\. 核心引擎设计 (Engine Design)

### a. 游戏状态机 (Game State Machine)

这是整个引擎的核心。

```cpp
// (在某个全局的 "GameState.h" 中)
enum GameState {
    STATE_TITLE_SCREEN,
    STATE_MAP,
    STATE_COMBAT,
    STATE_DIALOGUE
};

// 全局变量来管理状态
GameState g_currentState = STATE_TITLE_SCREEN;

// 在 main.cpp 的主循环中
while (!WindowShouldClose()) {
    // 1. 更新逻辑
    switch (g_currentState) {
        case STATE_MAP:      updateMap();      break;
        case STATE_COMBAT:   updateCombat();   break;
        case STATE_DIALOGUE: updateDialogue(); break;
        // ...
    }
    
    // 2. 绘制画面
    BeginDrawing();
    ClearBackground(BLACK);
    switch (g_currentState) {
        case STATE_MAP:      drawMap();      break;
        case STATE_COMBAT:   drawCombat();   break;
        case STATE_DIALOGUE: drawDialogue(); break;
        // ...
    }
    EndDrawing();
}
```

### b. 地图系统 (Map System)

  * **数据:** `std::vector<std::vector<int>> currentMap;` (0=空地, 1=墙, 2=敌人...)
  * **移动:** 采用“按住+冷却”方案（见 `user` 的“现代化包装”讨论）。
  * **摄像机:** 采用“平滑摄像机”方案 (Lerp) 来追赶玩家，营造2.5D质感。

### c. 战斗引擎 (Combat Engine)

  * **数据:** 战斗开始时，从“地图”状态传入 `Player` 和 `Enemy` 的数据。
  * **回合:** `bool isPlayerTurn = true;`
  * **AI:** 敌人AI极其简单：`if (!isPlayerTurn) { player.hp -= (enemy.attack - player.defense); isPlayerTurn = true; }`
  * **UI:** 纯 `DrawText` 绘制的菜单。

### d. 剧情/事件系统 (Event System)

  * **对话:** `std::vector<std::string> currentDialogue;`，按键逐行显示。
  * **“标志”系统:** 这是实现你们完整剧情的关键（如果时间充裕）。
    `std::map<std::string, bool> g_gameFlags;`
      * *示例:* `g_gameFlags["defeated_boss_1"] = true;`
      * 一个NPC的对话可以检查这个flag：`if (g_gameFlags["defeated_boss_1"]) { ... }`

## 6\. 核心数据结构 (C-Style C++ Structs)

这是你们可以直接开始写的代码，**禁止**在 `struct` 中加入**任何函数**。

```cpp
#include <string>
#include <vector>
#include <map>
#include "raylib.h"

// 游戏全局状态，用于 C 风格的“传递”
struct GameEngine; 

// --- 基础数据 ---

// 角色和敌人的通用属性
struct Stats {
    int hp;
    int maxHp;
    int attack;
    int defense;
};

// --- 地图实体 ---

struct Player {
    int gridX; // 逻辑位置 X
    int gridY; // 逻辑位置 Y
    Stats stats;
    // (其他: 名字, 贴图...)
};

struct Enemy {
    int id;
    std::string name;
    int gridX;
    int gridY;
    Stats stats;
    bool isDefeated; // 是否已被击败 (用于在地图上移除)
};

// --- 战斗专用 ---

struct CombatState {
    bool isActive;
    bool isPlayerTurn;
    Enemy* currentOpponent; // 指向正在战斗的敌人
    int menuChoice; // 0=攻击, 1=逃跑
    std::string message; // "Taffy 造成了 10 点伤害！"
};

// --- 引擎核心 ---

// 这个结构体将保存游戏的所有状态
// 我们把它作为参数传递给 C 风格的函数
// e.g. updateMap(GameEngine& engine);
// e.g. drawMap(GameEngine& engine);
struct GameEngine {
    // 状态
    GameState currentState;
    
    // 实体
    Player player;
    std::vector<Enemy> mapEnemies; // 当前地图上的所有敌人
    
    // 战斗
    CombatState combat;
    
    // 地图 & 摄像机
    Camera2D camera;
    std::vector<std::vector<int>> currentMapData;
    
    // 剧情
    std::map<std::string, bool> gameFlags;
    std::vector<std::string> currentDialogue;
};
```

## 7\. 25天冲刺计划 (Milestone Plan)

  * **Week 1 (Days 1-7): 地基**

      * [ ] (3人) 搭建好 Git 仓库、Raylib 环境，所有人都能跑通一个“Hello World”窗口。
      * [ ] (1人) 实现 `GameEngine` 结构体和 `GameState` 状态机（能按键在“地图”和“战斗”状态间切换）。
      * [ ] (1人) 实现地图加载和绘制（能从文件读取 `mapData` 并画出来）。
      * [ ] (1人) 实现玩家在地图上的\*\*“瞬移”\*\*（最简单的 `playerX++`）。

  * **Week 2 (Days 8-14): 核心循环**

      * [ ] (2人) **攻坚：** 实现完整的**回合制战斗引擎**（`updateCombat` / `drawCombat`），实现MVP第6-10条。
      * [ ] (1人) **优化：** 将“瞬移”升级为“按住+冷却”的现代化移动。
      * [ ] (1人) **优化：** 实现“平滑摄像机” (Lerp Camera)。

  * **Week 3 (Days 15-21): 填充内容**

      * [ ] (1人) 实现对话系统 (`updateDialogue` / `drawDialogue`)。
      * [ ] (1人) 实现 `g_gameFlags` 剧情标志系统。
      * [ ] (1人) 制作第一个 Boss (一个血更厚的敌人，有独立的对话)。
      * [ ] (全员) 寻找并集成音效和背景音乐。

  * **Week 4 (Days 22-25): 抛光与交付**

      * [ ] **功能冻结 (Feature Freeze)！禁止添加任何新功能！**
      * [ ] (全员) 疯狂测试，修复 Bug。
      * [ ] (全员) 确保项目能干净地编译和运行。
      * [ ] (全员) 完成项目，准备展示！

-----

这份文档就是你们的蓝图。你们是否同意以此为基础开始行动？