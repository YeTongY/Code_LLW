#include "GameState.h"
#include "Map.h"

#include "tmxlite/Map.hpp"
#include "tmxlite/Layer.hpp"
#include "tmxlite/TileLayer.hpp"
#include "tmxlite/ObjectGroup.hpp"

#include <iostream>
#include <fstream>  // 文件操作核心库
#include <vector>   // 用vector存储地图的行（动态数组，比普通数组灵活）
#include <string>   // 存储每行的字符串
#include <set>

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
void DrawMap(const GameContext& map) {
    for(int y = 0;y < map.height;y++){
        for(int x = 0;x < map.width;x++){
            DrawSingleTile(map,x,y);
        }
    }
}

/**
 * @brief 绘制单个块
 * 
 * @param map 地图指针
 * @param tileX x坐标
 * @param tileY y坐标
 * @param type 地图块类型
 */
void DrawSingleTile(const GameContext& map, int tileX, int tileY) {
    // 获取 tile type
    TileType type = map.tiles[tileY][tileX];
    // 如果没有纹理映射，直接返回
    if (map.mapTextures.count(type) == 0) return;

    Texture2D tileset = map.mapTextures.at(type);

    // 计算在 tileset 中的位置（以 tileSize 为单位）
    int tileSetX = 0;
    int tileSetY = 0;

    //需要什么类型就在这里添加 然后对素材图进行行列匹配
    switch (type) {
    case TileType::EMPTY: tileSetX = 0; tileSetY = 0; break;
    case TileType::GRASS: tileSetX = 1; tileSetY = 0; break;
    case TileType::WALL:  tileSetX = 2; tileSetY = 0; break;
    default: break;
    }

    //计算在素材图中的位置（以 tileSize 为单位）
    Rectangle sourceRect = {
        static_cast<float>(tileSetX * map.tileSize),
        static_cast<float>(tileSetY * map.tileSize),
        static_cast<float>(map.tileSize),
        static_cast<float>(map.tileSize)
    };

    //计算在屏幕中的位置（以像素为单位）
    Vector2 destPos = {
        static_cast<float>(tileX * map.tileSize),
        static_cast<float>(tileY * map.tileSize)
    };

    // 只绘制切割出来的小块
    DrawTextureRec(tileset, sourceRect, destPos, WHITE);
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
    
    // 尝试多个可能的路径加载纹理
    const char* texturePaths[] = {
        "res/graphics/tilesets/Godot_Autotiles_32x32.png",
        "../res/graphics/tilesets/Godot_Autotiles_32x32.png",
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
    //清理旧的关卡数据
    ctx.tiles.clear();
    ctx.enemies.clear();

    //实现从 Tiled 文件加载地图和敌人数据
    tmx::Map tiledMap;
    if(!tiledMap.load(filepath)){
        TraceLog(LOG_ERROR, "[Map] Tiled 地图加载失败: %s", filepath);
        return false;
    }

    // 读取地图尺寸和图块大小
    ctx.width = tiledMap.getTileCount().x;
    ctx.height = tiledMap.getTileCount().y;
    ctx.tileSize = tiledMap.getTileSize().x; // 设置瓦片大小
    
    TraceLog(LOG_INFO, "[Map] 地图尺寸: %dx%d, 瓦片大小: %d", ctx.width, ctx.height, ctx.tileSize);
    
    // 初始化地图数据结构
    ctx.tiles.resize(ctx.height, vector<TileType>(ctx.width, TileType::EMPTY));

    // 读取图层数据
    const auto& layers = tiledMap.getLayers();
    for(const auto& layer : layers){
        if(layer->getType() == tmx::Layer::Type::Tile){
            // 处理图块图层
            const auto& tileLayer = layer->getLayerAs<tmx::TileLayer>();
            const auto& tiles = tileLayer.getTiles();

            // 遍历所有地块
            for(int y = 0; y < ctx.height; y++){
                for(int x = 0; x < ctx.width; x++){
                    unsigned int gid = tiles[y * ctx.width + x].ID;
                    if(gid != 0){
                        // 在这里添加地块分类，对能否通行进行判断
                        if(layer->getName() == "Collision"){
                            ctx.tiles[y][x] = TileType::WALL;
                        }
                        else if(layer->getName() == "Tree"){
                            // 装饰图层暂时不影响地图数据
                            ctx.tiles[y][x] = TileType::WALL;
                        }
                        else if(layer->getName() == "Enemy"){
                            ctx.tiles[y][x] = TileType::WALL;
                        }
                        else if(layer->getName() == "NPC"){
                            ctx.tiles[y][x] = TileType::WALL;
                        }
                        else if(layer->getName() == "Ground"){
                            ctx.tiles[y][x] = TileType::GRASS;
                        }
                }
            }
        }
    }

    // 解析对象层来创建敌人
    for(const auto& layer: layers){
        if(layer->getType() == tmx::Layer::Type::Object && layer->getName() == "Enemies"){
            const auto& objectlayer = layer->getLayerAs<tmx::ObjectGroup>();

                for(const auto& object : objectlayer.getObjects()){
                    Enemy enemy = {};//声明敌人主结构体

                    //读取敌人位置
                    float objX = object.getPosition().x;
                    float objY = object.getPosition().y;
                    float objHeight = object.getAABB().height;

                    //计算实际位置
                    enemy.gridX = static_cast<int>(objX / ctx.tileSize);
                    enemy.gridY = static_cast<int>((objY + objHeight - 1) / ctx.tileSize);
                    enemy.visualPosition = { (float)enemy.gridX * ctx.tileSize, (float)enemy.gridY * ctx.tileSize };
                    enemy.moveTarget = enemy.visualPosition;
                    enemy.patrolCenter = enemy.visualPosition;

                    //从tilesMap中获取敌人数据
                    //严肃声明，在tilesmap里面自定义敌人数据的时候，不要修改变量名，将对象层的敌人模块复制粘贴并且仅修改属性值
                    for(const auto& prop : object.getProperties()){
                        if (prop.getName() == "hp") enemy.stats.hp = prop.getIntValue();
                        if (prop.getName() == "maxHp") enemy.stats.maxHp = prop.getIntValue();
                        if (prop.getName() == "attack") enemy.stats.attack = prop.getIntValue();
                        if (prop.getName() == "defense") enemy.stats.defense = prop.getIntValue();
                        if (prop.getName() == "moveSpeed") enemy.moveSpeed = prop.getFloatValue();
                    }

                    //设置其他默认属性
                    enemy.isActive = true;
                    enemy.isMoving = true;
                    enemy.aiState = AI_STATE_PATROL;
                    enemy.currentDirection = ENEMY_DIR_DOWN;

                    //添加到世界中
                    ctx.enemies.push_back(enemy);
                }
                
            }
        }
    }

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