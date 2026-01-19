#include "dht11.h"
#include <spdlog/spdlog.h>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <spdlog/spdlog.h>
#include <unistd.h>

namespace bsp
{

DHT11::DHT11(const std::string &devName)
    : devName(devName), fd(-1), initialized(false)
{
    devPath = "/dev/" + devName;
}

DHT11::~DHT11()
{
    cleanup();
}

DHT11::DHT11(DHT11 &&other) noexcept
    : devName(std::move(other.devName)), devPath(std::move(other.devPath)), fd(other.fd),
      initialized(other.initialized)
{
    other.fd = -1;
    other.initialized = false;
}

DHT11 &DHT11::operator=(DHT11 &&other) noexcept
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

ErrorCode DHT11::init()
{
    if (initialized)
    {
        spdlog::warn("Device {} already initialized", devName);
        return ErrorCode::Ok;
    }

    // 打开设备节点
    fd = open(devPath.c_str(), O_RDONLY);
    if (fd < 0)
    {
        spdlog::error("open {} failed", devPath);
        return ErrorCode::DevOpen;
    }

    initialized = true;
    spdlog::info("init {} success", devName);
    return ErrorCode::Ok;
}

ErrorCode DHT11::readData(DHT11Data &data)
{
    if (!initialized || fd < 0)
    {
        spdlog::error("{} not ready (not initialized)", devName);
        return ErrorCode::DevNotReady;
    }

    // 读取4个字节的数据：湿度整数、湿度小数、温度整数、温度小数
    uint8_t rawData[4] = {0};
    ssize_t n = read(fd, rawData, sizeof(rawData));

    if (n < 0)
    {
        spdlog::error("read from {} failed", devName);
        return ErrorCode::DevIo;
    }

    if (n != sizeof(rawData))
    {
        spdlog::error("read size mismatch from {}: expected {}, got {}",
                     devName, sizeof(rawData), n);
        return ErrorCode::DevIo;
    }

    // 填充数据结构
    data.humidity_int = rawData[0];
    data.humidity_decimal = rawData[1];
    data.temperature_int = rawData[2];
    data.temperature_decimal = rawData[3];

    spdlog::debug("Read from {} - Humidity: {}.{}% RH, Temperature: {}.{}°C",
                  devName, data.humidity_int, data.humidity_decimal,
                  data.temperature_int, data.temperature_decimal);

    return ErrorCode::Ok;
}

bool DHT11::isReady() const
{
    return initialized && fd >= 0;
}

std::string DHT11::getDeviceName() const
{
    return devName;
}

void DHT11::cleanup()
{
    if (fd >= 0)
    {
        close(fd);
        fd = -1;
    }
    initialized = false;
}

} // namespace bsp

