#include "Map.h"
#include <iostream>
#include <fstream>  // 文件操作核心库
#include <vector>   // 用vector存储地图的行（动态数组，比普通数组灵活）
#include <string>   // 存储每行的字符串

using namespace std;

std::vector<std::vector<TileType>>tiles;

/**
 * @brief 构造函数：初始化地图
 * 
 * @param w 宽
 * @param h 高
 * @param size 单位块的像素个数
 */
Map::Map(int w,int h,int size):width(w), height(h), tileSize(size) {
    //对地图块进行初始化
    if(width < 0) width = 0;
    if(height < 0) height = 0;
    if(tileSize < 0) tileSize = 0;

    //初始化vector容器，将位置全部改成EMPTY
    tiles.resize(height);                            //初始化行

    for(auto& row : tiles){
        row.resize(width, TileType::EMPTY);     //初始化每一列
    }
}

/**
 * @brief 析构函数：清理资源
 * 
 */
Map::~Map() {
    
}

/**
 * @brief TODO: 从文件加载地图数据
 * 
 * @param filepath 
 */
bool Load(const char* filepath) {
    // 打开文件流
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "错误" << filepath << std::endl;  // 打开错误进行报错
        return false;
    }

    // 清空原有地图数据
    tiles.clear();
    std::string line;

    //逐行读取数据
    while(std::getline(file, line)){
        std::vector<TileType> currentRow;
        for(char c : line){
            //后期会对不同地块进行分类
            switch(c)
            {
            case '0':{
                currentRow.push_back(TileType::EMPTY);
                break;
            }
            case '1':{
                currentRow.push_back(TileType::GRASS);
                break;
            }
            case '2':{
                currentRow.push_back(TileType::WALL);
                break;
            }
            default:{
                break;
            }
            }
        }
        tiles.push_back(currentRow);
    }

    //关闭文件
    file.close();
    return true;
}

void Draw() {
    
}

/**
 * @brief 绘制单个块
 * 
 * @param tileX x坐标
 * @param tileY y坐标
 * @param tileMap 地图块坐标
 */
void DrawSingleTile(int tileX,int tileY,TileType type, int tileSize) {

}

/**
 * @brief 更新地图状态
 * 
 */
bool Map::Update() {

    return true;
}

/**
 * @brief TODO: 检测碰撞
 * 
 * @param position 位置信息
 * @return true 
 * @return false 
 */
bool CheckCollision(Vector2 position) {
    
    return false;
}
