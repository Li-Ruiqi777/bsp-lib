# bsp-lib

BSP Library - 嵌入式 Linux 系统硬件抽象层库

为嵌入式 Linux 系统构建统一、标准化、可复用的板级支持包（BSP）硬件抽象库，封装硬件设备的底层操作接口；同时提供轻量级命令行测试工具，便于硬件快速验证与调试。

## 编译

### 环境要求

- CMake 3.10 或更高版本
- C++11 兼容的编译器（g++ 4.9+ 或 clang++ 3.3+）
- Linux 系统

### 编译步骤

```bash
# 创建构建目录
mkdir build && cd build

# 配置调试日志
cmake ..  -DBSP_LOG_LEVEL_DEBUG=ON
# 或 
cmake ..  -DBSP_LOG_LEVEL_DEBUG=OFF

# 编译
make -j$nproc

# 安装
make install

# 运行测试
path_to_install/test_led
```


## 使用说明

### 作为库使用

- 在代码中包含总头文件

```cpp
#include "bsp.h"
```

- 编译时链接动态库

```bash
g++ -o myapp myapp.cpp -L./build/lib -lbsp
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
...
```

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

## 开发计划

### v1.0（当前版本）
- LED
- KEY
- AP3216C
- 基础命令行工具
- 基础测试用例

### v1.1（计划中）
- 温湿度传感器模块
- 蜂鸣器模块
- 摄像头模块
- IMU 模块
- 气压计模块

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
