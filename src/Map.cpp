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
void InitMap(Map* map, int w, int h, int size) {
    //对地图块进行初始化
    map->width = (w < 0) ? 0 : w;
    map->height = (h < 0) ? 0 : h;
    map->tileSize = (size < 0) ? 0 : size;

    //初始化vector容器，将位置全部改成EMPTY
    map->tiles.resize(map->height);

    for(auto& row : map->tiles){
        row.resize(map->width, TileType::EMPTY);
    }
}

/**
 * @brief 清理地图资源
 * 
 * @param map 地图指针
 */
void CleanupMap(Map* map) {
    map->tiles.clear();
}

/**
 * @brief TODO: 从文件加载地图数据
 * 
 * @param map 地图指针
 * @param filepath 文件路径
 */
bool LoadMap(Map* map, const char* filepath) {
    // 打开文件流
    ifstream file(filepath);
    if (!file.is_open()) {
        cout << "错误: 无法打开文件 " << filepath << endl;
        return false;
    }

    // 清空原有地图数据
    map->tiles.clear();
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
        map->tiles.push_back(currentRow);
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
void DrawMap(Map* map) {
    // TODO: 实现绘制逻辑
}

/**
 * @brief 绘制单个块
 * 
 * @param map 地图指针
 * @param tileX x坐标
 * @param tileY y坐标
 * @param type 地图块类型
 */
void DrawSingleTile(Map* map, int tileX, int tileY, TileType type) {
    // TODO: 实现单个块绘制逻辑
}

/**
 * @brief 更新地图状态
 * 
 * @param map 地图指针
 */
bool UpdateMap(Map* map) {
    // TODO: 实现更新逻辑
    return true;
}

/**
 * @brief TODO: 检测碰撞
 * 
 * @param map 地图指针
 * @param position 位置信息
 */
bool CheckMapCollision(Map* map, Vector2 position) {
    // TODO: 实现碰撞检测逻辑
    return false;
}

/**
 * @brief 获取地图宽度
 */
int GetMapWidth(const Map* map) {
    return map->width;
}

/**
 * @brief 获取地图高度
 */
int GetMapHeight(const Map* map) {
    return map->height;
}

/**
 * @brief 获取地图块大小
 */
int GetMapTileSize(const Map* map) {
    return map->tileSize;
}