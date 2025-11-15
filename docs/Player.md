# `LLW` 项目 - 角色模块 (`Player`) 说明文档

**日期:** 2025-11-15

## 1. 概述 (Overview)

本模块 (`Player`) 负责 GDD P0（Week 1）阶段的**所有**与玩家相关的逻辑。

本模块是 P0 阶段的**“渲染总指挥”**。它**“拥有”** `raylib` 摄像机 (`Camera2D`)，并负责“指挥” `Map` 模块在摄像机视野内绘制。

## 2. 核心文件

* **`src/Player.h`**
    * “蓝图”文件。
    * **定义**了 `struct Stats` 和 `struct Player` 的数据结构。
    * **声明**了两个“公共”函数：`updatePlayer` 和 `drawPlayer`。
* **`src/Player.cpp`**
    * “厨房”文件。
    * **实现**了 `updatePlayer` 和 `drawPlayer` 的所有 P0 逻辑。

## 3. P0 核心功能实现

### a. `updatePlayer(GameEngine& engine)`

**职责：** 处理 P0 的“瞬移”和“碰撞检测”。

1.  **意图 (Intent)：** * 监听 `IsKeyPressed(KEY_W/A/S/D)`（“按一下”）。
    * 使用 `else if` 链确保 P0 阶段的“4 向移动约束”（禁止斜向）。
    * 将“意图”存入**临时**的 `nextX, nextY` 变量。
2.  **检查 (Check)：**
    * **安全第一：** 使用“嵌套 `if`”或“短路 `&&`”来确保**“边界检查”**（防止越界崩溃）**永远**在“障碍检查”**之前**执行。
    * **合同：** “假设” `engine.currentMapData` 已被 `Map` 模块正确填充。
    * **合同：** “假设” `0` 是 P0 的可移动地板。
    * **日志：** 使用 `TraceLog` 在“非法”移动时（越界、撞墙）打印警告。
3.  **执行 (Execute)：**
    * **只**在**所有**检查都通过时（`if (canMove)`），才将 `nextX, nextY` “批准”并同步回 `engine.player.gridX, gridY`。

### b. `drawPlayer(const GameEngine& engine)`

**职责：** P0 的“渲染总指挥”。

1.  **摄像机“合同” (GDD v2.5)：**
    * **“拥有”** 摄像机逻辑。
    * **获取** `engine.camera` 的“副本”。
    * **设置 GDD 规范：**
        * `camera.zoom = 3.0f` (3倍整数缩放)。
        * `camera.offset = { 1920/2.0f, 1080/2.0f }` (1080p 窗口居中)。
        * `camera.target = ...` (P0“死跟”玩家的世界坐标中心)。
2.  **渲染“指挥”：**
    * **`BeginMode2D(camera)`：** **“开启”** 3.0 倍缩放的摄像机模式。
    * **`drawMap(engine)`：** **“指挥”**（调用）`Map` 模块，让它在“摄像机视野”里画出地图。
    * **`DrawRectangle(...)`：** **“绘制”** P0 的红色方块（玩家）。
    * **`EndMode2D()`：** **“关闭”** 摄像机模式，恢复 1:1 屏幕绘制（为 P1 的 UI 准备）。

## 4. 团队协作指南 (!!!)

### A. 本模块的“依赖” (Dependencies)
本模块（`Player`）的“施工”**依赖**于以下“合同”：

1.  **依赖 `GameState.h` (队友 C)：**
    * **必须**提供 `GameEngine` 结构体。
    * **必须**在 `GameEngine` 中包含 `Player player`。
    * **必须**在 `GameEngine` 中包含 `Camera2D camera`。
    * **必须**在 `GameEngine` 中包含 `vector<vector<TileType>> currentMapData` (或 `int`)。

2.  **依赖 `Map.h` (队友 A)：**
    * **必须**提供一个 `void drawMap(const GameEngine& engine)` 函数。

3.  **依赖 `Map.cpp` (队友 A)：**
    * `loadMap` **必须**正确填充 `engine.currentMapData`，否则“碰撞检测”会失效。
    * `drawMap` **必须**是一个“愚蠢”的绘制器（只画图块，不碰摄像机）。

### B. “调用”本模块 (Clients)
本模块（`Player`）**必须**被 `main.cpp`（队友 C）调用：

1.  **`main.cpp` 必须：** `#include "Player.h"`。
2.  **`main.cpp` (Update `switch`) 必须：** `case STATE_MAP: updatePlayer(engine); break;`
3.  **`main.cpp` (Draw `switch`) 必须：** `case STATE_MAP: drawPlayer(engine); break;`