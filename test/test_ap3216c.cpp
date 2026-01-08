#include <iostream>
#include <thread>
#include <chrono>
#include "../src/driver/ap3216c/ap3216c.h"

using namespace bsp;

void testAP3216C()
{
    std::cout << "=== AP3216C Sensor Test ===" << std::endl;

    // 创建 AP3216C 实例
    AP3216C sensor("ap3216c");

    // 初始化设备
    std::cout << "\n[1] Initializing AP3216C device..." << std::endl;
    ErrorCode ret = sensor.init();
    if (ret != ErrorCode::Ok)
    {
        std::cerr << "Failed to initialize AP3216C device: " << static_cast<int>(ret) << std::endl;
        return;
    }
    std::cout << "Device initialized: " << sensor.getDeviceName() << std::endl;
    std::cout << "Device ready: " << (sensor.isReady() ? "Yes" : "No") << std::endl;

    // 读取传感器数据
    std::cout << "\n[2] Reading sensor data (10 times)..." << std::endl;
    AP3216CData data;

    for (int i = 0; i < 10; ++i)
    {
        ret = sensor.readData(data);
        if (ret != ErrorCode::Ok)
        {
            std::cerr << "Failed to read sensor data: " << static_cast<int>(ret) << std::endl;
            break;
        }

        std::cout << "\nRead #" << (i + 1) << ":" << std::endl;
        std::cout << "  IR (Infrared):  " << data.ir << std::endl;
        std::cout << "  ALS (Light):    " << data.als << std::endl;
        std::cout << "  PS (Proximity): " << data.ps << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "\n=== Test Completed ===" << std::endl;
}

int main()
{
    try
    {
        testAP3216C();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
