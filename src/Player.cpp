#include "GameState.h"  // 先包含 GameState.h，它会包含 Player.h
#include "raylib.h"
#include "Map.h"//TODO 需等待map完成
#include "raymath.h"
#include <algorithm>

const int TILE_SIZE = 32;//标准图块大小
const char* playerSpriteAddress = "res/graphics/player/Pixel_Taffy/Pixel_Taffy_Sprite.png";//玩家精灵位置


//辅助函数，用于比较两个实体的y坐标
bool CompareEntities(const RenderEntity& a, const RenderEntity& b){
    return a.sortY < b.sortY;
}

















/**
 * @brief (P1) 加载玩家所需的美术资源 (贴图)
 * (这个函数必须在 main() 的"初始化"中被调用)
 * @param ctx 
 */
void LoadPlayerAssets(GameContext& ctx){
    // 从 build 目录运行，优先使用 ../ 访问项目根目录
    const char* possiblePaths[] = {
        "../res/graphics/player/Pixel_Taffy/Pixel_Taffy_Sprite_v3_XIXI.png",
        "res/graphics/player/Pixel_Taffy/Pixel_Taffy_Sprite_v3_XIXI.png",
        "../../res/graphics/player/Pixel_Taffy/Pixel_Taffy_Sprite_v3_XIXI.png"
    };
    
    bool loaded = false;
    for (int i = 0; i < 3; i++) {
        ctx.player.spriteSheet = LoadTexture(possiblePaths[i]);
        if(ctx.player.spriteSheet.id != 0){
            TraceLog(LOG_INFO, "[Player] 玩家精灵图加载成功: %s (ID: %d, 尺寸: %dx%d)", 
                     possiblePaths[i], 
                     ctx.player.spriteSheet.id,
                     ctx.player.spriteSheet.width,
                     ctx.player.spriteSheet.height);
            loaded = true;
            break;
        }
    }
    
    if(!loaded){
        TraceLog(LOG_ERROR, "[Player] 玩家精灵图加载失败，尝试了所有路径");
        TraceLog(LOG_ERROR, "[Player] 当前工作目录: %s", GetWorkingDirectory());
        TraceLog(LOG_ERROR, "[Player] 将使用红色方块作为占位符");
    }


    //------开始加载滤镜------
    TraceLog(LOG_INFO, "[Shader] 开始加载滤镜，屏幕尺寸: %.0fx%.0f", ctx.screenWidth, ctx.screenHeight);
    
    // 检查屏幕尺寸是否有效
    if (ctx.screenWidth <= 0 || ctx.screenHeight <= 0) {
        TraceLog(LOG_WARNING, "[Shader] 屏幕尺寸无效 (%.0fx%.0f)，跳过滤镜初始化", 
                 ctx.screenWidth, ctx.screenHeight);
        ctx.mapRenderTexture.id = 0;
        ctx.cyberpunkShader.id = 0;
        return; // 提前返回，不加载滤镜
    }
    
    // 1. 创建渲染纹理（用于后处理）
    int width = (int)ctx.screenWidth;
    int height = (int)ctx.screenHeight;
    
    TraceLog(LOG_INFO, "[Shader] 转换后的整数尺寸: %dx%d", width, height);
    
    ctx.mapRenderTexture = LoadRenderTexture(width, height);
    if(ctx.mapRenderTexture.id != 0){
        TraceLog(LOG_INFO, "[Shader] 渲染纹理创建成功 (尺寸: %dx%d, ID: %d)", 
                 width, height, ctx.mapRenderTexture.id);
    } else {
        TraceLog(LOG_ERROR, "[Shader] 渲染纹理创建失败！");
    }

    // 2. 加载"着色器" (Shader) - 从 build 目录访问
    const char* shaderPaths[] = {
        "../res/graphics/shaders/cyberpunk.fs",
        "res/graphics/shaders/cyberpunk.fs",
        "../../res/graphics/shaders/cyberpunk.fs",
        "Code_LLW_Project/res/graphics/shaders/cyberpunk.fs",
        "./res/graphics/shaders/cyberpunk.fs"
    };
    
    bool shaderLoaded = false;
    for (int i = 0; i < 5; i++) {
        ctx.cyberpunkShader = LoadShader(0, shaderPaths[i]);
        if(ctx.cyberpunkShader.id != 0){
            TraceLog(LOG_INFO, "[Shader] 赛博朋克滤镜加载成功: %s (ID: %d)", 
                     shaderPaths[i], ctx.cyberpunkShader.id);
            shaderLoaded = true;
            break;
        }
    }
    
    if(!shaderLoaded){
        TraceLog(LOG_ERROR, "[Shader] 赛博朋克滤镜加载失败，尝试了所有路径");
        TraceLog(LOG_ERROR, "[Shader] 当前工作目录: %s", GetWorkingDirectory());
        TraceLog(LOG_ERROR, "[Shader] 将使用默认渲染（无滤镜效果）");
    }
    //------滤镜加载结束

}







/**
 * @brief (P1) 卸载玩家的美术资源
 * (这个函数必须在 main() 的“清理”中被调用)
 * @param ctx 
 */
void UnloadPlayerAssets(GameContext& ctx){
    if(ctx.player.spriteSheet.id != 0){
        UnloadTexture(ctx.player.spriteSheet);
    }
    else{
        TraceLog(LOG_ERROR,"[Player Texture Unloader] Trying to unload an empty texture!");
    }
    //卸载"全局"滤镜
    if(ctx.cyberpunkShader.id != 0){
        UnloadShader(ctx.cyberpunkShader);
        TraceLog(LOG_INFO, "[Shader] 赛博朋克滤镜已卸载");
    }
    if(ctx.mapRenderTexture.id != 0){
        UnloadRenderTexture(ctx.mapRenderTexture);
        TraceLog(LOG_INFO, "[Shader] 渲染纹理已卸载");
    }
}











/**
 * @brief 用于读取键盘输入来更新玩家位置 (后按键优先逻辑)
 * 
 * @param ctx Gamestate里的GameContext核心数据文件
 */
void updatePlayer(GameContext& ctx){

    //------ 1. 实时处理输入栈 (无论是否正在移动都要处理) ------
    
    // 我们关心的四个方向键
    int directionKeys[] = { KEY_W, KEY_S, KEY_A, KEY_D };

    for (int key : directionKeys) {
        // 如果刚刚按下了某个键
        if (IsKeyPressed(key)) {
            // 先尝试从栈中移除它（防止重复），确保它被放到最末尾
            for (int i = 0; i < ctx.player.inputStack.size(); ) {
                if (ctx.player.inputStack[i] == key) {
                    ctx.player.inputStack.erase(ctx.player.inputStack.begin() + i);
                } else {
                    i++;
                }
            }
            // 把新按下的键推入栈顶（优先级最高）
            ctx.player.inputStack.push_back(key);
        }

        // 如果松开了某个键
        if (IsKeyReleased(key)) {
            // 从栈中移除
            for (int i = 0; i < ctx.player.inputStack.size(); ) {
                if (ctx.player.inputStack[i] == key) {
                    ctx.player.inputStack.erase(ctx.player.inputStack.begin() + i);
                } else {
                    i++;
                }
            }
        }
    }

    // 【安全检查】清理那些实际上没有被按住的键
    // (防止因窗口切换等原因导致的“卡键”现象)
    for (int i = 0; i < ctx.player.inputStack.size(); ) {
        if (!IsKeyDown(ctx.player.inputStack[i])) {
            ctx.player.inputStack.erase(ctx.player.inputStack.begin() + i);
        } else {
            i++;
        }
    }

    //------ 2. 开始移动状态更新 ------

    float dt = GetFrameTime(); 
    float moveSpeed = ctx.player.moveSpeed;
    
    // 处理平滑移动动画
    if (ctx.player.isMoving)
    {
        // “平滑”地“追”向“目标”
        ctx.player.visualPosition = Vector2MoveTowards(
            ctx.player.visualPosition, // 当前视觉位置
            ctx.player.moveTarget,     // 目标像素位置
            ctx.player.moveSpeed * dt  // 这一帧移动距离
        );

        // 检查是否到达
        if (Vector2Distance(ctx.player.visualPosition, ctx.player.moveTarget) < 1.0f)
        {
            ctx.player.isMoving = false;
            ctx.player.visualPosition = ctx.player.moveTarget; // 校准
            ctx.player.gridX = (int)(ctx.player.moveTarget.x / TILE_SIZE);
            ctx.player.gridY = (int)(ctx.player.moveTarget.y / TILE_SIZE);
        }
    }
    // 如果静止，检查输入栈来决定下一步动作
    else 
    {
        // 只有当输入栈不为空时才处理移动
        if (!ctx.player.inputStack.empty()) {
            
            // 取出栈顶的键（最后按下的那个键）
            int activeKey = ctx.player.inputStack.back();

            int nextX = ctx.player.gridX;
            int nextY = ctx.player.gridY;

            // 根据这个“最高优先级”的键来决定方向
            if (activeKey == KEY_W) { 
                nextY--; 
                ctx.player.currentDirection = PLAYER_DIR_UP; 
            }
            else if (activeKey == KEY_S) { 
                nextY++; 
                ctx.player.currentDirection = PLAYER_DIR_DOWN; 
            }
            else if (activeKey == KEY_A) { 
                nextX--; 
                ctx.player.currentDirection = PLAYER_DIR_LEFT; 
            }
            else if (activeKey == KEY_D) { 
                nextX++; 
                ctx.player.currentDirection = PLAYER_DIR_RIGHT; 
            }

            // 碰撞检测
            if ((nextX != ctx.player.gridX || nextY != ctx.player.gridY) &&
                (nextY >= 0 && nextY < ctx.tiles.size()) &&
                (nextX >= 0 && nextX < ctx.tiles[nextY].size()) &&
                (ctx.tiles[nextY][nextX] == EMPTY || ctx.tiles[nextY][nextX] == GRASS))
            {
                // 开始移动
                ctx.player.isMoving = true;
                ctx.player.moveTarget = { (float)nextX * TILE_SIZE, (float)nextY * TILE_SIZE };
            }
        }
    }
}



/**
 * @brief 纯粹的玩家精灵绘制函数
 * 
 * @param player 玩家数据（只读）
 * 
 * @note 该函数只负责绘制玩家精灵图，不管摄像机、滤镜等
 * @note 调用前必须已经处于 BeginMode2D() 状态
 */
void DrawPlayerSprite(const Player& player)
{
    Rectangle source = { 0.0f, 0.0f, 32.0f, 64.0f };

    switch (player.currentDirection)
    {
        case PLAYER_DIR_LEFT:
            source.x = 0.0f;
            break;
        case PLAYER_DIR_DOWN:
            source.x = 32.0f;
            break;
        case PLAYER_DIR_RIGHT:
            source.x = 64.0f;
            break;
        case PLAYER_DIR_UP:
            source.x = 96.0f;
            break;
        default:
            source.x = 32.0f;
            TraceLog(LOG_WARNING, "[Player] 检测到异常的 currentDirection 值: %d, 使用默认朝向", player.currentDirection);
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


/**
 * @brief 场景渲染总指挥
 * 
 * @param ctx Gamestate里的GameContext核心数据文件
 * 
 * @note 该函数负责：摄像机设置、滤镜应用、地图绘制、实体绘制编排
 * @note 为未来的 Y-Sorting 做准备（预留 P1 实体绘制区域）
 */
void DrawMapScene(const GameContext& ctx){
    Camera2D camera = ctx.camera;//创建一个camera的副本




    //------开始设置角色摄像机------
    camera.zoom = 3.0f;//? 为啥要加f--->因为zoom本身就是个float量，3.0这样会默认为double
    camera.offset = {ctx.screenWidth/2.0f, ctx.screenHeight/2.0f}; // 摄像机偏移量，设置为屏幕中心（屏幕宽高的一半）
    //  让摄像机跟随平滑的"视觉位置"
    camera.target = {
        ctx.player.visualPosition.x + (TILE_SIZE / 2.0f), // 使用 visualPosition.x
        ctx.player.visualPosition.y + (TILE_SIZE / 2.0f)  // 使用 visualPosition.y
    };
    //------结束角色摄像机设置------




    //------开始角色摄像机绘制------
    BeginMode2D(camera);

    // 【P1 滤镜渲染流程 - 只对地图应用滤镜】
    // 检查滤镜资源是否有效
    bool useShader = (ctx.mapRenderTexture.id != 0 && ctx.cyberpunkShader.id != 0);
    
    if(useShader) {
        // 第一步：先结束当前摄像机
        EndMode2D();
        
        // 第二步：将地图渲染到纹理（使用摄像机）
        BeginTextureMode(ctx.mapRenderTexture);
        ClearBackground(RAYWHITE);
        BeginMode2D(camera); // 在纹理中应用相同的摄像机
    }
    //=============开始地图渲染================








    // 按 2.5D 顺序绘制地图和实体
    DrawGroundLayers(ctx);
    DrawYSortLayer(ctx);
    DrawOverheadLayers(ctx);










    //=============结束地图渲染================
    if(useShader) {
        EndMode2D(); // 结束纹理中的摄像机
        EndTextureMode(); // 结束渲染到纹理
        
        // 第三步：使用着色器将地图纹理绘制到屏幕（全屏绘制）
        BeginShaderMode(ctx.cyberpunkShader);
        
        // 简单的全屏绘制 - 将纹理以屏幕尺寸1:1绘制
        DrawTexturePro(
            ctx.mapRenderTexture.texture,
            Rectangle{ 0, 0, 
                      (float)ctx.mapRenderTexture.texture.width, 
                      -(float)ctx.mapRenderTexture.texture.height },  // 源矩形（负高度=Y轴翻转）
            Rectangle{ 0, 0, ctx.screenWidth, ctx.screenHeight },     // 目标矩形（全屏）
            Vector2{ 0, 0 },     // 原点
            0.0f,                // 旋转角度
            WHITE
        );
        
        EndShaderMode();
        
        // 第四步：重新开启摄像机以绘制玩家
        BeginMode2D(camera);
    }

    // ============开始玩家和敌人绘制===============
    

    //1.创建渲染列表
    vector<RenderEntity> renderList;

    //2.将玩家加入渲染列表
    renderList.push_back({ENTITY_PLAYER, (void*)&ctx.player, ctx.player.visualPosition.y + 64.0f});

    //3.将所有敌人加入渲染列表
    for(const auto& enemy: ctx.enemies){
        if(enemy.isActive){
            renderList.push_back({ENTITY_ENEMY, (void*)&enemy, enemy.visualPosition.y +64.0f});
        }
    }
    

    //4.开始排序

    sort(renderList.begin(),renderList.end(),[](const RenderEntity& a, const RenderEntity& b){
        return a.sortY < b.sortY;
    });
    
    
    //5.开始绘制
    for(const auto& item: renderList){
        if(item.type == ENTITY_PLAYER){
            const Player* p = (const Player*)item.data;
            DrawPlayerSprite(*p);
        }
        else if(item.type == ENTITY_ENEMY){
            const Enemy* e = (const Enemy*)item.data;
            DrawEnemySprite(ctx, *e);
        }

    }
    
    
    
    


    
    
    
    
    // ============结束玩家和敌人绘制===============
    EndMode2D();
    //------结束角色摄像机绘制------


}