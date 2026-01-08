# lib_bsp 仓库架构设计说明

# 1. 文档概述

## 1.1 设计目标

为嵌入式 Linux 系统构建统一、标准化、可复用的板级支持包（BSP）硬件抽象库，封装蜂鸣器、LED、按键、摄像头、IMU、气压计、温湿度传感器的底层硬件操作接口；同时提供轻量级命令行测试工具，支持脱离服务化逻辑的硬件快速验证与调试。核心目标：

- 接口标准化：不同硬件提供统一风格的调用接口，降低上层使用成本

- 结构轻量化：简化目录层级，避免冗余嵌套，提升开发与维护效率

- 易测试性：内置命令行工具，支持单硬件/全硬件功能快速验证

- 可扩展性：新增硬件时仅需新增对应模块，无需修改现有核心代码

## 1.2 适用场景

- 运行环境：嵌入式 Linux 系统（i.MX6ULL）

- 硬件范围：蜂鸣器、LED、按键、摄像头、IMU、气压计、温湿度传感器

- 上层使用者：硬件服务化进程、轻量级应用程序、硬件调试人员

## 1.3 核心约束

- 开发语言：C++14，使用现代 C++ 特性（类、命名空间、enum class 等），保持轻量级设计

- 运行态约束：仅依赖 Linux 系统调用（open/ioctl/read/write 等），无 RTOS 或服务化依赖

- 实现范围：纯用户态封装，不涉及内核驱动开发，仅对接 /dev 设备节点

- 输出形式：编译为 libbsp.so 动态库，配合总头文件对外提供接口

- 错误处理：提供统一错误码体系（enum class），所有接口返回值可追溯硬件操作状态

# 2. 整体架构

## 2.1 分层逻辑

采用「命令行工具层 → API 封装层 → 硬件驱动适配层 → 公共基础层」的四层架构，各层职责清晰、低耦合，具体分层关系如下：

命令行工具层：解析用户指令，调用 API 层接口完成硬件调试

API 封装层：对外暴露标准化硬件操作接口，屏蔽底层适配差异

硬件驱动适配层：对接 Linux 设备节点，实现具体硬件的读写与配置

公共基础层：提供全库共用的工具函数（日志、错误处理、通用工具等）

## 2.2 模块划分

按硬件类型拆分独立模块，所有硬件模块统一归类至 driver 目录下，每个硬件单独一个文件夹（含 .h 头文件和 .c 源文件）；模块间无直接依赖，仅通过公共基础层交互，具体模块划分如下：

|模块路径|包含硬件|核心接口示例|
|---|---|---|
|src/driver/led/|LED|Led 类：init()、setState()、turnOn()、turnOff()|
|src/driver/beep/|蜂鸣器|Beep 类：init()、play()、stop()|
|src/driver/key/|按键|Key 类：init()、readState()、registerCallback()|
|src/driver/camera/|摄像头|Camera 类：init()、capture()、setResolution()|
|src/driver/imu/|IMU|Imu 类：init()、readData()、setSampleRate()|
|src/driver/barometer/|气压计|Barometer 类：init()、readData()|
|src/driver/temp_hum/|温湿度传感器|TempHum 类：init()、readData()|
|src/common/|公共工具|Logger 类、ErrorCode 枚举、errorToString()|
|src/cli/|命令行工具|CliParser 类：parse()、printHelp()、printVersion()|
# 3. 详细设计

## 3.1 目录结构

遵循「头文件与源文件同目录、简化层级」的原则，目录结构如下：

```plaintext

lib_bsp/
├── src/                     # 所有源码目录
│   ├── driver/              # 硬件驱动适配层（所有硬件模块集中在此）
│   │   ├── led/             # LED 模块
│   │   │   ├── led.h        # LED 模块头文件
│   │   │   └── led.cpp      # LED 模块源文件
│   │   ├── beep/            # 蜂鸣器模块
│   │   │   ├── beep.h
│   │   │   └── beep.cpp
│   │   ├── key/             # 按键模块
│   │   │   ├── key.h
│   │   │   └── key.cpp
│   │   ├── camera/          # 摄像头模块
│   │   │   ├── camera.h
│   │   │   └── camera.cpp
│   │   ├── imu/             # IMU 模块
│   │   │   ├── imu.h
│   │   │   └── imu.cpp
│   │   ├── barometer/       # 气压计模块
│   │   │   ├── barometer.h
│   │   │   └── barometer.cpp
│   │   └── temp_hum/        # 温湿度传感器模块
│   │       ├── temp_hum.h
│   │       └── temp_hum.cpp
│   ├── common/              # 公共基础层
│   │   ├── bsp_common.h     # 公共定义（错误码、日志类、类型别名）
│   │   ├── log.cpp          # 日志功能实现
│   │   ├── error.cpp        # 错误处理实现
│   │   └── utils.cpp        # 通用工具函数
│   └── cli/                 # 命令行测试工具层
│       ├── cli_parser.h     # 指令解析头文件
│       ├── cli_parser.cpp   # 指令解析实现
│       └── bsp_tool_main.cpp # 命令行工具入口
├── test/                    # 自动化测试用例
│   ├── test_led.cpp
│   ├── test_imu.cpp
│   ├── test_camera.cpp
│   └── test_all.sh          # 全硬件批量测试脚本
├── bsp.h                    # 总对外头文件（汇总所有硬件模块接口）
├── CMakeLists.txt           # CMake 编译脚本（编译动态库+命令行工具）
├── README.md                # 仓库说明（含编译流程、使用文档、接口说明）
└── LICENSE                  # 协议文件（企业内部使用可自定义）
    
```

## 3.2 核心设计规范

### 3.2.1 接口设计原则

- 面向对象设计：所有硬件模块使用 C++ 类封装，如 `Led`、`Beep`、`Imu` 等，通过类的成员函数提供操作接口；

- 统一命名风格：类名使用 PascalCase（如 `Led`、`Beep`），成员函数使用 camelCase（如 `setState()`、`readData()`）；

- 统一返回值：所有接口返回 `ErrorCode` 枚举类型（定义在 bsp_common.h），`ErrorCode::Ok` 表示成功，其他值表示具体错误类型；

- RAII 原则：硬件设备类在构造时初始化设备名，通过 `init()` 方法打开设备，析构时自动释放资源；

- 移动语义支持：所有硬件类支持移动构造和移动赋值，禁止拷贝构造和拷贝赋值，避免资源重复释放；

- 精简参数：接口参数仅保留必要项，复杂配置通过结构体或配置类封装传递（如 `CameraConfig`）。

### 3.2.2 错误码设计（统一可扩展）

```cpp
// 定义在 src/common/bsp_common.h
namespace bsp {
    enum class ErrorCode {
        Ok = 0,                    // 操作成功
        InvalidParam = -1,        // 无效参数（空指针、非法设备名等）
        DevOpen = -2,             // 设备节点打开失败（权限不足、设备不存在）
        DevIo = -3,               // 设备读写/控制失败（write/read/ioctl 出错）
        DevNotReady = -4,         // 设备未初始化或未就绪
        MemAlloc = -5,            // 内存分配失败
        Unsupported = -6         // 不支持的操作（如非法分辨率配置）
    };
    
    // 错误码转字符串
    std::string errorToString(ErrorCode err);
}
```

### 3.2.3 总头文件设计（bsp.h）

根目录的 bsp.h 是对外暴露的唯一头文件，汇总所有硬件模块的接口声明和公共定义，上层使用者仅需包含此文件即可调用所有硬件接口，无需逐个包含子模块头文件。所有接口都在 `bsp` 命名空间下。示例如下：

```cpp
#ifndef BSP_H
#define BSP_H

// 引入公共定义（错误码、日志类等）
#include "common/bsp_common.h"

// 引入各硬件模块接口声明
#include "driver/led/led.h"
#include "driver/beep/beep.h"
#include "driver/key/key.h"
#include "driver/camera/camera.h"
#include "driver/imu/imu.h"
#include "driver/barometer/barometer.h"
#include "driver/temp_hum/temp_hum.h"

#endif // BSP_H
```

### 3.2.4 日志设计（分级可控）

- 日志分级：支持 DEBUG（调试）、INFO（普通信息）、WARN（警告）、ERROR（错误）四个级别；

- 运行时控制：通过 `Logger::setLevel()` 方法动态设置日志级别，也可通过 CMake 选项 `BSP_LOG_LEVEL_DEBUG` 控制编译时默认级别；

- 日志格式：统一为「[模块名][级别] 文件名:行号 - 日志内容」，示例：[LED][ERROR] led.cpp:58 - open /dev/led0 failed

- 使用方式：通过宏 `BSP_LOG_DEBUG()`、`BSP_LOG_INFO()`、`BSP_LOG_WARN()`、`BSP_LOG_ERROR()` 输出日志

## 3.3 编译设计

### 3.3.1 编译输出

- 动态库：编译生成 libbsp.so，输出至 build/lib/ 目录（CMake 自动创建）；

- 命令行工具：编译生成 bsp_tool 可执行文件，输出至 build/bin/ 目录（CMake 自动创建）。

### 3.3.2 核心编译目标（CMake）

```bash
# 创建构建目录
mkdir build && cd build

# 配置 CMake（Debug 模式，启用调试日志）
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBSP_LOG_LEVEL_DEBUG=ON

# 或 Release 模式（关闭调试日志）
cmake .. -DCMAKE_BUILD_TYPE=Release -DBSP_LOG_LEVEL_DEBUG=OFF

# 编译
cmake --build .

# 安装（需要 root 权限）
sudo cmake --install .

# 运行测试
ctest
# 或直接运行
./bin/test_led
```

### 3.3.3 编译依赖

- CMake 3.10 或更高版本
- C++11 兼容的编译器（g++ 4.8+ 或 clang++ 3.3+）
- 仅依赖 Linux 系统库（libc.so、libpthread.so），无第三方依赖
- 适配嵌入式交叉编译环境（可通过 CMake 工具链文件指定交叉编译器）

## 3.4 命令行工具设计

bsp_tool 支持指令式调用，无需交互式操作，可直接在脚本中集成，核心功能及使用示例如下：

```bash

# 通用格式：bsp_tool <硬件模块> <操作> [参数1] [参数2] ...

# 通用操作
bsp_tool -v  # 查看 libbsp 版本
bsp_tool -h  # 查看帮助信息（所有支持的模块、操作及参数说明）

# LED 测试
bsp_tool led set ledx on          # 打开 LED（对应/dev/ledx）
bsp_tool led set ledx off         # 关闭 LED

# AP3216C
bsp_tool ap3216c read              # 使用默认设备名 /dev/ap3216c
bsp_tool ap3216c read ap3216c      # 显式指定设备名

# 蜂鸣器测试
bsp_tool beep play 1000           # 蜂鸣器响 1000ms（1 秒）
bsp_tool beep stop                # 停止蜂鸣器

# 摄像头测试
bsp_tool camera init 1920x1080    # 初始化摄像头，分辨率 1920x1080
bsp_tool camera capture /tmp/frame.jpg  # 采集一帧图片保存至指定路径

# IMU测试
bsp_tool sensor read temp_hum     # 读取温湿度数据
bsp_tool sensor read imu          # 读取 IMU 数据（加速度、角速度）
bsp_tool sensor set-rate imu 100  # 设置 IMU 采样率为 100Hz    
```

# 4. 测试与验证

## 4.1 单元测试

每个硬件模块对应独立的单元测试用例（如 test_led.c、test_imu.c），覆盖以下场景：

- 参数异常测试：传入空指针、非法设备名、无效句柄等；

- 设备异常测试：设备节点不存在、权限不足、设备未初始化等；

- 正常功能测试：验证初始化、核心操作、资源释放的正确性。

## 4.2 集成测试

提供 test_all.sh 批量测试脚本，自动遍历所有硬件模块的核心功能，输出结构化测试报告，包含：测试模块、测试用例、测试结果（通过/失败）、错误信息（失败时）。

## 4.3 手动测试

通过 bsp_tool 命令行工具手动验证硬件功能，适合开发过程中的快速调试和问题定位，示例如下：

```bash

# 验证 LED0 点亮
bsp_tool led set led0 on  # 观察硬件 LED0 是否点亮
# 验证温湿度传感器数据
bsp_tool sensor read temp_hum  # 查看输出数据是否在合理范围
# 验证摄像头采集
bsp_tool camera init 1280x720 && bsp_tool camera capture /tmp/test.jpg  # 查看 /tmp/test.jpg 是否生成且可正常查看
    
```

# 5. 版本规划

|版本|核心功能|交付物|
|---|---|---|
|v1.0|基础硬件接口封装（LED）；基础命令行工具功能|libbsp.so、bsp_tool、基础测试用例|
|v1.1|新增蜂鸣器、按键、摄像头、IMU、气压计接口封装；完善命令行工具功能|扩展版 libbsp.so、完善版 bsp_tool、全量测试用例|
|v1.2|优化稳定性（新增超时处理、设备重连机制）；优化日志输出；支持交叉编译|稳定版 libbsp.so、优化版 bsp_tool、完整测试报告、交叉编译脚本|
# 6. 总结

## 6.1 核心优势

- 结构简洁：目录层级扁平化，头文件与源文件同目录，降低维护成本；

- 接口统一：标准化的命名、返回值、句柄管理，上层调用门槛低；

- 独立可测：命令行工具支持脱离上层服务快速调试，问题定位高效；

- 可扩展性强：新增硬件仅需新增 driver 子模块，无侵入式修改。

## 6.2 关键约束

- 仅封装用户态硬件操作，不涉及内核驱动开发；

- 无服务化、IPC 等非硬件相关逻辑，保持库的轻量性；

- 接口发布后需保持向后兼容，避免上层适配成本增加。

## 6.3 后续扩展

- 新增硬件：在 src/driver/ 下新增对应硬件文件夹（含 .h 和 .c），在 bsp.h 中添加接口声明即可；

- 功能扩展：可在不修改现有接口的前提下，新增高级功能接口（如 LED 呼吸灯模式）；

- 适配扩展：支持更多嵌入式 Linux 架构（如 RISC-V），可通过 Makefile 新增交叉编译配置。

