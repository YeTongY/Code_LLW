#include "GameState.h"
#include "Map.h"

#include "tmxlite/Map.hpp"
#include "tmxlite/Layer.hpp"
#include "tmxlite/TileLayer.hpp"
#include "tmxlite/ObjectGroup.hpp"
#include "tmxlite/Property.hpp"

#include <iostream>
#include <fstream>      // 文件操作核心库
#include <vector>       // 用vector存储地图的行（动态数组，比普通数组灵活）
#include <string>       // 存储每行的字符串
#include <set>          // 用set避免重复卸载纹理
#include <functional>   // 用于存储可调用对象
#include <algorithm>    // 用于排序渲染项
using namespace std;

/**
 * @brief 清理地图资源，卸载地图纹理，防止内存溢出
 * 
 * @param map 地图指针
 */
void CleanupMap(GameContext& map) {
    // 避免对同一 GPU 纹理 ID 重复调用 UnloadTexture
    std::set<unsigned int> unloadedIds;
    for (auto const& [key, val] : map.mapTextures) {
        if (unloadedIds.find(val.id) == unloadedIds.end()) {
            UnloadTexture(val);
            unloadedIds.insert(val.id);
        }
    }
    map.tiles.clear();
    map.mapTextures.clear();
}

/**
 * @brief 绘制地图
 * 
 * @param map 地图指针
 */
namespace {

enum class TiledLayerType {
    Ground,
    YSort,
    Overhead,
    Ignored,
    decorationLayers
};//辅助函数，根据图层名称分类



// 根据图层名称分类
TiledLayerType ClassifyLayer(const std::string& name)
{
    if(name == "Ground" || name == "ground" || name == "Background" || name == "background"){
        return TiledLayerType::Ground;
        //地面层
    }
    if(name == "Overhead" || name == "overhead" || name == "Foreground" || name == "foreground"){
        return TiledLayerType::Overhead;
        //天花板层
    }
    if(name == "Collision" || name == "collision" || name == "Tree" || name == "Enemy" || name == "NPC" || name == "Objects" || name == "Wall"){
        return TiledLayerType::YSort;
        //墙壁层
    }
    if(name == "Decoration" || name == "decoration" || name == "WallDecor"){
        return TiledLayerType::decorationLayers;
    }
    return TiledLayerType::Ignored;
}

//  辅助结构体，用于存储可排序的渲染项
struct SortedRenderable {
    float sortKey;
    std::function<void()> draw;
};

// 绘制图层列表的辅助函数
void DrawLayerList(const GameContext& map, const vector<vector<vector<unsigned int>>>& layers)
{
    for(const auto& layerGIDs : layers){
        for(int y = 0;y < map.height;y++){
            for(int x = 0;x < map.width;x++){
                unsigned int gid = layerGIDs[y][x];
                if(gid != 0){
                    DrawSingleTile(map, x, y, gid);
                }
            }
        }
    }
}

}

//绘制地板层
void DrawGroundLayers(const GameContext& map)
{
    DrawLayerList(map, map.groundLayers);
}

//绘制Y排序层
void DrawYSortLayer(const GameContext& map)
{
    vector<SortedRenderable> sortedDrawables;

    // 处理普通的物体
    for(const auto& layerGIDs : map.tileGIDs){
        for(int y = 0;y < map.height;y++){
            for(int x = 0;x < map.width;x++){
                unsigned int gid = layerGIDs[y][x];
                if(gid == 0) continue;
                //排序基准为脚底
                float sortY = (float)((y + 1) * map.tileSize);
                sortedDrawables.push_back({ sortY, [=, &map]() {
                    DrawSingleTile(map, x, y, gid);
                }});
            }
        }
    }

    //处理挂墙物体或者贴墙物体
    for(const auto& layerGIDs : map.decorationLayers){
        for(int y = 0;y < map.height;y++){
            for(int x = 0;x < map.width;x++){
                unsigned int gid = layerGIDs[y][x];
                if(gid == 0) continue;
                //排序基准为脚底
                // 贴墙物体的脚底位置需要上移半个tileSize
                // 这样它们才能正确地覆盖在玩家和敌人之上
                // 假设贴墙物体高度为半个tileSize
                float sortY = (float)((y + 1) * map.tileSize) + (map.tileSize * 0.5f); 
                sortedDrawables.push_back({ sortY, [=, &map]() {
                    DrawSingleTile(map, x, y, gid);
                }});
            }
        }
    }

    const GameContext* ctxPtr = &map;
    sortedDrawables.push_back({ map.player.visualPosition.y + map.tileSize, [ctxPtr]() {
        DrawPlayerSprite(ctxPtr->player);
    }});

    for(const Enemy& enemy : map.enemies){
        if(!enemy.isActive) continue;
        const Enemy* enemyPtr = &enemy;
        sortedDrawables.push_back({ enemy.visualPosition.y + map.tileSize, [ctxPtr, enemyPtr]() {
            Rectangle source = {0,0,32,64};
            source.height = 64.0f;
            source.y = 0.0f;
            switch (enemyPtr->currentDirection){
                case ENEMY_DIR_RIGHT: source.x = 0.0f; break;
                case ENEMY_DIR_UP: source.x = 32.0f; break;
                case ENEMY_DIR_LEFT: source.x = 64.0f; break;
                case ENEMY_DIR_DOWN: default: source.x = 96.0f; break;
            }
            Vector2 drawDestPosition = {
                enemyPtr->visualPosition.x,
                enemyPtr->visualPosition.y - ctxPtr->tileSize
            };
            if(ctxPtr->enemySpriteSheet.id != 0){
                DrawTextureRec(ctxPtr->enemySpriteSheet, source, drawDestPosition, WHITE);
            } else {
                DrawRectangle((int)drawDestPosition.x, (int)drawDestPosition.y, 32, 64, BLUE);
            }
        }});
    }

    std::sort(sortedDrawables.begin(), sortedDrawables.end(), [](const SortedRenderable& a, const SortedRenderable& b){
        return a.sortKey < b.sortKey;
    });

    for(const auto& renderable : sortedDrawables){
        renderable.draw();
    }
}

//绘制前景层
void DrawOverheadLayers(const GameContext& map)
{
    DrawLayerList(map, map.overheadLayers);
}

void DrawMap(const GameContext& map) {
    if (map.width <= 0 || map.height <= 0) {
        TraceLog(LOG_WARNING, "[Map] 地图尺寸无效: %dx%d", map.width, map.height);
        return;
    }
    if (map.tiles.empty()) {
        TraceLog(LOG_WARNING, "[Map] 地图数据为空");
        return;
    }

    DrawGroundLayers(map);
    DrawYSortLayer(map);
    DrawOverheadLayers(map);
}

/**
 * @brief 绘制单个块
 * 
 * @param map 地图指针
 * @param tileX x坐标
 * @param tileY y坐标
 * @param type 地图块类型
 */
void DrawSingleTile(const GameContext& map, int tileX, int tileY,unsigned int gid) {
    // 获取该位置的 GID
    int tilesetIndex = -1;
    for(int i = static_cast<int>(map.tilesetFirstGIDs.size()) - 1;i >= 0 ;i--){
        if(gid >= static_cast<unsigned int>(map.tilesetFirstGIDs[i])){
            tilesetIndex = i;
            break;
        }
    }
    if(tilesetIndex == -1 || static_cast<size_t>(tilesetIndex) >= map.tilesetTextures.size()) return;

    // 收集本地纹理
    Texture2D tileset = map.tilesetTextures[tilesetIndex];
    unsigned int localID = gid - map.tilesetFirstGIDs[tilesetIndex];
    int columns = tileset.width / map.tileSize;
    if(columns == 0) return;

    // 计算图块在 tileset 中的位置
    int tilesetX = localID % columns;
    int tilesetY = localID / columns;

    // 定义源矩形和目标位置
    Rectangle sourceRect = {
        (float)(tilesetX * map.tileSize),
        (float)(tilesetY * map.tileSize),
        (float)map.tileSize,
        (float)map.tileSize
    };
    Vector2 destPos = {
        (float)(tileX * map.tileSize),
        (float)(tileY * map.tileSize)
    };

    DrawTextureRec(tileset, sourceRect, destPos, WHITE);

    if(tileset.id == 0) return;
}
/**
 * @brief 加载地图纹理
 * 将地图纹理放置在maps文件下，注意png格式
 * 在这个函数后添加读取地图
 * 直接复制粘贴即可，修改map.h库里面TileType中的地图类型
 * 
 * @param map 
 */
void LoadMapTextures(GameContext& map) {
    map.mapTextures.clear();
    
    // 从 build 目录运行，优先使用 ../ 路径
    const char* texturePaths[] = {
        "../res/graphics/tilesets/Godot_Autotiles_32x32.png",
        "res/graphics/tilesets/Godot_Autotiles_32x32.png",
        "../../res/graphics/tilesets/Godot_Autotiles_32x32.png"
    };
    
    Texture2D tileset = {0};
    for (int i = 0; i < 3; i++) {
        tileset = LoadTexture(texturePaths[i]);
        if (tileset.id > 0) {
            TraceLog(LOG_INFO, "[Map] 纹理加载成功，路径: %s", texturePaths[i]);
            break;
        }
    }
    
    if (tileset.id == 0) {
        TraceLog(LOG_ERROR, "[Map] 无法加载地图纹理!");
        return;
    }
    
    // 加载整个 tileset 图集，并指向到所有 TileType（复用同一 GPU 纹理）
    map.mapTextures[TileType::EMPTY] = tileset;
    map.mapTextures[TileType::GRASS] = tileset;
    map.mapTextures[TileType::WALL] = tileset;
}                                   

/**
 * @brief 加载Tiled地图
 * 
 * @param ctx 
 * @param filepath 
 * @return true 
 * @return false 
 */
bool LoadLevelFromTiled(GameContext& ctx, const char* filepath){
    TraceLog(LOG_INFO, "[Map] 尝试加载地图文件: %s", filepath);

    // 新增：确保切换地图前完全释放旧纹理
    for (Texture2D& tex : ctx.tilesetTextures) {
        if (tex.id != 0) {
            UnloadTexture(tex);
        }
    }

    ctx.tiles.clear();
    ctx.tileGIDs.clear();
    ctx.groundLayers.clear();
    ctx.overheadLayers.clear();
    ctx.enemies.clear();
    ctx.mapTextures.clear();
    ctx.tilesetTextures.clear();
    ctx.tilesetFirstGIDs.clear();
    ctx.gameEvents.clear();
    ctx.decorationLayers.clear();

    // 新增：按优先级列出可能的相对路径，兼容不同工作目录
    std::vector<std::string> pathCandidates = {
        std::string(filepath),
        std::string("../") + filepath,
        std::string("../../") + filepath
    };

    tmx::Map tiledMap;
    bool loaded = false;
    std::string resolvedPath;

    for (const auto& candidate : pathCandidates) {
        if (!FileExists(candidate.c_str())) {
            continue;
        }
        try {
            if (tiledMap.load(candidate)) {
                TraceLog(LOG_INFO, "[Map] 成功加载地图: %s", candidate.c_str());
                loaded = true;
                resolvedPath = candidate;
                break;
            }
        } catch (const std::exception& e) {
            TraceLog(LOG_WARNING, "[Map] 加载地图时发生异常 (%s): %s", candidate.c_str(), e.what());
        }
    }

    if (!loaded) {
        TraceLog(LOG_ERROR, "[Map] 无法在以下路径加载地图:");
        for (const auto& candidate : pathCandidates) {
            TraceLog(LOG_ERROR, "  - %s", candidate.c_str());
        }
        TraceLog(LOG_ERROR, "[Map] 当前工作目录: %s", GetWorkingDirectory());
        return false;
    }

    // 读取地图尺寸和图块大小
    ctx.width = tiledMap.getTileCount().x;
    ctx.height = tiledMap.getTileCount().y;
    ctx.tileSize = tiledMap.getTileSize().x; // 设置瓦片大小
    
    TraceLog(LOG_INFO, "[Map] 地图尺寸: %dx%d, 瓦片大小: %d", ctx.width, ctx.height, ctx.tileSize);
    
    // 从 Tiled 地图加载 tileset 纹理
    const auto& tilesets = tiledMap.getTilesets();
    TraceLog(LOG_INFO, "[Map] 地图包含 %zu 个 tileset", tilesets.size());
    
    for(const auto& tileset : tilesets) {
        // 记录该 tileset 的起始 GID
        ctx.tilesetFirstGIDs.push_back(tileset.getFirstGID());
        
        string imagePath = tileset.getImagePath();
        if(imagePath.empty()) {
            TraceLog(LOG_WARNING, "[Map] Tileset '%s' 没有图片路径", tileset.getName().c_str());
            ctx.tilesetTextures.push_back(Texture2D{0});
            continue;
        }
        
        // 提取文件名（去掉路径中的 graphics/tilesets/ 或其他前缀）
        size_t lastSlash = imagePath.find_last_of("/\\");
        string filename = (lastSlash != string::npos) ? imagePath.substr(lastSlash + 1) : imagePath;
        
        // 尝试多个可能的路径
        vector<string> possiblePaths = {
            string("../res/graphics/tilesets/") + filename,  // 从 build/ 运行
            string("res/graphics/tilesets/") + filename,     // 从项目根运行
            string("../res/") + imagePath,                   // .tmx 中的相对路径 (从 build/)
            string("res/") + imagePath,                      // .tmx 中的相对路径 (从根目录)
        };
        
        Texture2D texture = {0};
        bool loaded = false;
        for (const auto& path : possiblePaths) {
            texture = LoadTexture(path.c_str());
            if (texture.id != 0) {
                TraceLog(LOG_INFO, "[Map] 成功加载 tileset '%s': %s (FirstGID: %d, 尺寸: %dx%d)", 
                         tileset.getName().c_str(), path.c_str(), 
                         tileset.getFirstGID(), texture.width, texture.height);
                loaded = true;
                break;
            }
        }
        
        if (loaded) {
            ctx.tilesetTextures.push_back(texture);
        } else {
            TraceLog(LOG_ERROR, "[Map] 无法加载 tileset 纹理: %s (尝试的文件名: %s)", 
                     imagePath.c_str(), filename.c_str());
            ctx.tilesetTextures.push_back(Texture2D{0});
        }
    }
    
    // 初始化地图数据结构
    ctx.tiles.assign(ctx.height, vector<TileType>(ctx.width, TileType::EMPTY));

    // 读取图层数据
    const auto& layers = tiledMap.getLayers();
    for(const auto& layer : layers){
        if(layer->getType() == tmx::Layer::Type::Tile){
            const auto& tileLayer = layer->getLayerAs<tmx::TileLayer>();
            const auto& tiles = tileLayer.getTiles();

            vector<vector<unsigned int>> currentLayerGIDs(ctx.height, vector<unsigned int>(ctx.width, 0));
            TraceLog(LOG_INFO, "[Map] 处理图块图层: %s", layer->getName().c_str());

            for(int y = 0; y < ctx.height; y++){
                for(int x = 0; x < ctx.width; x++){
                    unsigned int gid = tiles[y * ctx.width + x].ID;
                    currentLayerGIDs[y][x] = gid;

                    if(gid == 0) continue;

                    string name = layer->getName();
                    if(name == "Collision" || name == "collision" || name == "Tree" || name == "Enemy" || name == "NPC" || name == "Wall"){
                        ctx.tiles[y][x] = TileType::WALL;
                    }
                    else if(name == "Ground" || name == "ground" || name == "Empty" || name == "empty" || name == "Background"){
                        ctx.tiles[y][x] = TileType::GRASS;
                    }
                    else {
                        if(ctx.tiles[y][x] != TileType::WALL){
                            ctx.tiles[y][x] = TileType::GRASS;
                        }
                    }
                }
            }

            switch (ClassifyLayer(layer->getName())){
                case TiledLayerType::Ground:
                    ctx.groundLayers.push_back(currentLayerGIDs);
                    break;
                case TiledLayerType::Overhead:
                    ctx.overheadLayers.push_back(currentLayerGIDs);
                    break;
                case TiledLayerType::YSort:
                    ctx.tileGIDs.push_back(currentLayerGIDs);
                    break;
                case TiledLayerType::decorationLayers:
                    ctx.decorationLayers.push_back(currentLayerGIDs);
                    break;
                default:
                    TraceLog(LOG_INFO, "[Map] 忽略图块图层: %s", layer->getName().c_str());
                    break;
            }
        }
    }

    // 解析对象层来创建敌人
    for(const auto& layer: layers){
        //解析敌人层
        if(layer->getType() == tmx::Layer::Type::Object && layer->getName() == "Enemies"){
            const auto& objectlayer = layer->getLayerAs<tmx::ObjectGroup>();
            // 解析敌人对象

            for(const auto& object : objectlayer.getObjects()){
                Enemy enemy = {};//声明敌人主结构体

                //读取敌人位置
                float objX = object.getPosition().x;
                float objY = object.getPosition().y;
                float objHeight = object.getAABB().height;

                //计算实际位置
                enemy.gridX = static_cast<int>(objX / ctx.tileSize);
                enemy.gridY = static_cast<int>(objY / ctx.tileSize);
                enemy.visualPosition = { objX , objY};
                enemy.patrolCenter = enemy.visualPosition;
                enemy.moveTarget = enemy.visualPosition;

                //默认移动属性
                enemy.moveSpeed = 60.0f; // 默认移动速度
                enemy.patrolRange = 100.0f; // 默认巡逻范围
                enemy.aggroRange = 150.0f; // 默认索敌范围
                enemy.stats = {100, 100, 10, 5}; // 默认属性：hp, maxHp, attack, defense

                //从tilesMap中获取敌人数据
                //严肃声明，在tilesmap里面自定义敌人数据的时候，不要修改变量名，将对象层的敌人模块复制粘贴并且仅修改属性值
                for(const auto& prop : object.getProperties()){
                    if (prop.getName() == "hp") enemy.stats.hp = prop.getIntValue();
                    if (prop.getName() == "maxHp") enemy.stats.maxHp = prop.getIntValue();
                    if (prop.getName() == "attack") enemy.stats.attack = prop.getIntValue();
                    if (prop.getName() == "defense") enemy.stats.defense = prop.getIntValue();
                    if (prop.getName() == "moveSpeed") enemy.moveSpeed = prop.getFloatValue();
                    //读取ai逻辑
                    if (prop.getName() == "patrolRange") enemy.patrolRange = prop.getFloatValue();
                    if (prop.getName() == "aggroRange") enemy.aggroRange = prop.getFloatValue();
                }

                //设置其他默认属性
                enemy.isActive = true;
                enemy.isMoving = false;
                enemy.aiState = AI_STATE_PATROL;
                enemy.currentDirection = ENEMY_DIR_DOWN;

                //添加到世界中
                ctx.enemies.push_back(enemy);
            }
        }

        //解析事件层
        if(layer->getType() == tmx::Layer::Type::Object && layer->getName() == "Events"){
            // 解析事件对象
            const auto& objecLayer = layer->getLayerAs<tmx::ObjectGroup>();

            for(const auto& object : objecLayer.getObjects()){
                GameEvent event = {};
                event.eventType = DIALOGUE; // 默认事件类型为对话
                event.triggerType = ON_NONE;
                event.isOneShot = false;

                EventData_Dialogue dialogueData = {};

                for(const auto& prop : object.getProperties()){
                    const auto propName = prop.getName();
                    const auto propType = prop.getType();

                    if (propName == "triggerType") {
                        if (propType == tmx::Property::Type::String) {
                            string triggerStr = prop.getStringValue();
                            if (triggerStr == "ON_INTERACT") event.triggerType = ON_INTERACT;
                            else if (triggerStr == "ON_ENTER_ZONE") event.triggerType = ON_ENTER_ZONE;
                            else if (triggerStr == "ON_AUTO_START") event.triggerType = ON_AUTO_START;
                            else {
                                TraceLog(LOG_WARNING, "[Map] 未知的 triggerType: %s", triggerStr.c_str());
                                event.triggerType = ON_NONE;
                            }
                        } else {
                            TraceLog(LOG_WARNING, "[Map] triggerType 属性类型不是字符串");
                        }
                    }
                    if (propName == "triggerValue") {
                        if (propType == tmx::Property::Type::String) {
                            event.triggerValue = prop.getStringValue();
                        } else {
                            TraceLog(LOG_WARNING, "[Map] triggerValue 属性类型不是字符串");
                        }
                    }
                    if (propName == "scriptPath") {
                        if (propType == tmx::Property::Type::String) {
                            event.scriptPath = prop.getStringValue();
                            dialogueData.scriptPath = event.scriptPath;
                        } else {
                            TraceLog(LOG_WARNING, "[Map] scriptPath 属性类型不是字符串");
                        }
                    }
                    if (propName == "isOneShot") {
                        if (propType == tmx::Property::Type::Boolean) {
                            event.isOneShot = prop.getBoolValue();
                        } else if (propType == tmx::Property::Type::Int || propType == tmx::Property::Type::Object) {
                            event.isOneShot = (prop.getIntValue() != 0);
                        } else if (propType == tmx::Property::Type::String) {
                            string boolStr = prop.getStringValue();
                            event.isOneShot = (boolStr == "true" || boolStr == "1");
                        } else {
                            TraceLog(LOG_WARNING, "[Map] isOneShot 属性类型不受支持");
                        }
                    }

                    //解析事件类型
                    if (propName == "eventType") {
                        if (propType == tmx::Property::Type::String) {
                            string typeStr = prop.getStringValue();
                            if(typeStr == "DIALOGUE") event.eventType = DIALOGUE;
                            else if(typeStr == "TELEPORT") event.eventType = TELEPORT;
                            else if(typeStr == "COMBAT") event.eventType = COMBAT;
                            else {
                                TraceLog(LOG_WARNING, "[Map] 未知的事件类型: %s", typeStr.c_str());
                                event.eventType = NONE;
                            }
                        } else {
                            TraceLog(LOG_WARNING, "[Map] eventType 属性类型不是字符串");
                        }
                    }
                }

                event.area = {
                    object.getPosition().x,
                    object.getPosition().y,
                    object.getAABB().width,
                    object.getAABB().height
                };

                event.dialogue.push_back(dialogueData);

                if(event.triggerType != ON_NONE && !event.scriptPath.empty()){
                    ctx.gameEvents.push_back(event);
                }
            }
        }
        //解析NPC层
        if(layer->getType() == tmx::Layer::Type::Object && layer->getName() == "NPC"){
            // 解析NPC对象层
            const auto& objectLayer = layer->getLayerAs<tmx::ObjectGroup>();

            for(const auto& object : objectLayer.getObjects()){
                (void)object; // 占位，后续将根据需求解析 NPC
            }

        }

        //解析传送门层
        if(layer->getType() == tmx::Layer::Type::Object && layer->getName() == "Portals"){
            // 解析传送门对象层
            const auto& objectLayer = layer->getLayerAs<tmx::ObjectGroup>();

            for(const auto& object : objectLayer.getObjects()){
                // 可以在这里解析传送门对象的属性并创建传送门实体
                // 目前仅作为占位符
                GameEvent newPortalEvent = {};
                newPortalEvent.eventType = TELEPORT;
                newPortalEvent.isTrigged = false;
                newPortalEvent.triggerType = ON_ENTER_ZONE;
                newPortalEvent.isOneShot = true;

                newPortalEvent.area = {
                    object.getPosition().x,
                    object.getPosition().y,
                    object.getAABB().width,
                    object.getAABB().height
                };

                EventData_Portal portal = {};

                portal.bounds = newPortalEvent.area;

                for(const auto& prop : object.getProperties()){
                    const auto propName = prop.getName();
                    const auto propType = prop.getType();

                    if( propName == "targetMap" ) {
                        if (propType == tmx::Property::Type::String) {
                            portal.targetMap = prop.getStringValue();
                        } else {
                            TraceLog(LOG_WARNING, "[Map] targetMap 属性类型不是字符串");
                        }
                    }
                    if( propName == "targetSpawnPoint") {
                        if (propType == tmx::Property::Type::String) {
                            portal.targetSpawnPoint = prop.getStringValue();
                        } else {
                            TraceLog(LOG_WARNING, "[Map] targetSpawnPoint 属性类型不是字符串");
                        }
                    }
                    if( propName == "targetPosX") {
                        if (propType == tmx::Property::Type::Float) {
                            portal.targetPosition.x = prop.getFloatValue();
                        } else if (propType == tmx::Property::Type::Int || propType == tmx::Property::Type::Object) {
                            portal.targetPosition.x = static_cast<float>(prop.getIntValue());
                        } else {
                            TraceLog(LOG_WARNING, "[Map] targetPosX 属性类型不是数值");
                        }
                    }
                    if( propName == "targetPosY") {
                        if (propType == tmx::Property::Type::Float) {
                            portal.targetPosition.y = prop.getFloatValue();
                        } else if (propType == tmx::Property::Type::Int || propType == tmx::Property::Type::Object) {
                            portal.targetPosition.y = static_cast<float>(prop.getIntValue());
                        } else {
                            TraceLog(LOG_WARNING, "[Map] targetPosY 属性类型不是数值");
                        }
                    }
                    if( propName == "triggerType") {
                        if (propType == tmx::Property::Type::String) {
                            string triggerStr = prop.getStringValue();
                            if (triggerStr == "ON_INTERACT") newPortalEvent.triggerType = ON_INTERACT;
                            else if (triggerStr == "ON_AUTO_START") newPortalEvent.triggerType = ON_AUTO_START;
                            else if (triggerStr == "ON_ENTER_ZONE") newPortalEvent.triggerType = ON_ENTER_ZONE;
                            else TraceLog(LOG_WARNING, "[Map] 未知的传送 triggerType: %s", triggerStr.c_str());
                        } else {
                            TraceLog(LOG_WARNING, "[Map] 传送 triggerType 属性类型不是字符串");
                        }
                    }
                    if (propName == "isOneShot") {
                        if (propType == tmx::Property::Type::Boolean) {
                            newPortalEvent.isOneShot = prop.getBoolValue();
                        } else if (propType == tmx::Property::Type::Int || propType == tmx::Property::Type::Object) {
                            newPortalEvent.isOneShot = (prop.getIntValue() != 0);
                        } else if (propType == tmx::Property::Type::String) {
                            string boolStr = prop.getStringValue();
                            newPortalEvent.isOneShot = (boolStr == "true" || boolStr == "1");
                        } else {
                            TraceLog(LOG_WARNING, "[Map] 传送 isOneShot 属性类型不受支持");
                        }
                    }
                }

                newPortalEvent.portal.push_back(portal);

                if(!portal.targetMap.empty()){
                    ctx.gameEvents.push_back(newPortalEvent);
                }
            }

        }
        //可以继续添加其他对象层的解析
    }

    TraceLog(LOG_INFO, "[Map] 地图加载完成");
    TraceLog(LOG_INFO, "[Map] - 地图尺寸: %dx%d", ctx.width, ctx.height);
    TraceLog(LOG_INFO, "[Map] - Tileset 数量: %zu", ctx.tilesetTextures.size());
    TraceLog(LOG_INFO, "[Map] - 敌人数量: %zu", ctx.enemies.size());
    TraceLog(LOG_INFO, "[Map] - tiles 数组大小: %zu", ctx.tiles.size());
    TraceLog(LOG_INFO, "[Map] - Ground 层: %zu", ctx.groundLayers.size());
    TraceLog(LOG_INFO, "[Map] - Y-Sort 层: %zu", ctx.tileGIDs.size());
    TraceLog(LOG_INFO, "[Map] - Overhead 层: %zu", ctx.overheadLayers.size());

    return true;
}


/**
 * @brief 更新地图状态
 * 
 * @param map 地图指针
 */
bool UpdateMap(GameContext& map) {
    // TODO: 实现更新逻辑
    
    return true;
}

/**
 * @brief 获取地图宽度
 */
int GetMapWidth(const GameContext& map) {
    return map.width;
}

/**
 * @brief 获取地图高度
 */
int GetMapHeight(const GameContext& map) {
    return map.height;
}

/**
 * @brief 获取地图块大小
 */
int GetMapTileSize(const GameContext& map) {
    return map.tileSize;
}