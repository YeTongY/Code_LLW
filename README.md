# Code LLW (JRPG Game)

一个基于 Raylib 和 C++ 的日式 RPG 游戏项目。

## 项目简介

《LLW》是一个 3 人团队开发的 JRPG 游戏,使用 C 风格的 C++ 和 Raylib 图形库。游戏包含地图探索、回合制战斗等核心玩法。

## 开发环境要求

- **操作系统**: Windows
- **编译器**: GCC (MinGW-w64)
- **构建工具**: Make
- **图形库**: Raylib (已包含在 `lib/raylib/` 目录中)
- **版本控制**: Git

## 项目结构

```
LLW_Project/
├── .gitignore          # Git 忽略文件配置
├── README.md           # 项目说明文档(本文件)
├── GDD.md              # 游戏设计文档
├── Makefile            # 编译配置文件
├── src/                # 源代码目录
│   ├── main.cpp        # 主程序入口
│   ├── GameState.h     # 游戏状态定义
│   ├── Map.h/cpp       # 地图系统
│   ├── Combat.h/cpp    # 战斗系统
│   ├── Player.h/cpp    # 玩家角色
│   └── Enemy.h/cpp     # 敌人角色
├── res/                # 资源文件目录
│   ├── graphics/       # 图片资源
│   │   ├── player/     # 玩家角色图片
│   │   ├── enemies/    # 敌人图片
│   │   ├── tilesets/   # 地图瓦片
│   │   └── ui/         # UI 界面图片
│   ├── audio/          # 音频资源
│   │   ├── music/      # 背景音乐
│   │   └── sfx/        # 音效
│   └── data/           # 游戏数据
│       ├── maps/       # 地图数据文件
│       └── dialogue/   # 对话脚本
├── lib/                # 第三方库
│   └── raylib/         # Raylib 库文件
│       ├── include/    # 头文件
│       └── lib/        # 库文件
└── build/              # 编译输出目录(不提交到 Git)
```

---

## 如何编译项目

### 1. 克隆项目(首次使用)

```powershell
git clone https://github.com/YeTongY/Code_LLW.git
cd Code_LLW
```

### 2. 确保环境配置正确

- 确保 `make` 和 `g++` 已安装并添加到系统 PATH。
- 确保 `lib/raylib/` 文件夹中已有 Raylib 库文件。

### 3. 编译项目

在项目根目录 (`LLW_Project/`) 打开终端,运行:

```powershell
make
```

编译成功后,可执行文件会生成在 `build/LLW.exe`。

### 4. 运行游戏

```powershell
.\build\LLW.exe
```

或者直接双击 `build/LLW.exe` 文件。

### 5. 清理编译文件

如果需要清理所有编译生成的文件:

```powershell
make clean
```

---

## Git 协作规范

### 提交代码前的准备

1. **拉取最新代码** (每次开始工作前必做):
   ```powershell
   git pull
   ```

2. **检查当前状态**:
   ```powershell
   git status
   ```

### 提交代码的步骤

1. **添加修改的文件到暂存区**:
   ```powershell
   git add .
   ```
   注意: `.gitignore` 会自动忽略 `build/` 文件夹和 `.exe` 等编译产物,所以不用担心提交这些文件。

2. **提交代码并写清楚说明**:
   ```powershell
   git commit -m "类型: 简短描述你做了什么"
   ```

3. **推送到远程仓库**:
   ```powershell
   git push
   ```

### 提交信息规范

为了让团队成员清楚地了解每次提交的内容,请遵循以下提交信息格式:

**格式**: `类型: 简短描述`

**常用类型**:
- `feat`: 新功能 (feature)
  - 例: `feat: 实现玩家移动功能`
  - 例: `feat: 添加战斗系统基础框架`
  
- `fix`: 修复 Bug
  - 例: `fix: 修复玩家移动时的碰撞检测问题`
  - 例: `fix: 修复战斗结束后程序崩溃的 bug`
  
- `docs`: 文档更新
  - 例: `docs: 更新 GDD.md 中的角色设定`
  - 例: `docs: 完善 README.md 编译说明`
  
- `style`: 代码格式调整(不影响功能)
  - 例: `style: 统一代码缩进格式`
  
- `refactor`: 代码重构(不改变功能)
  - 例: `refactor: 重构地图加载函数`
  
- `test`: 添加测试代码
  - 例: `test: 添加战斗伤害计算测试`
  
- `chore`: 构建/工具相关的修改
  - 例: `chore: 更新 Makefile 编译参数`

### 不要提交的文件

以下文件已经在 `.gitignore` 中配置,**不应该**提交到 Git:
- `build/` 文件夹及其所有内容
- `*.exe` 可执行文件
- `*.o` 目标文件
- `.vs/` Visual Studio 缓存文件

### 解决冲突

如果 `git push` 时提示冲突:

1. 先拉取最新代码:
   ```powershell
   git pull
   ```

2. Git 会提示哪些文件有冲突,打开这些文件手动解决冲突。

3. 解决冲突后,重新提交:
   ```powershell
   git add .
   git commit -m "fix: 解决合并冲突"
   git push
   ```

---

## 日常工作流程

1. **开始工作前**:
   ```powershell
   git pull  # 拉取队友的最新代码
   ```

2. **编写代码**:
   - 在 `src/` 目录中编写代码
   - 将资源文件放到 `res/` 对应的子目录中
   - 使用相对路径加载资源: `../res/graphics/player/xxx.png`

3. **测试编译**:
   ```powershell
   make
   .\build\LLW.exe
   ```

4. **提交代码**:
   ```powershell
   git add .
   git commit -m "feat: 你的功能描述"
   git push
   ```

---

## 常见问题

### Q: 编译时提示找不到 raylib.h?
A: 确保 `lib/raylib/include/` 目录中有 `raylib.h` 文件。

### Q: 链接时提示找不到 libraylib.a?
A: 确保 `lib/raylib/lib/` 目录中有 `libraylib.a` 文件(MinGW) 或 `raylib.lib`(MSVC)。

### Q: 运行时提示找不到图片文件?
A: 检查你的工作目录是否在 `build/` 文件夹中。资源文件路径应该使用 `../res/` 开头。

### Q: git push 失败?
A: 先运行 `git pull` 拉取最新代码,解决可能的冲突后再 push。

---

## 团队成员

- [在这里添加团队成员信息]

## 许可证

[在这里添加许可证信息]
