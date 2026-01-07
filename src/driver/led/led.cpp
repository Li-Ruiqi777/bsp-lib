#include "led.h"
#include "../../common/bsp_common.h"
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

namespace bsp
{

Led::Led(const std::string &dev_name) : dev_name_(dev_name), fd_(-1), initialized_(false)
{
    dev_path_ = "/dev/" + dev_name_;
}

Led::~Led()
{
    cleanup();
}

Led::Led(Led &&other) noexcept
    : dev_name_(std::move(other.dev_name_)), dev_path_(std::move(other.dev_path_)), fd_(other.fd_),
      initialized_(other.initialized_)
{
    other.fd_ = -1;
    other.initialized_ = false;
}

Led &Led::operator=(Led &&other) noexcept
{
    if (this != &other)
    {
        cleanup();
        dev_name_ = std::move(other.dev_name_);
        dev_path_ = std::move(other.dev_path_);
        fd_ = other.fd_;
        initialized_ = other.initialized_;
        other.fd_ = -1;
        other.initialized_ = false;
    }
    return *this;
}

ErrorCode Led::init()
{
    if (initialized_)
    {
        BSP_LOG_WARN("LED", "Device %s already initialized", dev_name_.c_str());
        return ErrorCode::Ok;
    }

    // 打开设备节点
    fd_ = open(dev_path_.c_str(), O_RDWR);
    if (fd_ < 0)
    {
        BSP_LOG_ERROR("LED", "open %s failed", dev_path_.c_str());
        return ErrorCode::DevOpen;
    }

    initialized_ = true;
    BSP_LOG_INFO("LED", "init %s success", dev_name_.c_str());
    return ErrorCode::Ok;
}

ErrorCode Led::setState(bool on)
{
    if (!initialized_ || fd_ < 0)
    {
        BSP_LOG_ERROR("LED", "%s not ready (not initialized)", dev_name_.c_str());
        return ErrorCode::DevNotReady;
    }

    // 写入状态
    int ret = 1;
    if (on)
        ret = ioctl(this->fd_, LED_ON);
    else
        ret = ioctl(this->fd_, LED_OFF);
    if (ret == -1)
    {
        BSP_LOG_ERROR("LED", "set %s state failed (on=%d)", dev_name_.c_str(), on);
        return ErrorCode::DevIo;
    }

    BSP_LOG_DEBUG("LED", "set %s to %s", dev_name_.c_str(), on ? "on" : "off");
    return ErrorCode::Ok;
}

ErrorCode Led::turnOn()
{
    return setState(true);
}

ErrorCode Led::turnOff()
{
    return setState(false);
}

bool Led::isReady() const
{
    return initialized_ && fd_ >= 0;
}

std::string Led::getDeviceName() const
{
    return dev_name_;
}

void Led::cleanup()
{
    if (fd_ >= 0)
    {
        close(fd_);
        fd_ = -1;
    }
    initialized_ = false;
}

} // namespace bsp
