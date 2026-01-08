#ifndef BSP_KEY_H
#define BSP_KEY_H

#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <chrono>
#include <linux/input.h>
#include "common/bsp_common.h"

namespace bsp
{

class Key
{
public:
    using KeyCallback = std::function<void(int code, int value)>;

    // 长按检测阈值(ms)
    static constexpr int LONG_PRESS_THRESHOLD_MS = 500;

    explicit Key(const std::string &devName = "input/event2");
    ~Key();

    // 禁止拷贝
    Key(const Key &) = delete;
    Key &operator=(const Key &) = delete;

    // 支持移动语义
    Key(Key &&other) noexcept;
    Key &operator=(Key &&other) noexcept;

    ErrorCode init();
    ErrorCode start();
    ErrorCode stop();
    bool isReady() const;
    bool isRunning() const;

    void setCallback(KeyCallback cb);
    std::string getDeviceName() const;

private:
    void eventLoop();
    void cleanup();

    std::string devName;
    std::string devPath;
    int fd;
    std::atomic<bool> running;
    std::atomic<bool> initialized;
    std::thread eventThread;
    KeyCallback callback;

    // 长按检测相关
    int lastKeyCode;
    bool lastKeyPressed;
    std::chrono::system_clock::time_point lastPressTime;
    bool longPressReported;
};

} // namespace bsp

#endif // BSP_KEY_H
