#include <iostream>
#include <thread>
#include <chrono>
#include "../src/driver/key/key.h"

using namespace bsp;

void testKey()
{
    std::cout << "=== Key Driver Test ===" << std::endl;

    // 创建 Key 实例
    Key key("input/event2");

    // 设置事件回调
    key.setCallback([](int code, int value) {
        std::string state;
        if (value == 0)
            state = "released";
        else if (value == 1)
            state = "pressed";
        else if (value == 2)
            state = "held";
        else
            state = "unknown";

        std::cout << "Key Event - Code: " << code << ", State: " << state << std::endl;
    });

    // 初始化设备
    std::cout << "\n[1] Initializing device..." << std::endl;
    ErrorCode ret = key.init();
    if (ret != ErrorCode::Ok)
    {
        std::cerr << "Failed to initialize Key device: " << static_cast<int>(ret) << std::endl;
        return;
    }
    std::cout << "Device initialized: " << key.getDeviceName() << std::endl;
    std::cout << "Device ready: " << (key.isReady() ? "Yes" : "No") << std::endl;

    // 启动事件监听
    std::cout << "\n[2] Starting event listener..." << std::endl;
    ret = key.start();
    if (ret != ErrorCode::Ok)
    {
        std::cerr << "Failed to start Key event listener: " << static_cast<int>(ret) << std::endl;
        return;
    }
    std::cout << "Event listener started" << std::endl;
    std::cout << "Running: " << (key.isRunning() ? "Yes" : "No") << std::endl;

    // 监听事件 15 秒
    std::cout << "\n[3] Waiting for key events (15 seconds)..." << std::endl;
    std::cout << "Please press KEY on the device..." << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(15));

    // 停止监听
    std::cout << "\n[4] Stopping event listener..." << std::endl;
    ret = key.stop();
    if (ret != ErrorCode::Ok)
    {
        std::cerr << "Failed to stop Key event listener: " << static_cast<int>(ret) << std::endl;
        return;
    }
    std::cout << "Event listener stopped" << std::endl;
    std::cout << "Running: " << (key.isRunning() ? "Yes" : "No") << std::endl;

    std::cout << "\n=== Test Completed ===" << std::endl;
}

int main()
{
    try
    {
        testKey();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
