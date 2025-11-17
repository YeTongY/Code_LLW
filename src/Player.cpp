#include "Player.h"
#include "raylib.h"
#include "GameState.h"
#include "Map.h"//TODO 需等待map完成
#include "raymath.h"

const int TILE_SIZE = 32;//标准图块大小
const char* playerSpriteAddress = "res/graphics/player/Pixel_Taffy/Pixel_Taffy_Sprite.png";//玩家精灵位置

/**
 * @brief (P1) 加载玩家所需的美术资源 (贴图)
 * (这个函数必须在 main() 的"初始化"中被调用)
 * @param ctx 
 */
void LoadPlayerAssets(GameContext& ctx){
    // 尝试多个可能的路径（支持从build目录或项目根目录运行）
    const char* possiblePaths[] = {
        "res/graphics/player/Pixel_Taffy/Pixel_Taffy_Sprite.png",
        "../res/graphics/player/Pixel_Taffy/Pixel_Taffy_Sprite.png",
        "../../res/graphics/player/Pixel_Taffy/Pixel_Taffy_Sprite.png"
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

    // 2. 加载"着色器" (Shader) - 尝试多个路径
    const char* shaderPaths[] = {
        "res/graphics/shaders/cyberpunk.fs",        // 从项目根目录运行
        "../res/graphics/shaders/cyberpunk.fs",     // 从 build 目录运行
        "../../res/graphics/shaders/cyberpunk.fs",  // 从更深的子目录运行
        "Code_LLW_Project/res/graphics/shaders/cyberpunk.fs",  // 从父目录运行
        "./res/graphics/shaders/cyberpunk.fs"       // 当前目录
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
  * @brief 用于读取键盘输入来更新玩家位置
  * 
  * @param ctx Gamestate里的GameContext核心数据文件
  */
void updatePlayer(GameContext& ctx){


    //------开始移动状态更新------

    
    Vector2 input = { 0.0f, 0.0f }; // (0, 0) = 不动

    float dt = GetFrameTime(); // 获取帧间时间
    float moveSpeed = ctx.player.moveSpeed; // 移动速度像素/秒
    Vector2 desiredMove = Vector2Scale(input, moveSpeed * dt);// (速度 * 时间 = 这一帧应该移动的“距离”)


    //初始化移动意图
    int nextX = ctx.player.gridX;
    int nextY = ctx.player.gridY;

    //将按键转化为移动意图
    if(IsKeyPressed(KEY_W)){//当W键按下 一次 时，仅一次向上移动一格
        nextY--;//*注意，向上是减少Y坐标
        ctx.player.currentDirection = PLAYER_DIR_UP;
    }
    else if(IsKeyPressed(KEY_S)){//当S键按下 一次 时，仅一次向下移动一格
        nextY++;//*注意，向下是增加Y坐标
        ctx.player.currentDirection = PLAYER_DIR_DOWN;
    }
    else if(IsKeyPressed(KEY_A)){//当A键按下 一次 时，仅一次向左移动一格
        nextX--;
        ctx.player.currentDirection = PLAYER_DIR_LEFT;
    }
    else if(IsKeyPressed(KEY_D)){//当D键按下 一次 时，仅一次向右移动一格
        nextX++;
        ctx.player.currentDirection = PLAYER_DIR_RIGHT;
    }

    //------开始进行碰撞检测------

    bool canMove = false; //初始化为：不可移动

    //【修复】安全检查顺序：先检查 Y，再用该行的实际宽度检查 X
    //这样可以处理"锯齿状"地图（不同行宽度不同的情况）

    //检查地图是否为空
    if(ctx.tiles.empty()){
        TraceLog(LOG_FATAL,"[PlayerMove] Map data is empty!");
        canMove = false;
    }
    //【第一步】检测 Y 轴是否超出地图限制
    else if(nextY < 0 || nextY >= (int)ctx.tiles.size()){
        TraceLog(LOG_WARNING,"[PlayerMove] Invalid move attempt! Out of map bounds of Y. Target position:(%d,%d)",nextX,nextY);
        canMove = false;
    }
    //【第二步】Y 轴安全，现在用 nextY 这一行来检查 X 的实际宽度
    else if(ctx.tiles[nextY].empty()){
        TraceLog(LOG_WARNING,"[PlayerMove] Invalid move attempt! Row %d is empty. Target position:(%d,%d)",nextY,nextX,nextY);
        canMove = false;
    }
    else if(nextX < 0 || nextX >= (int)ctx.tiles[nextY].size()){
        TraceLog(LOG_WARNING,"[PlayerMove] Invalid move attempt! Out of map bounds of X. Target position:(%d,%d)",nextX,nextY);
        canMove = false;
    }
    //【第三步】X 和 Y 都安全，检测目标位置是否可移动
    else if(ctx.tiles[nextY][nextX] != EMPTY && ctx.tiles[nextY][nextX] != GRASS){
        TraceLog(LOG_WARNING,"[PlayerMove] Invalid move attempt! Target tile is not walkable. Target position:(%d,%d)",nextX,nextY);
        canMove = false;
    }
    else{
        //所有检查通过，可以移动
        canMove = true;
    }
    //------碰撞检测结束------

    //------开始执行移动------
    if(canMove){
    ctx.player.gridX = nextX;
    ctx.player.gridY = nextY;
    }
    //------移动结束------

    //------移动状态更新结束------


    //------开始战斗状态更新------
    //? 战斗状态进入应该写在这吗？

    if(IsKeyPressed(KEY_E)){
        //TODO 在这里进入战斗状态
    }

    //------结束战斗状态更新------
}


/**
 * @brief 角色摄像机，负责绘制非战斗情况，即大地图时的画面
 * 
 * @param ctx Gamestate里的GameContext核心数据文件
 */
void drawPlayer(const GameContext& ctx){
    Camera2D camera = ctx.camera;//创建一个camera的副本

    //------开始设置角色摄像机------
    camera.zoom = 3.0f;//? 为啥要加f--->因为zoom本身就是个float量，3.0这样会默认为double
    camera.offset = {ctx.screenWidth/2.0f, ctx.screenHeight/2.0f}; // 摄像机偏移量，设置为屏幕中心（屏幕宽高的一半）
    camera.target = { // 摄像机目标位置，指向玩家所在瓦片的中心点
        (float)(ctx.player.gridX * TILE_SIZE) + (TILE_SIZE / 2.0f), // 玩家网格X坐标 * 瓦片大小 + 瓦片中心偏移
        (float)(ctx.player.gridY * TILE_SIZE) + (TILE_SIZE / 2.0f)  // 玩家网格Y坐标 * 瓦片大小 + 瓦片中心偏移
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

    //绘制地图（如果使用滤镜，会渲染到纹理；否则直接到屏幕）
    DrawMap(ctx);

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

    //绘制角色
    Rectangle source;//声明绘制源
    source.width = 32.0f;
    source.height = 64.0f;
    source.y = 0.0f;


    // 根据“当前方向” (已在 updatePlayer 设置)
    // 修改 source.x 来“裁剪”正确的帧
    switch (ctx.player.currentDirection)
    {
        // “合同”：第 1 帧 (x=0) 是 "Left"
        case PLAYER_DIR_LEFT:
            source.x = 0.0f;   // 0 * 32
            break;
            
        // “合同”：第 2 帧 (x=32) 是 "Down"
        case PLAYER_DIR_DOWN:
            source.x = 32.0f;  // 1 * 32
            break;                
            // “合同”：第 3 帧 (x=64) 是 "Right"
        case PLAYER_DIR_RIGHT:
            source.x = 64.0f;  // 2 * 32
            break;
        // "合同"：第 4 帧 (x=96) 是 "Up"
        case PLAYER_DIR_UP:
            source.x = 96.0f;  // 3 * 32
            break;
        
        // 【P1 安全网】防止未初始化或垃圾值导致的花屏 Bug
        default:
            source.x = 32.0f;  // 默认朝下 (Down)
            TraceLog(LOG_WARNING, "[Player] 检测到异常的 currentDirection 值: %d, 使用默认朝向", ctx.player.currentDirection);
            break;
    }

    //必须"向上偏移" 1 格 (TILE_SIZE)
    // 这样"32x64"的"脚"才能"踩"在"32x32"的 P0 格子上
    Vector2 drawDestPosition = {//仅渲染用坐标
        (float)ctx.player.gridX * TILE_SIZE,
        (float)ctx.player.gridY * TILE_SIZE - TILE_SIZE // 【【【向上偏移！】】】
    };

    // 【调试】绘制玩家精灵图（不受滤镜影响）
    if(ctx.player.spriteSheet.id != 0){
        // 纹理加载成功，绘制精灵图
        DrawTextureRec(ctx.player.spriteSheet, source, drawDestPosition, WHITE);
    } else {
        // 【备用方案】纹理未加载，绘制红色方块作为占位符
        DrawRectangle(
            ctx.player.gridX * TILE_SIZE, 
            ctx.player.gridY * TILE_SIZE - TILE_SIZE,
            32, 64, RED
        );
        DrawText("NO SPRITE", ctx.player.gridX * TILE_SIZE, ctx.player.gridY * TILE_SIZE, 10, WHITE);
    }

    //结束角色渲染，恢复到正常尺寸以备之后绘制ui
    EndMode2D();
    //------结束角色摄像机绘制------

    //TODO P1阶段时，可以在这之后绘制UI

}