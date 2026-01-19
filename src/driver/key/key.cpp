#include "key.h"
#include "common/bsp_common.h"
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>

namespace bsp
{

Key::Key(const std::string &devName)
    : devName(devName), fd(-1), running(false), initialized(false),
      lastKeyCode(-1), lastKeyPressed(false), longPressReported(false)
{
    devPath = "/dev/" + devName;
}

Key::~Key()
{
    stop();
    cleanup();
}

Key::Key(Key &&other) noexcept
    : devName(std::move(other.devName)), devPath(std::move(other.devPath)), fd(other.fd),
      running(other.running.load()), initialized(other.initialized.load()),
      callback(std::move(other.callback)), lastKeyCode(other.lastKeyCode),
      lastKeyPressed(other.lastKeyPressed), lastPressTime(other.lastPressTime),
      longPressReported(other.longPressReported)
{
    other.fd = -1;
    other.running = false;
    other.initialized = false;
}

Key &Key::operator=(Key &&other) noexcept
{
    if (this != &other)
    {
        stop();
        cleanup();
        devName = std::move(other.devName);
        devPath = std::move(other.devPath);
        fd = other.fd;
        initialized = other.initialized.load();
        running = other.running.load();
        callback = std::move(other.callback);
        lastKeyCode = other.lastKeyCode;
        lastKeyPressed = other.lastKeyPressed;
        lastPressTime = other.lastPressTime;
        longPressReported = other.longPressReported;
        other.fd = -1;
        other.running = false;
        other.initialized = false;
    }
    return *this;
}

ErrorCode Key::init()
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

ErrorCode Key::start()
{
    if (!initialized || fd < 0)
    {
        spdlog::error("{} not ready (not initialized)", devName);
        return ErrorCode::DevNotReady;
    }

    if (running)
    {
        spdlog::warn("Device {} already running", devName);
        return ErrorCode::Ok;
    }

    running = true;
    try
    {
        eventThread = std::thread(&Key::eventLoop, this);
        spdlog::info("start {} success", devName);
        return ErrorCode::Ok;
    }
    catch (const std::exception &e)
    {
        running = false;
        spdlog::error("Failed to start event loop: {}", e.what());
        return ErrorCode::DevIo;
    }
}

ErrorCode Key::stop()
{
    if (!running)
    {
        return ErrorCode::Ok;
    }

    running = false;

    if (eventThread.joinable())
    {
        eventThread.join();
    }

    spdlog::info("stop {} success", devName);
    return ErrorCode::Ok;
}

bool Key::isReady() const
{
    return initialized && fd >= 0;
}

bool Key::isRunning() const
{
    return running;
}

void Key::setCallback(KeyCallback cb)
{
    callback = cb;
}

std::string Key::getDeviceName() const
{
    return devName;
}

void Key::eventLoop()
{
    struct input_event event;

    spdlog::debug("Event loop started for {}", devName);

    while (running)
    {
        ssize_t n = read(fd, &event, sizeof(struct input_event));

        if (n < 0)
        {
            if (running)
            {
                spdlog::error("read from {} failed", devName);
            }
            break;
        }

        if (n != sizeof(struct input_event))
        {
            spdlog::warn("read size mismatch from {}", devName);
            continue;
        }

        // 只处理按键事件
        if (event.type == EV_KEY)
        {
            spdlog::debug("Event from {} - code: {}, value: {}", devName,
                          event.code, event.value);

            // 按键按下时，记录按下信息
            if (event.value == 1)
            {
                lastKeyCode = event.code;
                lastKeyPressed = true;
                lastPressTime = std::chrono::system_clock::now();
                longPressReported = false;
                // 报告按下事件
                if (callback)
                {
                    callback(event.code, 1);
                }
            }
            // 按键释放时，检查是否为长按
            else if (event.value == 0)
            {
                lastKeyPressed = false;
                if (lastKeyCode == event.code)
                {
                    auto now = std::chrono::system_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPressTime);

                    // 如果按键按下时长超过阈值，报告为长按，否则报告为普通释放
                    if (duration.count() >= LONG_PRESS_THRESHOLD_MS && !longPressReported)
                    {
                        // 长按，报告长按事件（值为2）
                        if (callback)
                        {
                            callback(event.code, 2);
                            spdlog::debug("Long press detected - code: {}, duration: {} ms",
                                        event.code, duration.count());
                        }
                    }
                    else if (!longPressReported && callback)
                    {
                        // 短按，报告释放事件（值为0）
                        callback(event.code, 0);
                    }
                }
            }
        }
    }

    spdlog::debug("Event loop ended for {}", devName);
}

void Key::cleanup()
{
    if (fd >= 0)
    {
        close(fd);
        fd = -1;
    }
    initialized = false;
}

} // namespace bsp
