#ifndef BSP_AP3216C_H
#define BSP_AP3216C_H

#include <string>
#include <cstdint>
#include "common/bsp_common.h"

namespace bsp
{

/**
 * @brief AP3216C 环境光传感器数据结构
 */
struct AP3216CData
{
    uint16_t ir;   // 红外数据
    uint16_t als;  // 环境光强度数据
    uint16_t ps;   // 接近传感器数据（距离）
};

/**
 * @brief AP3216C 环境光传感器类
 *
 * 支持读取红外、环境光强度和距离传感器数据
 */
class AP3216C
{
public:
    /**
     * @brief 构造函数
     * @param devName AP3216C 设备名（如 "ap3216c"，对应 /dev/ap3216c）
     */
    explicit AP3216C(const std::string &devName = "ap3216c");

    /**
     * @brief 析构函数，自动释放资源
     */
    ~AP3216C();

    // 禁止拷贝构造和赋值
    AP3216C(const AP3216C &) = delete;
    AP3216C &operator=(const AP3216C &) = delete;

    // 允许移动构造和赋值
    AP3216C(AP3216C &&other) noexcept;
    AP3216C &operator=(AP3216C &&other) noexcept;

    /**
     * @brief 初始化 AP3216C 设备
     * @return ErrorCode::Ok 成功，其他错误码失败
     */
    ErrorCode init();

    /**
     * @brief 读取一次传感器数据
     * @param data 传感器数据结构体引用，用于存储读取的数据
     * @return ErrorCode::Ok 成功，其他错误码失败
     */
    ErrorCode readData(AP3216CData &data);

    /**
     * @brief 检查设备是否已初始化
     * @return true 已初始化，false 未初始化
     */
    bool isReady() const;

    /**
     * @brief 获取设备名
     * @return 设备名字符串
     */
    std::string getDeviceName() const;

private:
    std::string devName;
    std::string devPath;
    int fd;
    bool initialized;

    void cleanup();
};

} // namespace bsp

#endif // BSP_AP3216C_H
