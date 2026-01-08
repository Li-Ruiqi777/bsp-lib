#include "key.h"
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
        BSP_LOG_WARN("KEY", "Device %s already initialized", devName.c_str());
        return ErrorCode::Ok;
    }

    // 打开设备节点
    fd = open(devPath.c_str(), O_RDONLY);
    if (fd < 0)
    {
        BSP_LOG_ERROR("KEY", "open %s failed", devPath.c_str());
        return ErrorCode::DevOpen;
    }

    initialized = true;
    BSP_LOG_INFO("KEY", "init %s success", devName.c_str());
    return ErrorCode::Ok;
}

ErrorCode Key::start()
{
    if (!initialized || fd < 0)
    {
        BSP_LOG_ERROR("KEY", "%s not ready (not initialized)", devName.c_str());
        return ErrorCode::DevNotReady;
    }

    if (running)
    {
        BSP_LOG_WARN("KEY", "Device %s already running", devName.c_str());
        return ErrorCode::Ok;
    }

    running = true;
    try
    {
        eventThread = std::thread(&Key::eventLoop, this);
        BSP_LOG_INFO("KEY", "start %s success", devName.c_str());
        return ErrorCode::Ok;
    }
    catch (const std::exception &e)
    {
        running = false;
        BSP_LOG_ERROR("KEY", "Failed to start event loop: %s", e.what());
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

    BSP_LOG_INFO("KEY", "stop %s success", devName.c_str());
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

    BSP_LOG_DEBUG("KEY", "Event loop started for %s", devName.c_str());

    while (running)
    {
        ssize_t n = read(fd, &event, sizeof(struct input_event));

        if (n < 0)
        {
            if (running)
            {
                BSP_LOG_ERROR("KEY", "read from %s failed", devName.c_str());
            }
            break;
        }

        if (n != sizeof(struct input_event))
        {
            BSP_LOG_WARN("KEY", "read size mismatch from %s", devName.c_str());
            continue;
        }

        // 只处理按键事件
        if (event.type == EV_KEY)
        {
            BSP_LOG_DEBUG("KEY", "Event from %s - code: %d, value: %d", devName.c_str(),
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
                            BSP_LOG_DEBUG("KEY", "Long press detected - code: %d, duration: %ld ms",
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

    BSP_LOG_DEBUG("KEY", "Event loop ended for %s", devName.c_str());
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
