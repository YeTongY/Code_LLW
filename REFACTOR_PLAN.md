# GDD v3.13 重构计划 - 两步走方案

## 目标
将 `drawPlayer` 函数重构为清晰的架构：
- `DrawPlayerSprite(const Player&)` - 纯粹的玩家绘制函数
- `DrawMapScene(const GameContext&)` - 场景渲染总指挥

## 步骤

### 1. 修改 Player.h
在文件末尾的函数声明部分：

```cpp
// 删除：
void drawPlayer(const GameContext& ctx);

// 添加：
void DrawPlayerSprite(const Player& player);
void DrawMapScene(const GameContext& ctx);
```

### 2. 修改 Player.cpp

#### 2.1 在 `updatePlayer` 函数之后添加新函数 `DrawPlayerSprite`：

```cpp
void DrawPlayerSprite(const Player& player)
{
    Rectangle source = { 0.0f, 0.0f, 32.0f, 64.0f };

    switch (player.currentDirection)
    {
        case PLAYER_DIR_LEFT:  source.x = 0.0f;   break;
        case PLAYER_DIR_DOWN:  source.x = 32.0f;  break;
        case PLAYER_DIR_RIGHT: source.x = 64.0f;  break;
        case PLAYER_DIR_UP:    source.x = 96.0f;  break;
        default:
            source.x = 32.0f;
            TraceLog(LOG_WARNING, "[Player] Abnormal currentDirection: %d", player.currentDirection);
            break;
    }

    Vector2 drawDestPosition = {
        player.visualPosition.x,
        player.visualPosition.y - TILE_SIZE
    };

    if (player.spriteSheet.id != 0) {
        DrawTextureRec(player.spriteSheet, source, drawDestPosition, WHITE);
    } else {
        DrawRectangle((int)drawDestPosition.x, (int)drawDestPosition.y, 32, 64, RED);
        DrawText("NO SPRITE", (int)player.visualPosition.x, (int)player.visualPosition.y, 10, WHITE);
    }
}
```

#### 2.2 将 `drawPlayer` 重命名为 `DrawMapScene`

找到函数签名：
```cpp
void drawPlayer(const GameContext& ctx){
```

改为：
```cpp
void DrawMapScene(const GameContext& ctx){
```

#### 2.3 在 `DrawMapScene` 的最后，找到绘制角色的代码块（从 `//绘制角色` 到 `EndMode2D()`）

替换整个代码块为：
```cpp
    // P1 实体绘制区域
    DrawPlayerSprite(ctx.player);
    
    // TODO: 未来敌人绘制

    EndMode2D();
}
```

### 3. 修改 Exploration.cpp

找到：
```cpp
drawPlayer(*ctx);
```

改为：
```cpp
DrawMapScene(*ctx);
```

## 完成后的架构

```
DrawMapScene (总指挥)
├── 设置摄像机
├── 应用滤镜（可选）
├── 绘制地图
└── 绘制实体
    ├── DrawPlayerSprite(player)
    └── [未来] DrawEnemySprite(enemy)
```

## 优势
1. 职责清晰：每个函数只做一件事
2. 易于扩展：添加敌人只需在DrawMapScene中调用DrawEnemySprite
3. 为Y-Sorting做准备：未来只需在实体绘制区域添加排序逻辑
