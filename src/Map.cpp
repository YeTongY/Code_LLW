#include "GameState.h"
#include "Map.h"
#include <iostream>
#include <fstream>  // 文件操作核心库
#include <vector>   // 用vector存储地图的行（动态数组，比普通数组灵活）
#include <string>   // 存储每行的字符串

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
    for (auto const& [key, val] : map.mapTextures) {
        UnloadTexture(val);
    }
    //卸载地图纹理
    map.tiles.clear();             //释放内存
    map.mapTextures.clear();       //释放内存
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
    return true;
}

/**
 * @brief 绘制地图
 * 
 * @param map 地图指针
 */
void DrawMap(const GameContext& map) {
    // TODO: 实现绘制逻辑
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
    //获取纹理坐标
    TileType type = map.tiles[tileY][tileX];
    //判断纹理是否存在
    if(map.mapTextures.count(type)){
        Texture2D Texture = map.mapTextures.at(type);
        //计算世界坐标
        int worldX = tileX * map.tileSize;
        int worldY = tileY * map.tileSize;
        //绘制
        DrawTexture(Texture,worldX,worldY,WHITE);
    }
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
    map.mapTextures[TileType::EMPTY] = LoadTexture("res/data/maps/empty.png");
    map.mapTextures[TileType::GRASS] = LoadTexture("res/data/maps/grass.png");
    map.mapTextures[TileType::WALL] = LoadTexture("res/data/maps/wall.png");
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