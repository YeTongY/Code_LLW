# 字体加载模块 (`FontLoader`) 说明文档

## 1\. 概述 (Overview)

本模块 (`FontLoader`) 的**唯一**职责是解决 `raylib` 默认不支持中文的问题。

它通过一个\*\*“启动时扫描”\*\* (Scan-on-Load) 的“烘焙” (Baking) 过程，来创建一个**包含游戏中所有必需字符**（中英文、数字、符号）的 `Font` 对象。

其他所有模块（如 `Map`、`Combat`、`UI`）**不**应该自己加载字体，而应该**统一使用**本模块提供的 `engine.mainFont` 对象来绘制所有文本。

## 2\. 核心文件

  * **`src/FontLoader.h`**

      * “菜单”/“蓝图”文件。
      * 向 `main.cpp` 等其他文件**声明**（宣告）了两个“公共”函数：
          * `loadGameFont()`
          * `unloadGameFont()`

  * **`src/FontLoader.cpp`**

      * “厨房”/“实现”文件。
      * 包含了**真正**的烘焙逻辑，以及一个“内部”的辅助函数 `textLoader`。

## 3\. 工作原理 (“烘焙”流程)

本模块采用“精确清单”方案，以在“内存占用”和“开发便利”之间取得完美平衡。

`loadGameFont()` 函数在游戏启动时，会执行以下操作：

1.  **读取“烘焙清单”**：
      * 它调用**内部**的 `textLoader()` 函数。
      * `textLoader()` 会读取 `res/data/dialogue/all_game_script.txt` 文件的**全部**内容，存入一个 `std::string`。
2.  **追加“安全网”**：
      * 将所有基础英文字符（A-Z, a-z, 0-9, \!?...）**追加**到这个 `std::string` 的末尾。
3.  **扫描“清单”**：
      * 它调用 `raylib` 的 `LoadCodepoints()`，扫描这个“巨型” `std::string`。
      * `LoadCodepoints()` 会提取出所有**不重复**的字符（比如 `"你好你好"` 只会提取 "你" 和 "好"），并返回一个 `int*` 码点（Codepoint）数组。
4.  **执行“烘焙”**：
      * 它调用 `raylib` 的 `LoadFontEx()`。
      * `LoadFontEx` 拿着“字体菜谱” (`res/fonts/SourceHanSansSC-Regular.otf`) 和“码点清单” (`int* codepoints`)。
      * 它将清单上的**每一个**字符，从字体文件中“烘焙”成指定大小（48px）的**像素画 (Glyph)**。
      * 所有这些“像素画”被拼成一张巨大的“烤盘”（`Texture2D`）。
5.  **存储结果**：
      * 这张“烤盘”和它的“索引卡”被打包成一个 `Font` 对象，**存入** `engine.mainFont`。
6.  **清理**：
      * 调用 `UnloadCodepoints()` 释放临时的 `int*` 清单数组，防止内存泄漏。

## 4\. 团队协作指南 (\!\!\!)

**这是本模块最重要的部分，所有人都必须阅读。**

### A. 如何使用 (For `main.cpp` 程序员)

1.  **包含头文件：**

      * 在 `main.cpp` 的顶部，你**必须** `#include "FontLoader.h"`。

2.  **加载/卸载：**

      * 在 `main()` 函数的**初始化**区域（`InitWindow()` 之后），你**必须**调用 `loadGameFont(engine);`。
      * 在 `main()` 函数的**清理**区域（`CloseWindow()` 之前），你**必须**调用 `unloadGameFont(engine);`。

3.  **绘制文本 (黄金规则)：**

      * **禁止**再使用 `DrawText()`！它不认识中文。
      * 你**必须**使用 `DrawTextEx()`，并**总是**传入 `engine.mainFont` 作为第一个参数。

    <!-- end list -->

    ```cpp
    // 正确的绘制方式：
    DrawTextEx(
        engine.mainFont,                   // 1. 使用我们烘焙好的“主字体”
        "Taffy: 你好，刘立伟病毒！",       // 2. 你的文本
        (Vector2){ 10, 10 },               // 3. 屏幕坐标
        48,                                // 4. 字体大小 (必须和烘焙时一致)
        1,                                 // 5. 字间距
        WHITE                              // 6. 颜色
    );
    ```

### B. 如何添加新台词 (For ALL Teammates)

我们的系统**不会**自动加载所有汉字，它**只**加载“烘焙清单”上的汉字。

**“烘焙清单” = `res/data/dialogue/all_game_script.txt`**

**【团队黄金规则】**

> **如果你想在游戏里显示任何新的汉字（UI、对话、Boss战），你必须先把这个汉字写进 `res/data/dialogue/all_game_script.txt` 文件里。**

  * **如果你不这么做**：你新加的台词（比如“你太弱了！”）中的“太”和“弱”**不在**“烘焙清单”上，`LoadFontEx` 就**没有“烘焙”它们**。
  * **后果**：游戏运行时，`DrawTextEx` 会在“烤盘”上找不到“太”和“弱”，它们将被显示为 `?` 或方框。

## 5\. 资源依赖

本模块的正常运行，**依赖**于 Git 仓库中的以下文件：

1.  **`res/data/fonts/SourceHanSansSC-Regular.otf`**

      * (或 `simhei.ttf`，取决于你们 `FontLoader.cpp` 里的 `fontFile_Address` 变量)
      * **职责：** 这是“字体菜谱”。

2.  **`res/data/dialogue/all_game_script.txt`**

      * **职责：** 这是“烘焙清单”。

**请确保这两个文件都已正确添加到 Git 仓库中并已提交！**