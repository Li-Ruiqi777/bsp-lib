#ifndef BSP_COMMON_H
#define BSP_COMMON_H

#include <cstdint>
#include <string>

namespace bsp
{

// 错误码定义
enum class ErrorCode
{
    Ok = 0,            // 操作成功
    InvalidParam = -1, // 无效参数（空指针、非法设备名等）
    DevOpen = -2,      // 设备节点打开失败（权限不足、设备不存在）
    DevIo = -3,        // 设备读写/控制失败（write/read/ioctl 出错）
    DevNotReady = -4,  // 设备未初始化或未就绪
    MemAlloc = -5,     // 内存分配失败
    Unsupported = -6   // 不支持的操作（如非法分辨率配置）
};

// 错误码转字符串
std::string errorToString(ErrorCode err);

// 版本信息
constexpr int VERSION_MAJOR = 1;
constexpr int VERSION_MINOR = 0;
constexpr int VERSION_PATCH = 0;

} // namespace bsp

#endif // BSP_COMMON_H
