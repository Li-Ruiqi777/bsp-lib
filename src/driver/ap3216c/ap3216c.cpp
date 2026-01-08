#include "ap3216c.h"
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

namespace bsp
{

AP3216C::AP3216C(const std::string &devName)
    : devName(devName), fd(-1), initialized(false)
{
    devPath = "/dev/" + devName;
}

AP3216C::~AP3216C()
{
    cleanup();
}

AP3216C::AP3216C(AP3216C &&other) noexcept
    : devName(std::move(other.devName)), devPath(std::move(other.devPath)), fd(other.fd),
      initialized(other.initialized)
{
    other.fd = -1;
    other.initialized = false;
}

AP3216C &AP3216C::operator=(AP3216C &&other) noexcept
{
    if (this != &other)
    {
        cleanup();
        devName = std::move(other.devName);
        devPath = std::move(other.devPath);
        fd = other.fd;
        initialized = other.initialized;
        other.fd = -1;
        other.initialized = false;
    }
    return *this;
}

ErrorCode AP3216C::init()
{
    if (initialized)
    {
        BSP_LOG_WARN("AP3216C", "Device %s already initialized", devName.c_str());
        return ErrorCode::Ok;
    }

    // 打开设备节点
    fd = open(devPath.c_str(), O_RDWR);
    if (fd < 0)
    {
        BSP_LOG_ERROR("AP3216C", "open %s failed", devPath.c_str());
        return ErrorCode::DevOpen;
    }

    initialized = true;
    BSP_LOG_INFO("AP3216C", "init %s success", devName.c_str());
    return ErrorCode::Ok;
}

ErrorCode AP3216C::readData(AP3216CData &data)
{
    if (!initialized || fd < 0)
    {
        BSP_LOG_ERROR("AP3216C", "%s not ready (not initialized)", devName.c_str());
        return ErrorCode::DevNotReady;
    }

    // 读取3个 uint16_t 数据
    uint16_t rawData[3] = {0};
    ssize_t n = read(fd, rawData, sizeof(rawData));

    if (n != sizeof(rawData))
    {
        BSP_LOG_ERROR("AP3216C", "read size mismatch from %s: expected %zu, got %zd",
                     devName.c_str(), sizeof(rawData), n);
        return ErrorCode::DevIo;
    }

    // 填充数据结构
    data.ir = rawData[0];
    data.als = rawData[1];
    data.ps = rawData[2];

    BSP_LOG_DEBUG("AP3216C", "Read from %s - IR: %u, ALS: %u, PS: %u",
                  devName.c_str(), data.ir, data.als, data.ps);

    return ErrorCode::Ok;
}

bool AP3216C::isReady() const
{
    return initialized && fd >= 0;
}

std::string AP3216C::getDeviceName() const
{
    return devName;
}

void AP3216C::cleanup()
{
    if (fd >= 0)
    {
        close(fd);
        fd = -1;
    }
    initialized = false;
}

} // namespace bsp
