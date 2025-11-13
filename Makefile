# ==============================================================================
# Makefile for LLW_Project on Windows with GCC (MinGW-w64)
# ==============================================================================

# 1. Compiler and Flags
# ------------------------------------------------------------------------------
# Use g++ for C++ compilation
CC = g++
# CFLAGS are compiler flags for C and C++ files
# -Wall: Turn on all warnings, helps you write better code
# -g:    Include debugging information in the executable
# -O1:   Basic optimization
CFLAGS = -Wall -g -O1
# We are using C++17 standard
CPPFLAGS = -std=c++17

# 2. Directories
# ------------------------------------------------------------------------------
# Source directory where your .cpp files are
SRC_DIR = src
# Build directory where the .exe and object files will be placed
BUILD_DIR = build
# Library directory for Raylib
LIB_DIR = lib/raylib

# 3. Raylib Specific Paths
# ------------------------------------------------------------------------------
# Tell the compiler where to find raylib.h
INCLUDES = -I$(LIB_DIR)/include
# Tell the linker where to find libraylib.a
LDFLAGS = -L$(LIB_DIR)/lib
# List of libraries to link
# For Raylib on Windows, these are required
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

# 4. Files and Targets
# ------------------------------------------------------------------------------
# The name of your final executable
TARGET = $(BUILD_DIR)/LLW.exe
# Find all .cpp files in the source directory automatically
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
# Create a list of object files (.o) from the source files list
# e.g., src/main.cpp becomes build/main.o
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# ==============================================================================
# Rules - The "Recipes" for Building
# ==============================================================================

# The first rule is the default one, executed when you just type "make"
all: $(TARGET)

# Rule to link the final executable
# This depends on all the object files being ready
$(TARGET): $(OBJS)
	@echo "Linking executable: $(TARGET)"
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS) $(LIBS)
	@echo "Build finished! Run with: $(TARGET)"

# Pattern rule to compile a .cpp file into a .o object file
# This says: "To make a file like build/something.o, you need src/something.cpp"
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling: $<"
	@mkdir -p $(BUILD_DIR) # Create build directory if it doesn't exist
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

# Rule to clean up the project
# Deletes the build directory and all its contents
clean:
	@echo "Cleaning project..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean finished."

# Phony targets are not actual files
.PHONY: all clean
# ==============================================================================
# LLW_Project 的 Makefile (适用于 Windows + GCC/MinGW-w64)
#
# [使用方法]
# 1. 确保你已经安装了 MinGW-w64 (提供了 gcc, g++ 和 make 工具)。
# 2. 确保 make 和 g++ 已经添加到了系统的 PATH 环境变量中。
# 3. 在项目根目录 (LLW_Project/) 打开终端，然后：
#    - 输入 `make` 或 `make all` 来编译整个项目。
#    - 输入 `make clean` 来删除所有编译生成的文件。
#    - 输入 `make` 后，可执行文件会生成在 `build/` 文件夹中。
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

# 3. Raylib 相关的路径和库
# ------------------------------------------------------------------------------
# 告诉编译器去哪里找 raylib.h 等头文件
INCLUDES = -I$(LIB_DIR)/include
# 告诉链接器去哪里找 libraylib.a 等库文件
LDFLAGS = -L$(LIB_DIR)/lib
# 需要链接的库列表
# 在 Windows 上使用 Raylib，这些都是必需的
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

# 4. 文件和最终目标
# ------------------------------------------------------------------------------
# 你最终想要生成的可执行文件的名字
TARGET = $(BUILD_DIR)/LLW.exe
# 自动查找 src/ 文件夹下所有的 .cpp 文件
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
# 根据找到的 .cpp 文件列表，生成一个对应的 .o (目标文件) 列表
# 例如，src/main.cpp 会被转换成 build/main.o
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

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
    $(CC) $(OBJS) -o $(TARGET) $(LDFLAGS) $(LIBS)
    @echo "编译完成! 请运行: $(subst /,\,$(TARGET))"

# 模式规则：如何将一个 .cpp 文件编译成一个 .o 文件
# 这条规则的意思是：“为了制作一个 build/目录下的.o文件，你需要一个 src/目录下的同名.cpp文件”
# $< 代表依赖文件 (即 .cpp)，$@ 代表目标文件 (即 .o)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
    @echo "正在编译: $<"
    @mkdir -p $(BUILD_DIR) # 如果 build 文件夹不存在，就创建它
    $(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

# 清理规则：用于删除所有编译产物
# 当你运行 "make clean" 时，会执行这里的命令
clean:
    @echo "正在清理项目..."
    @rm -rf $(BUILD_DIR)
    @echo "清理完成。"

# .PHONY 用于声明一个“伪目标”，它不是一个真正的文件名。
# 这样做可以防止 make 因为存在一个名为 "clean" 的文件而出错。
.PHONY: all clean