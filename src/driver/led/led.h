#ifndef LED_H
#define LED_H

#include "../../common/bsp_common.h"
#include <string>
#include <sys/ioctl.h> //ioctl() 声明和 _IO 系列宏

namespace bsp
{

#define LED_MAGIC 'L'
#define LED_ON    _IO(LED_MAGIC, 0)
#define LED_OFF   _IO(LED_MAGIC, 1)

/**
 * @brief LED 设备类
 *
 * 用于控制 LED 设备的打开和关闭
 */
class Led
{
public:
    /**
     * @brief 构造函数
     * @param dev_name LED 设备名（如 "led0"，对应 /dev/led0）
     */
    explicit Led(const std::string &dev_name);

    /**
     * @brief 析构函数，自动释放资源
     */
    ~Led();

    // 禁止拷贝构造和赋值
    Led(const Led &) = delete;
    Led &operator=(const Led &) = delete;

    // 允许移动构造和赋值
    Led(Led &&other) noexcept;
    Led &operator=(Led &&other) noexcept;

    /**
     * @brief 初始化 LED 设备
     * @return ErrorCode::Ok 成功，其他错误码失败
     */
    ErrorCode init();

    /**
     * @brief 设置 LED 状态
     * @param on 状态（true-打开，false-关闭）
     * @return ErrorCode::Ok 成功，其他错误码失败
     */
    ErrorCode setState(bool on);

    /**
     * @brief 打开 LED
     * @return ErrorCode::Ok 成功，其他错误码失败
     */
    ErrorCode turnOn();

    /**
     * @brief 关闭 LED
     * @return ErrorCode::Ok 成功，其他错误码失败
     */
    ErrorCode turnOff();

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
    std::string dev_name_;
    std::string dev_path_;
    int fd_;
    bool initialized_;

    void cleanup();
};

} // namespace bsp

#endif // LED_H
