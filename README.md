# bsp-lib

BSP Library - 嵌入式 Linux 系统硬件抽象层库（C++ 实现）

为嵌入式 Linux 系统构建统一、标准化、可复用的板级支持包（BSP）硬件抽象库，封装硬件设备的底层操作接口；同时提供轻量级命令行测试工具，支持脱离服务化逻辑的硬件快速验证与调试。

## 版本信息

当前版本：**v1.0**

- ✅ 基础硬件接口封装（LED）
- ✅ 基础命令行工具功能
- ✅ 基础测试用例

## 特性

- **接口标准化**：不同硬件提供统一风格的调用接口，降低上层使用成本
- **结构轻量化**：简化目录层级，避免冗余嵌套，提升开发与维护效率
- **易测试性**：内置命令行工具，支持单硬件/全硬件功能快速验证
- **可扩展性**：新增硬件时仅需新增对应模块，无需修改现有核心代码
- **现代 C++**：使用 C++11 标准，面向对象设计，支持移动语义

## 目录结构

```
bsp-lib/
├── src/                     # 所有源码目录
│   ├── driver/              # 硬件驱动适配层
│   │   └── led/             # LED 模块
│   │       ├── led.h
│   │       └── led.cpp
│   ├── common/              # 公共基础层
│   │   ├── bsp_common.h     # 公共定义（错误码、日志类、类型别名）
│   │   ├── log.cpp          # 日志功能实现
│   │   ├── error.cpp        # 错误处理实现
│   │   └── utils.cpp        # 通用工具函数
│   └── cli/                 # 命令行测试工具层
│       ├── cli_parser.h
│       ├── cli_parser.cpp
│       └── bsp_tool_main.cpp
├── test/                    # 自动化测试用例
│   └── test_led.cpp
├── bsp.h                    # 总对外头文件
├── CMakeLists.txt           # CMake 编译脚本
└── README.md                # 本文件
```

## 编译

### 环境要求

- CMake 3.10 或更高版本
- C++11 兼容的编译器（g++ 4.8+ 或 clang++ 3.3+）
- Linux 系统

### 编译步骤

```bash
# 创建构建目录
mkdir build && cd build

# 配置 CMake（Debug 模式，启用调试日志）
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBSP_LOG_LEVEL_DEBUG=ON

# 或 Release 模式（关闭调试日志）
cmake .. -DCMAKE_BUILD_TYPE=Release -DBSP_LOG_LEVEL_DEBUG=OFF

# 编译
cmake --build .

# 安装到系统（需要 root 权限）
sudo cmake --install .

# 运行测试
ctest
# 或直接运行
./bin/test_led
```

### 交叉编译

```bash
# 使用交叉编译工具链
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake \
         -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## 使用说明

### 作为库使用

在代码中包含总头文件：

```cpp
#include "bsp.h"
#include <iostream>

using namespace bsp;

int main() {
    // 创建 LED 对象
    Led led("led0");
    
    // 初始化设备
    ErrorCode ret = led.init();
    if (ret != ErrorCode::Ok) {
        std::cerr << "LED init failed: " 
                  << errorToString(ret) << std::endl;
        return 1;
    }
    
    // 控制 LED
    led.turnOn();
    // ... 其他操作
    led.turnOff();
    
    // 析构函数自动释放资源
    return 0;
}
```

编译时链接动态库：

```bash
g++ -std=c++11 -o myapp myapp.cpp -L./build/lib -lbsp -Wl,-rpath,./build/lib
```

### 命令行工具使用

```bash
# LED 测试
bsp_tool led set led0 on          # 打开 LED0
bsp_tool led set led0 off          # 关闭 LED0

# 查看帮助
bsp_tool -h

# 查看版本
bsp_tool -v
```

## API 文档

### LED 模块

#### `Led` 类

LED 设备控制类。

**构造函数：**
```cpp
explicit Led(const std::string& dev_name);
```

**成员函数：**

- `ErrorCode init()` - 初始化 LED 设备
- `ErrorCode setState(bool on)` - 设置 LED 状态（true-打开，false-关闭）
- `ErrorCode turnOn()` - 打开 LED
- `ErrorCode turnOff()` - 关闭 LED
- `bool isReady() const` - 检查设备是否已初始化
- `std::string getDeviceName() const` - 获取设备名

**返回值：**
- `ErrorCode::Ok` - 成功
- `ErrorCode::InvalidParam` - 无效参数
- `ErrorCode::DevOpen` - 设备打开失败
- `ErrorCode::DevNotReady` - 设备未就绪
- `ErrorCode::DevIo` - 设备 I/O 失败

**特性：**
- 支持移动语义（移动构造和移动赋值）
- 禁止拷贝（拷贝构造和拷贝赋值被删除）
- RAII：析构函数自动释放资源

### 错误码

所有接口返回 `ErrorCode` 枚举类型：

- `ErrorCode::Ok` - 操作成功
- `ErrorCode::InvalidParam` - 无效参数
- `ErrorCode::DevOpen` - 设备节点打开失败
- `ErrorCode::DevIo` - 设备读写/控制失败
- `ErrorCode::DevNotReady` - 设备未初始化或未就绪
- `ErrorCode::MemAlloc` - 内存分配失败
- `ErrorCode::Unsupported` - 不支持的操作

使用 `errorToString()` 函数可以将错误码转换为可读字符串。

### 日志系统

日志系统通过 `Logger` 类提供：

```cpp
// 设置日志级别
bsp::Logger::setLevel(bsp::LogLevel::Info);

// 使用日志宏
BSP_LOG_DEBUG("MODULE", "Debug message: %d", value);
BSP_LOG_INFO("MODULE", "Info message");
BSP_LOG_WARN("MODULE", "Warning message");
BSP_LOG_ERROR("MODULE", "Error message: %s", error_msg);
```

日志级别：`Debug` < `Info` < `Warn` < `Error`

## 测试

### 运行单元测试

```bash
# 编译并运行测试
cd build
cmake --build .
ctest

# 或直接运行
./bin/test_led
```

### 手动测试

使用命令行工具进行手动测试：

```bash
# 测试 LED
bsp_tool led set led0 on
bsp_tool led set led0 off
```

## 开发计划

### v1.0（当前版本）
- ✅ LED 模块
- ✅ 基础命令行工具
- ✅ 基础测试用例

### v1.1（计划中）
- 蜂鸣器模块（Beep 类）
- 按键模块（Key 类）
- 摄像头模块（Camera 类）
- IMU 模块（Imu 类）
- 气压计模块（Barometer 类）
- 温湿度传感器模块（TempHum 类）

### v1.2（计划中）
- 超时处理
- 设备重连机制
- 日志优化
- 交叉编译支持增强

## 许可证

详见 [LICENSE](LICENSE) 文件。

## 贡献

欢迎提交 Issue 和 Pull Request。

## 联系方式

如有问题或建议，请通过 Issue 反馈。
