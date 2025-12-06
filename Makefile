# ==============================================================================
# LLW_Project 的 Makefile (适用于 Windows + MSYS2)
#
# [使用方法]
# 在 PowerShell 或任何终端中，进入项目根目录，然后：
#    - 输入 `make` 或 `make all` 来编译整个项目。
#    - 输入 `make clean` 来删除所有编译生成的文件。
#    - 编译后，可执行文件会生成在 `build/` 文件夹中。
#
# [环境安装]
# 1. 下载安装 MSYS2: https://www.msys2.org/
# 2. 打开 "MSYS2 MSYS" 终端（注意不是 UCRT64 或 MINGW64），运行：
#    pacman -Syu
#    pacman -S make
# 3. 打开 "MSYS2 UCRT64" 终端，运行：
#    pacman -S mingw-w64-ucrt-x86_64-gcc
# 4. 将以下路径添加到系统环境变量 PATH（按顺序）：
#    C:\msys64\usr\bin
#    C:\msys64\ucrt64\bin
# 5. 重启 VS Code 或终端，验证安装：
#    make --version
#    gcc --version
#
# [注意事项]
# - 必须从 MSYS2 的 MSYS 环境安装 make（不是 UCRT64 的 mingw32-make）
# - 配置 PATH 后可以在任何终端（PowerShell/cmd）中使用 make 命令
# ==============================================================================

# 1. 编译器和编译参数
# ------------------------------------------------------------------------------
# 使用 g++ 作为 C++ 编译器
CC = g++
# CFLAGS 是用于编译 C 和 C++ 文件的参数
# -Wall: 开启所有警告，帮助你写出更规范的代码
# -g:    在可执行文件中包含调试信息，方便 GDB 调试
# -O1:   开启基础的编译优化
CFLAGS = -Wall -g -O1
# 指定使用 C++17 标准
CPPFLAGS = -std=c++17

# 2. 文件夹路径定义
# ------------------------------------------------------------------------------
# 存放 .cpp 源代码的文件夹
SRC_DIR = src
# 存放 .exe 和 .o (目标文件) 的输出文件夹
BUILD_DIR = build
# 存放 Raylib 库的文件夹
LIB_DIR = lib/raylib

# 3. 外部库 (Raylib + tmxlite + 头文件单文件库)
# ------------------------------------------------------------------------------
# 头文件搜索路径：raylib 自带 + tmxlite + 其它单文件库(json.hpp 已放入 raylib/include)
INCLUDES = -I$(LIB_DIR)/include -Ilib/tmxlite/include
# 告诉链接器去哪里找 libraylib.a 等库文件
LDFLAGS = -L$(LIB_DIR)/lib
# 需要链接的库列表
# 在 Windows 上使用 Raylib，这些都是必需的
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

# 静态链接选项，确保可执行文件包含运行时库
STATIC_FLAGS = -static -static-libgcc -static-libstdc++

# 4. 文件和最终目标 + 外部库源文件编译（tmxlite）
# ------------------------------------------------------------------------------
# 你最终想要生成的可执行文件的名字
TARGET = $(BUILD_DIR)/LLW.exe
# 自动查找 src/ 文件夹下所有的 .cpp 文件
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
# 根据找到的 .cpp 文件列表，生成一个对应的 .o (目标文件) 列表
# 例如，src/main.cpp 会被转换成 build/main.o
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# tmxlite 源文件（直接静态编入，不再单独生成 .a）
TML_DIR = lib/tmxlite
TML_SRC_DIR = $(TML_DIR)/src
# 仅匹配顶层 .cpp，miniz.c 作为 C 源一并加入
TML_SRCS_CPP = $(wildcard $(TML_SRC_DIR)/*.cpp)
TML_MINIZ_SRC = $(TML_SRC_DIR)/miniz.c
TML_PUGIXML_SRC = $(TML_SRC_DIR)/detail/pugixml.cpp
TML_OBJS_CPP = $(patsubst $(TML_SRC_DIR)/%.cpp, $(BUILD_DIR)/tmxlite_%.o, $(TML_SRCS_CPP))
TML_MINIZ_OBJ = $(BUILD_DIR)/tmxlite_miniz.o
TML_PUGIXML_OBJ = $(BUILD_DIR)/tmxlite_pugixml.o
TML_OBJS = $(TML_OBJS_CPP) $(TML_MINIZ_OBJ) $(TML_PUGIXML_OBJ)

# 将 tmxlite 对象加入最终链接对象列表
OBJS += $(TML_OBJS)

# ==============================================================================
# 编译规则 (告诉 make 如何一步步构建项目)
# ==============================================================================

# 第一个规则是默认规则。当你只输入 "make" 时，就会执行它。
# 它的目标是生成最终的 $(TARGET) 文件。
all: $(TARGET)

# 链接规则：如何生成最终的 .exe 文件
# 这个规则依赖于所有的 .o 文件都已经被成功编译出来。
$(TARGET): $(OBJS)
	@echo "链接生成可执行文件: $(TARGET)"
	$(CC) $(OBJS) -o $(TARGET) $(STATIC_FLAGS) $(LDFLAGS) $(LIBS)
	@echo "复制着色器文件到 build 目录..."
	@mkdir -p $(BUILD_DIR)/res/graphics/shaders
	@cp -f res/graphics/shaders/cyberpunk.fs $(BUILD_DIR)/res/graphics/shaders/
	@echo "编译完成! 请运行: build\LLW.exe"

# 模式规则：如何将一个 .cpp 文件编译成一个 .o 文件
# 这条规则的意思是："为了制作一个 build/目录下的.o文件，你需要一个 src/目录下的同名.cpp文件"
# $< 代表依赖文件 (即 .cpp)，$@ 代表目标文件 (即 .o)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "正在编译: $<"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

# tmxlite .cpp 源编译规则
$(BUILD_DIR)/tmxlite_%.o: $(TML_SRC_DIR)/%.cpp
	@echo "编译 tmxlite 源: $<"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -DTMXLITE_STATIC -I$(TML_DIR)/include -c $< -o $@

# tmxlite miniz.c 编译规则
$(BUILD_DIR)/tmxlite_miniz.o: $(TML_SRC_DIR)/miniz.c
	@echo "编译 tmxlite 源: $<"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -DTMXLITE_STATIC -I$(TML_DIR)/include -c $< -o $@

# tmxlite pugixml.cpp 编译规则
$(BUILD_DIR)/tmxlite_pugixml.o: $(TML_SRC_DIR)/detail/pugixml.cpp
	@echo "编译 tmxlite 依赖: $<"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -DTMXLITE_STATIC -I$(TML_DIR)/include -c $< -o $@

# 清理规则：用于删除所有编译产物
# 当你运行 "make clean" 时，会执行这里的命令
clean:
	@echo "正在清理项目..."
	@rm -rf $(BUILD_DIR)
	@echo "清理完成。"

# .PHONY 用于声明一个"伪目标"，它不是一个真正的文件名。
# 这样做可以防止 make 因为存在一个名为 "clean" 的文件而出错。
.PHONY: all clean
