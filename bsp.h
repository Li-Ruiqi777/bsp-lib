#ifndef BSP_H
#define BSP_H

// 引入公共定义（错误码、日志级别、版本信息等）
#include "src/common/bsp_common.h"

// 引入各硬件模块接口声明（v1.0 版本仅包含 LED）
#include "src/driver/led/led.h"

// v1.1 版本将包含以下模块：
// #include "src/driver/beep/beep.h"
// #include "src/driver/key/key.h"
// #include "src/driver/camera/camera.h"
// #include "src/driver/imu/imu.h"
// #include "src/driver/barometer/barometer.h"
// #include "src/driver/temp_hum/temp_hum.h"

#endif // BSP_H
