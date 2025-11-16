#include "GameState.h"
#include "Map.h"
#include <iostream>
#include <fstream>  // 文件操作核心库
#include <vector>   // 用vector存储地图的行（动态数组，比普通数组灵活）
#include <string>   // 存储每行的字符串
#include <set>

using namespace std;

/**
 * @brief 初始化地图
 * 
 * @param map 地图指针
 * @param w 宽
 * @param h 高
 * @param size 单位块的像素个数
 */
void InitMap(GameContext& map, int width, int height, int tileSize) {
    //对地图块进行初始化
    map.width = (width < 0) ? 0 : width;
    map.height = (height < 0) ? 0 : height;
    map.tileSize = (tileSize < 0) ? 0 : tileSize;

    //初始化vector容器，将位置全部改成EMPTY
    map.tiles.resize(map.height);

    for(auto& row : map.tiles){
        row.resize(map.width, TileType::EMPTY);
    }
}

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
 * @brief TODO: 从文件加载地图数据
 * 
 * @param map 地图指针
 * @param filepath 文件路径
 */
bool LoadMap(GameContext& map, const char* filepath) {
    // 打开文件流
    ifstream file(filepath);
    if (!file.is_open()) {
        cout << "错误: 无法打开文件 " << filepath << endl;
        return false;
    }

    // 清空原有地图数据
    map.tiles.clear();
    string line;

    //逐行读取数据
    while(getline(file, line)){
        vector<TileType> currentRow;
        for(char c : line){
            //后期会对不同地块进行分类
            switch(c)
            {
            case '0':
                currentRow.push_back(TileType::EMPTY);
                break;
            case '1':
                currentRow.push_back(TileType::GRASS);
                break;
            case '2':
                currentRow.push_back(TileType::WALL);
                break;
            default:
                break;
            }
        }
        map.tiles.push_back(currentRow);
    }

    //关闭文件
    file.close();
    // 设置 map 的宽高（以读取到的数据为准）
    map.height = static_cast<int>(map.tiles.size());
    map.width = (map.height > 0) ? static_cast<int>(map.tiles[0].size()) : 0;

    return true;
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
    // 加载整个 tileset 图集，并指向到所有 TileType（复用同一 GPU 纹理）
    Texture2D tileset = LoadTexture("res/data/maps/Godot_Autotiles_32x32.png");
    map.mapTextures[TileType::EMPTY] = tileset;
    map.mapTextures[TileType::GRASS] = tileset;
    map.mapTextures[TileType::WALL] = tileset;
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