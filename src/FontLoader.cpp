#include "FontLoader.h"
#include <fstream>   // 包含 C++ 文件库
#include <sstream>   // 包含 C++ 字符串流库
#include <string>    // 包含 C++ 字符串库
#include <raylib.h>

using namespace std;


/**
 * @brief 仅存在于FontLoader中的辅助函数，用于读取文本文件
 * 
 * @param text_address 要读取的文本文件的地址
 * @return string 返回读取完成的字符串
 */
static string textLoader(const char* text_address){

    //1.输入文件流
    ifstream file(text_address);

    // 2. 检查文件是否成功打开 (和 C 语言的 if (file == NULL) 一样重要)
    if (!file.is_open()) {
        // TraceLog 是 raylib 的日志功能，比 printf 好
        TraceLog(LOG_WARNING, "[Loader] Failed to open file: %s", text_address);
        return ""; // 返回一个空字符串，表示失败
    }

    // 3. 创建一个“字符串流” (stringstream)
    stringstream buffer;

    buffer << file.rdbuf();//从文件流中读取文字，存放到buffer
    file.close();//完成对文件的读取，关闭文件

    return buffer.str();//返回读取完成后的字符串

}

/**
 * @brief 用于加载字体
 * 
 * @param engine Gamestate里的engine核心数据文件
 */


void loadGameFont (GameEngine &engine){
    const char* fontFile_Address = "res/data/fonts/SourceHanSansSC-Regular.otf";//加载思源黑体字体文件
    const char* allGameText_Address = "res/data/dialogue/all_game_script.txt";//加载全文本文件

    string allGameText = textLoader(allGameText_Address);//读取全文本文件
    allGameText += "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!?.,:;-()";//给全文本文件添加基本英文字符以防止潜在的错误
    
    int codepointCount = 0; //初始化码点数量
    int* codepoints = LoadCodepoints(allGameText.c_str(), &codepointCount);//从文本文档里读取全部码点
    TraceLog(LOG_INFO, "[FontLoader] Found %d unique codepoints in script file.", codepointCount);//读取码点的日志输出

    engine.mainFont = LoadFontEx(//烘焙字体
        fontFile_Address, //字体地址
        48, //字体大小
        codepoints, //要烘焙的码点
        codepointCount //码点数
    );
    UnloadCodepoints(codepoints); //释放码点
    if (engine.mainFont.texture.id != 0) {//检查字体是否被成功烘焙
        TraceLog(LOG_INFO, "[FontLoader] Font '%s' loaded successfully.", fontFile_Address);
    } else {
        TraceLog(LOG_ERROR, "[FontLoader] FAILED to load font: %s", fontFile_Address);
    }

}

/**
 * @brief 卸载已烘焙的字体
 * 
 * @param engine Gamestate里的engine核心数据文件
 */
void unloadGameFont (GameEngine &engine){
    if(engine.mainFont.texture.id != 0){//检查字体是否被成功烘焙
        UnloadFont(engine.mainFont);
        TraceLog(LOG_INFO, "[FontLoader] Unloaded game font.");
    }
}

