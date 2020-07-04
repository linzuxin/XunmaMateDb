#### 本地编译&测试

- 软件安装
    - linux/mac ： cmake ， g++ 环境
    - windows：wingGW，cmake 环境

- 通过在 src/main.cpp 编写代码进行调试（学生&老师可用）
    1. 在终端（命令行）进入到项目根目录下的src目录
    2. 使用 g++ -o test student/*.cpp main.cpp 命令进行编译
    3. 执行
        - mac/Linux： ./test
        - windows： test.exe

- 直接通过gtest进行测试（老师可用）
  mac/Linux下：
    1. 在终端（命令行）进入到项目根目录下
    2. 创建 build 目录 : mkdir build && cd build
    3. 编译 : cmake ../ && make
    4. 运行测试 : make test
    5. 修改代码后可直接 build 目录下执行 : make clean && make && make test 即可
  windows 下：
    1. 在终端（命令行）进入到项目根目录下
    2. 进入到  script 目录下
    3. 编译 : build.bat
    4. 运行测试 : run.bat
    