# 玩家行走动画说明

本文档介绍玩家角色行走动画系统的工作原理，以及如何准备和调整配套资源。

## 功能概览

玩家在移动时会播放多帧行走循环。程序会检查加载到的精灵表是否按照“每行一个方向、每列一帧”的形式组织，并在行进过程中切换帧；如果贴图只有单帧，则自动退回旧版的静态渲染逻辑。

## 精灵表布局

目前支持两种排布方式：

1. **按行划分方向（推荐）**
   - 每一行对应一个朝向。
   - 行内每一列是该朝向的动画帧。
   - 读取阶段会根据帧宽/帧高从贴图尺寸推断出网格。

2. **四列静态贴图（兼容旧资源）**
   - 单行放置四个方向：左、下、右、上。
   - 仅根据方向切换静态帧，不播放走路循环。

### 默认帧尺寸

- 帧宽：32 像素。
- 帧高：64 像素（保证角色脚底落在地砖上）。
- 如果美术资源尺寸不同，可在 `LoadPlayerAssets` 中修改默认值。

## 运行时参数（`Player` 结构体）

| 字段 | 作用 |
| --- | --- |
| `frameWidth`, `frameHeight` | 定义从精灵表裁切出的单帧网格尺寸。 |
| `framesPerDirection` | 每个朝向拥有的帧数，根据贴图宽度自动计算。 |
| `animationRowCount` | 精灵表可用的方向行数，根据贴图高度自动计算。 |
| `usesRowBasedSheet` | 是否启用按行动画模式；否则走旧版静态渲染。 |
| `currentFrame` | 当前播放的帧索引。 |
| `animationTimer` | 累积时间，用于判断何时切换到下一帧。 |
| `frameDuration` | 每帧显示时长，默认约 0.167 秒（≈ 6 FPS）。 |

## 播放流程

1. `LoadPlayerAssets` 中初始化动画字段，并检测贴图是否含有多行多列。
2. `updatePlayer` 负责
   - 维持方向、更新位置；
   - 积累帧计时，时间超过 `frameDuration` 即推进帧索引，循环播放；
   - 当玩家停止移动时重置为第 0 帧，保证静止姿势一致。
3. `DrawPlayerSprite` 根据模式选择裁剪矩形：
   - 行动画模式：先按方向选行，再根据 `currentFrame` 选列；
   - 静态模式：沿用旧版的四列切换逻辑。

## 美术资源准备

1. 将 PNG 单帧素材合成一张精灵表，推荐顺序：
   - 第 0 行：面向下的帧 0..N；
   - 第 1 行：面向左；
   - 第 2 行：面向右；
   - 第 3 行：面向上。
2. 把贴图放在 `res/graphics/player/Pixel_Taffy/Pixel_Taffy_Sprite_v3_XIXI.png`（或相应地修改 `LoadPlayerAssets` 路径）。
3. 确认贴图尺寸恰好等于 `frameWidth * framesPerDirection` × `frameHeight * animationRowCount`。

## 参数调节

- 调整 `ctx.player.frameDuration` 可加快或放慢走路速度。默认值约 0.167 秒对应约 6 FPS。
- 如果只准备了一行但包含多个帧，只要 `framesPerDirection > 1` 就会自动播放同一行的循环，并对所有方向复用它。
- 如需适配其他尺寸的角色，先在 `LoadPlayerAssets` 中修改 `frameWidth` 与 `frameHeight`，再让自动推断重新计算行列。

## 代码实现详细流程

### 数据结构（`Player.h`）

```cpp
struct Player {
   // ...
   Texture2D spriteSheet;
   int frameWidth;
   int frameHeight;
   int framesPerDirection;
   int animationRowCount;
   int currentFrame;
   float animationTimer;
   float frameDuration;
   bool usesRowBasedSheet;
};
```

- `frameWidth`/`frameHeight`：决定 `DrawTextureRec` 时的裁剪窗口。
- `framesPerDirection`、`animationRowCount`：由贴图尺寸除以单帧尺寸得出，用于推断是否存在多帧动画。
- `usesRowBasedSheet`：当每个方向的帧数大于 1 时设为 `true`，即使只有一行也会启用动画。

### 载入阶段（`LoadPlayerAssets`）

关键步骤：

```cpp
ctx.player.frameWidth = TILE_SIZE;
ctx.player.frameHeight = TILE_SIZE * 2;
ctx.player.framesPerDirection = sprite.width / frameWidth;
ctx.player.animationRowCount = sprite.height / frameHeight;
ctx.player.usesRowBasedSheet = (framesPerDirection > 1);
```

1. 默认以 32×64 网格划分贴图（可根据资源调整）。
2. 用贴图实际宽高除以单帧尺寸，推断列数/行数。
3. 若判定为行式动画，则初始化 `currentFrame = 0`、`animationTimer = 0`，为后续运行做准备。

### 更新阶段（`updatePlayer`）

流程拆解：

1. 维护输入栈，确定 `ctx.player.currentDirection` 与 `isMoving` 状态。
2. 记录 `wasMoving` 与旧方向，用于判断是否需要重置帧。
3. 当 `usesRowBasedSheet` 为真且 `framesPerDirection > 1` 时：
   ```cpp
   ctx.player.animationTimer += dt;
   if (animationTimer >= frameDuration) {
      animationTimer -= frameDuration;
      currentFrame = (currentFrame + 1) % framesPerDirection;
   }
   ```
4. 若停止移动且彻底松开方向键（输入栈为空），才会将动画重置为第 0 帧；保持按键连续前进时计时器会延续，避免低帧率节奏无法触发动画。

### 绘制阶段（`DrawPlayerSprite`）

裁剪逻辑分两种：

```cpp
if (player.usesRowBasedSheet) {
   source.x = frameIndex * frameWidth;
   source.y = rowIndex * frameHeight; // rowIndex 根据方向映射
} else {
   source.x = directionIndex * frameWidth; // 旧版单行四帧
}
```

- 行动画模式下通过 `frameIndex` 选列，通过方向映射选行。映射表默认：
  - 下=0 行，左=1 行，右=2 行，上=3 行（不足行数会夹在合法范围内）。
- 旧版模式仍使用横向 4 格的布局，便于渐进迁移旧资源。
- 最终的垂直绘制位置为 `visualPosition.y - (frameHeight - TILE_SIZE)`，确保脚底贴地。

### 调试要点

- 可在贴图加载后打印 `framesPerDirection`、`animationRowCount` 等字段验证推断结果。
- 若希望强制启用旧逻辑，只需将 `usesRowBasedSheet` 设为 `false`。
- 若帧间隔不均匀，可自定义每个方向的节奏：在 `updatePlayer` 中按方向调整 `frameDuration` 或设立独立参数。

## 常见问题排查

- **没有动画**：确认加载后 `framesPerDirection > 1` 且 `usesRowBasedSheet` 为 true。
- **角色悬浮或下沉**：调整 `frameHeight` 或 `DrawPlayerSprite` 中的垂直偏移，使脚站在地面上。
- **方向混乱**：重新排列精灵表的行顺序，或修改 `DrawPlayerSprite` 里方向到行号的映射。
