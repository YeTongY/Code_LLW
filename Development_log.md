# 开发日志
2025-11-13 77eb37a init: 初始化 LLW 项目结构
2025-11-13 6b2624a 更新readme文件
2025-11-13 cb3e2de 更新GDD文件
2025-11-13 634a95b 更新GDD文件
2025-11-13 67ddda7 feat: 实现 Raylib 测试程序并完善构建系统
2025-11-13 b456f9c docs:更新了readme.md中的项目名称
2025-11-13 1ccca5b test:增加test文件夹练手raylib
2025-11-13 290dbf6 在gitignore文件夹中添加了.vscode
2025-11-13 46403d2 fix: 修复 Makefile 跨平台兼容性问题
2025-11-13 4d7d604 docs: 更新文档统一使用 MSYS2 make 命令
2025-11-13 126225b fix: 删去了误传上去的vscode配置文件
2025-11-13 ec42c58 1
2025-11-13 c9b359a 保留本地VS Code配置
2025-11-13 1e303c2 停止追踪.vscode配置文件
2025-11-13 848bc3a 建立了完整的项目结构文件夹
2025-11-13 cb97bad feat:- 添加对中文字体的支持 - 添加sample样例文档，以方便参考
2025-11-14 0fa95cc 状态机第一版接口
2025-11-14 71d7803 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-14 565bdff feat: 添加游戏引擎核心结构和库引用
2025-11-14 10555ef feat:初步实现地图读取功能
2025-11-14 d5abc59 feat: 添加字体加载器和游戏引擎核心
2025-11-14 25bfe39 docs:修改readme中的文件结构
2025-11-14 6c85973 refactor: 优化项目结构和头文件保护
2025-11-14 91bd221 fix:移除重复的md文件
2025-11-14 0936570 docs:添加字体加载模块 (`FontLoader`) 说明文档 - 添加了字体加载模块说明文档 - 把readme移到根目录去了，以能在GitHub正常显示
2025-11-14 792d110 fix:向全文本文件添加字体示例程序中所要显示的汉字
2025-11-14 9a627bd fix: 修正注释
2025-11-14 8a0de21 docs: 添加小型修复 / 杂项修复 的提交信息说明（misc 提交规范）
2025-11-14 a80f81d misc: 删除FontLoader.h注释的空行
2025-11-15 7ddedd6 feat:在GameState.h的Gameenigine结构体声明中加入了player结构体
2025-11-15 75dba18 misc: 删去Player下的占位符
2025-11-15 f64c204 feat: 在Gamestate的engine结构体中添加currentMapData和Camera2D camera
2025-11-15 a3bf4d2 fix:移除了enemy和combat中的错误代码
2025-11-15 f615af9 fix:在gamestate头文件下添加了using namespace std
2025-11-15 a582089 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-15 4a53ebd feat:将map模块的面向对象编程修改为类c格式的代码
2025-11-15 7e594dc Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-15 d72fb3a feat:初步修改了Map模块中面向对象编程的部分，改为类c语言的格式
2025-11-15 b4b9345 feat:初步修改了Map模块中面向对象编程的部分，改为类c语言的格式
2025-11-15 8bf6807 refactor: 将 Map 模块改为 C 风格实现
2025-11-15 867ee9a feat:将map模块的核心数据同步到GameState.h的enging结构体中
2025-11-15 d0540c8 feat: 实现 Player 模块的移动和渲染功能
2025-11-15 37ae90f fix:解决合并冲突
2025-11-15 747a353 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-15 2f8f953 fix: 修复 Player 碰撞检测中的"越界陷阱"
2025-11-15 d9b1951 chore: 提交 Player 越界修复并整理 sample 文件
2025-11-15 ea15b5a docs:添加角色模块说明文档 -删去了根本运行不起来的ai示例程序
2025-11-15 3a972b5 fix: 修复 GameState.h 与 Player.h 之间的循环引用
2025-11-15 442abde 推送方便下一步规划:
2025-11-15 a55ec33 chore：构建了完整的状态机头文件
2025-11-15 aa007b8 refactor: 将 Player 和 FontLoader 模块的结构体名统一为 GameContext
2025-11-15 96a4922 misc:添加一行小注释
2025-11-16 d67ffe9 misc: 添加头文件中对drawplayer的注释
2025-11-16 d5ba5a2 style：统一注释缩进格式-直观明了
2025-11-16 69f74f5 feat：实现了map部分读取地图和绘制功能
2025-11-16 a7f194a misc: 添加player里关于摄像机设置的注释
2025-11-16 075e27f refector：将地块类型和地图资产放到核心数据库GameContext中
2025-11-16 a20f23c refector：将map.h中元素全部存入核心数据库进行调用，同意数据调用
2025-11-16 5b51c87 feat: 添加战斗逻辑
2025-11-16 6b1be91 refactor: 改为纯C++风格并实现探索状态
2025-11-16 284819e refactor:把状态机实现改为纯c++
2025-11-16 9bd05cf refector:重构GameState.h，解决map.cpp报错
2025-11-16 cce268c refector:修改了map中函数的只读问题，方便player同步
2025-11-16 02ecbcf fix: 完善player的drawmap调用
2025-11-16 181ee5d misc: 将1920统一为ctx.screenWidth,将1080统一为ctx.screenHeight
2025-11-16 9192923 feat：将地图素材更新为单张，使用的时候进行调用裁剪
2025-11-16 c27aac2 fix:修改了主函数中width和height未初始化问题，增加纹理加载函数初始化，等待map纹理
2025-11-16 a1d6898 refector:增加了一个素材，编写了初步的地图进行测试
2025-11-16 e025868 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-16 c1ff2c5 fix:修复了文件读取地图功能
2025-11-16 02f1b80 feat: 添加了塔菲素材，已经燃尽了
2025-11-16 a23a1e2 misc:塔菲素材小修小改
2025-11-17 2c2d0a3 feat: 实现真正的塔菲渲染
2025-11-17 5ffca44 fix: 补上了player.h中player结构体缺少的spritesheet声明
2025-11-17 3ad2ace feat:对Enemy部分进行结构搭建，等待填充逻辑
2025-11-17 0f24139 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-17 18d346c fix: 修复玩家精灵图"花屏" Bug 并完善初始化
2025-11-17 9b7ec26 fix: 在 Windows 上将控制台输出设置为 UTF-8，避免日志乱码
2025-11-17 534fdc3 fix:修复了地图渲染歪着的错误
2025-11-17 0e59cba 实现赛博朋克滤镜系统 - 仅应用于地图渲染
2025-11-17 2954ad6 fix:修复了双击编译后的程序无法加载滤镜的错误
2025-11-17 7c81e9b chore: 重构了player绘制纹理的逻辑，以一种我看不懂的方法
2025-11-17 465fd6a fix:修复了build目录打开程序中文字体无法渲染的问题
2025-11-17 658b204 feat:添加了辉光文字绘制函数 chore:引入raygui.h以备后续gui绘制
2025-11-17 430330c 实现玩家平滑移动动画与UI显示优化
2025-11-17 e822604 misc:修改注释
2025-11-17 3904f5b feat：初步实现enemy部分功能
2025-11-17 fd1cc59 feat:实现了敌人的平滑移动，等待后期实现敌人ai以实现敌人范围巡逻
2025-11-17 e3a1fc8 重构: GDD v3.13 架构优化 - 职责分离与代码复用
2025-11-17 03d401e 合并: 解决 Player 模块重构的冲突并修复 Enemy 模块错误
2025-11-17 9bbc3a8 新增: DialogueState 模块和对话框 UI 资源
2025-11-17 1e1d482 docs:添加说明文档说明player修改
2025-11-17 aacf7f2 修复: 摄像机中心点定位问题，添加屏幕尺寸验证
2025-11-17 1f45e85 feature:更新taffy精灵图为更新后的
2025-11-17 a329c2b 修改: 更新raygui.h包含路径到lib/raylib/include
2025-11-17 26413c0 删除: 移除src/raygui.h（已移至lib/raylib/include）
2025-11-17 5197115 chore:添加库
2025-11-17 2dec2db feat: 添加了加载对话框ui的函数
2025-11-18 466063b feat:添加了敌人巡逻和追击玩家逻辑，优化了Enemy.h库和GameState.h库中调用数据的冲突
2025-11-18 c49ed76 fix:修复了GameState的核心库合并冲突
2025-11-18 095a32b fix: 修复不统一的enemies命名
2025-11-18 a14697b misc: 补一部分命名，添加更多塔菲
2025-11-18 1c2a050 chore: 添加ui初始结构
2025-11-18 c63d5af chore:添加tmxtile库
2025-11-18 7d970f7 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-18 3112c2c 集成: 正确引入tmxlite、json.hpp等lib库
2025-11-18 7cf7e2b feat: 完成了UI.cpp和UI.h的编写，下一步写dialogue
2025-11-18 772b2c0 feat：添加了标题菜单第一版文件
2025-11-18 0414207 WIP: chore:正在进行对话编写，
2025-11-18 12f122d Merge branches 'main' and 'main' of https://github.com/YeTongY/Code_LLW
2025-11-18 1a487de (空提交信息)
2025-11-18 af6a275 feat:完成dialogue，加了状态机
2025-11-18 c82782d Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-18 29ff292 misc:测试剧本
2025-11-18 5490bff feat：实现从tilesmap中渲染地图和敌人，并且进行分层渲染的同时保证地图的不可通行地块
2025-11-18 68b263e Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-18 c15d345 feat：修复了map.cpp的括号匹配错误，在tiledmap中添加了测试战斗用的敌人
2025-11-18 8a81e11 fix:用更新的逻辑替换旧逻辑
2025-11-18 908895c fix：修复了无限地图的问题
2025-11-18 3319c30 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-18 f31ec93 fix：修复了地图文件格式bug
2025-11-18 d6962e5 fix:修复地图路径错误，后缀忘改了
2025-11-18 8fd979c fix:修复地图素材路径
2025-11-18 26f5028 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-18 c857e13 feat: 增加按E检测
2025-11-18 12026c5 【重构与修复】数据驱动、地图系统与UI修复
2025-11-18 90fdfbf 修复地图加载和黑屏问题
2025-11-18 36e5f2f fix:修改了taffy的台词
2025-11-19 93d8840 回退 Level 1.tmx 到原始版本，避免 tileset firstgid 错乱
2025-11-19 5d52f0e 确保 Level 1.tmx 文件正确恢复
2025-11-19 23f0443 fix：重构map函数，修复了地图渲染没有分层的问题
2025-11-19 7fa7e40 misc: 删了个std
2025-11-19 7c32081 fix:修复了渲染重复次数过多的问题
2025-11-19 65515a2 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-19 0cbfcbb feat:实现了敌人数据读取和ai逻辑移动，等待状态机和渲染部分调整
2025-11-19 a0852be chore:在GameContext中添加了Event数组
2025-11-19 c5b5ac4 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-19 de87cee chore: 重构了事件的结构体
2025-11-19 86251b4 WIP:Event
2025-11-19 1a38f22 feat:添加了对event的访问，但完整功能仍需等待对event的写入完成
2025-11-19 97b6589 feat:增加在map中读取tiled事件层的功能
2025-11-19 fd720af Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-19 3ea14a7 fix:将所有 using std::xxx; 改为 using namespace std; …（循环依赖修复等）
2025-11-19 983bcef Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-19 9331ff2 fix:map读取tiled事件
2025-11-19 185829e Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-19 9d5b90d fix:map读取传送事件中push_back的错误… feat：对话/传送事件实现
2025-11-20 8de71e7 fix:修复更新函数，可以正常读取tiled事件数据
2025-11-20 2b5287a WIP:事件加载
2025-11-20 740fa8c fix:修复了游戏闪退问题
2025-11-20 d8f171e fix:修复了游戏闪退问题
2025-11-20 22ab304 fix:修复了朝向重复warning的问题
2025-11-20 3f95b4a feat:完成了事件的监听和运行，现在等待事件加载完成
2025-11-21 9d10ca4 feat:完成第一版战斗模块
2025-11-21 25b78a7 feat:大修map和event
2025-11-21 6b253ed Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-21 f10a2b1 fix: 删掉了多余的脚本路径
2025-11-21 e0c8363 misc: 添加event的注释，现在等待添加事件来测试
2025-11-23 4966422 feat:新增了测试用地图切换和对话部分
2025-11-23 744bb1a fix:删除了多余的dialogues文件夹
2025-11-23 0c65c4b fix:修复了文件名出现的bug
2025-11-23 b9a059e misc:添加按e加速对话播放
2025-11-23 2c507e3 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-23 7c096fa fix:修正了玩家碰撞箱位置，现在会直接使用逻辑位置（网格位置）
2025-11-23 d510e47 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-23 07ca75d fix:修复了tiled地图问题
2025-11-23 ed04439 feat：对UpdateEnemies函数进行更新，优化了敌人的平滑移动逻辑
2025-11-24 9deb0e9 misc:添加了标题界面的图片，以备标题界面更新
2025-11-24 435afd7 misc:添加了py程序来裁剪敌人朝向素材
2025-11-25 1b911b7 misc:将敌人素材进行切割
2025-11-25 1d59a18 feat:添加标题界面背景图
2025-11-25 bfed6bf feat：实现了敌人移动逻辑，可以追逐玩家
2025-11-25 cf8fe3f fix:修复编译问题
2025-11-25 0ede5f8 feat:对敌人移动逻辑优化，防止敌人巡逻目标为墙壁
2025-11-25 b8a4065 feat:完成了完整的测试版战斗系统
2025-11-26 c92ae44 feat：实现了敌人的四向移动和碰撞检测逻辑
2025-11-26 f322a70 feat:添加了未完的y排序，等待分层渲染完工
2025-11-27 c37a048 feat：分层渲染问题解决
2025-11-27 fa5686a feat：解决了分层渲染问题
2025-11-27 206837e feat：添加地图进行测试分层渲染
2025-11-28 ee48d23 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-28 7f94006 fix:修复对话路径
2025-11-28 2f57551 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-29 a695119 feat:添加资源，添加探索界面ui WIP:现在有问题…
2025-11-29 c41170c Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-30 2d5d1f7 fix：修复了贴墙物体的排序顺序问题
2025-11-30 d3e302c fix:修复地图乱码问题
2025-11-30 f603346 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-30 f689192 fix：修复了地图上不显示敌人的问题
2025-11-30 f1641e8 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-30 1b991ba fix：修复了战斗不切换的问题
2025-11-30 8304d48 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-11-30 4e560c8 misc：尝试修复头像ui不正常显示
2025-11-30 a3edf19 feat:添加脚步声
2025-11-30 96c037a misc:修复一个耳机砸键盘上的错误
2025-11-30 ca5e3d1 misc:修复声明错误
2025-11-30 48d6267 feat:完成了脚步的播放
2025-11-30 983dcca misc:统一日志样式
2025-12-01 7b8bef5 feat:添加背景音乐，不过在对话时不播放
2025-12-01 94e21f4 Implement defend stance logic
2025-12-01 9c11fc6 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-12-01 672d985 misc:换了个开始和关闭探索音乐的方式
2025-12-01 e47a1a2 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-12-01 67956a2 feat:添加战斗bgm
2025-12-01 8dac98c misc:修复了bgm播放，现在它十分的正常
2025-12-01 fd8ff54 feat:添加L04个剧情文件
2025-12-01 6b578ef feat:添加塔菲移动动画
2025-12-02 66b3725 修复了y排序和敌人闪烁问题
2025-12-02 d50d3ac Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-12-02 d500a65 fix:添加了巨量注释
2025-12-02 e99c8ec chore:添加战斗背景图片
2025-12-02 005b5b2 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-12-02 c1fd3d9 misc:修改图片
2025-12-02 b8bbbfd misc:将剧情文本加到字体加载文件里
2025-12-02 456e04b feat: 添加了战斗前后剧本触发
2025-12-02 fee3bd1 feat:回退渲染顺序
2025-12-02 921cd80 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-12-02 5fc316f feat : Level 1地图
2025-12-02 5978998 feat:尽力了.....地图l0-1
2025-12-02 91d2218 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-12-02 c5892a3 feat：添加了L1地图，
2025-12-02 0be6742 feat:初步绘制L0-1，进行测试地图传送功能是否正常
2025-12-02 73a74cb feat:按瓦片坐标读取传送目标点
2025-12-02 58ce720 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-12-03 ab6212b fix:优化了Level0-1和Level1的地图
2025-12-03 6f00c1e fix：修复了0-1的对话不触发问题
2025-12-03 8419dfa feat:添加恢复事件，等待地图读取实现
2025-12-03 52aa405 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-12-03 77fd7be feat：添加了Level 2 404地图
2025-12-03 78bcae4 feat: 添加不加载地图的传送
2025-12-03 0b2956f fix:优化地图细节和假传送的敌人重新生成问题
2025-12-04 3f729f6 docs：更新了README.md中map和enemy部分的简介和困难分析与解决
2025-12-04 c8b4d4d fix:修复了boss房间的地面
2025-12-04 52f3a4e fix：剧情补全
2025-12-04 06afe7b fix:添加了战斗背景
2025-12-05 6624687 feat:增加了L3的地图和对话，优化了敌人数值，修改了boss材质
2025-12-05 571afbe fix：修复L3地图
2025-12-05 91ad1fd feat:为战斗系统补充了伤害浮动和暴击机制
2025-12-05 8d9e4d4 fix:文本修复，贴图更新
2025-12-05 b8026f7 fix:把所有的MMK换成了momoka
2025-12-05 e93c81a fix:修复了地图敌人属性
2025-12-06 8ad8446 fix:修复L2的假门传送
2025-12-06 32d82e2 fix:修复L3的假传送
2025-12-06 0b3340b feat:修复未每帧调用更新音频流导致的音乐播放卡顿
2025-12-06 8253de7 fix:确认卡顿源于主进程日志，移除并回退音频逻辑
2025-12-06 324ae11 fix:更改结束对话为中文为主
2025-12-06 09e7386 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-12-06 41a011d feat:添加战斗胜利后恢复40%已损生命
2025-12-06 be161f1 feat:在战斗胜利后的剧情添加了回血说明
2025-12-06 3977acf feat:将日志显示级别调到warning及以上以避免日志输出阻塞主进程
2025-12-06 afe8e3c fix:把莫名其妙失效了的recover事件读取加回去了
2025-12-06 cb9596d fix:优化角色属性，给L
2025-12-06 049278f Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-12-06 1537b61 fix：把同时失效的recover事件入队给修好了…
2025-12-06 6199250 feat:添加了两个简单技能（未添加蓝条版本）
2025-12-06 04c811c fix:删除了测试版本的测试ui
2025-12-06 985ac0e Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-12-06 34060c4 fix:修复boss移动和巡逻属性，添加彩蛋
2025-12-06 87d8a99 fix:momoka对话框的文字
2025-12-06 ee1919f fix: 更新战斗界面的操作指南，修正对话内容
2025-12-06 8403013 feat:添加蓝和蓝条
2025-12-06 be39e8d Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-12-06 7ef43cb feat：完善隐藏事件
2025-12-06 341f4e6 fix：修复字体显示，添加蓝量小彩蛋
2025-12-06 310b31e Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-12-06 0d7090b misc:修改恢复事件，使其能够同时恢复魔法值
2025-12-06 9e51f18 Merge branch 'main' of https://github.com/YeTongY/Code_LLW
2025-12-06 3f46457 docs：完善readme
2025-12-06 4b916c7 misc:readme小修
2025-12-06 4b0bca6 misc:再修readme
2025-12-06 16289d8 feat:技能系统进一步完善