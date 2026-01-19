#ifndef BSP_DHT11_H
#define BSP_DHT11_H

#include <string>
#include <cstdint>
#include "../../common/bsp_common.h"

namespace bsp
{

/**
 * @brief DHT11 温湿度传感器数据结构
 */
struct DHT11Data
{
    uint8_t humidity_int;    // 湿度整数部分
    uint8_t humidity_decimal; // 湿度小数部分（DHT11通常为0）
    uint8_t temperature_int;  // 温度整数部分
    uint8_t temperature_decimal; // 温度小数部分（DHT11通常为0）
};

/**
 * @brief DHT11 温湿度传感器类
 *
 * 支持读取温度和湿度数据
 */
class DHT11
{
public:
    /**
     * @brief 构造函数
     * @param devName DHT11 设备名（如 "dht11"，对应 /dev/dht11）
     */
    explicit DHT11(const std::string &devName = "dht11");

    /**
     * @brief 析构函数，自动释放资源
     */
    ~DHT11();

    // 禁止拷贝构造和赋值
    DHT11(const DHT11 &) = delete;
    DHT11 &operator=(const DHT11 &) = delete;

    // 允许移动构造和赋值
    DHT11(DHT11 &&other) noexcept;
    DHT11 &operator=(DHT11 &&other) noexcept;

    /**
     * @brief 初始化 DHT11 设备
     * @return ErrorCode::Ok 成功，其他错误码失败
     */
    ErrorCode init();

    /**
     * @brief 读取一次传感器数据
     * @param data 传感器数据结构体引用，用于存储读取的数据
     * @return ErrorCode::Ok 成功，其他错误码失败
     */
    ErrorCode readData(DHT11Data &data);

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

#endif // BSP_DHT11_H

