#include <iostream>
#include <thread>
#include <chrono>
#include "../src/driver/dht11/dht11.h"

using namespace bsp;

void testDHT11()
{
    std::cout << "=== DHT11 Sensor Test ===" << std::endl;

    // 创建 DHT11 实例
    DHT11 sensor("dht11");

    // 初始化设备
    std::cout << "\n[1] Initializing DHT11 device..." << std::endl;
    ErrorCode ret = sensor.init();
    if (ret != ErrorCode::Ok)
    {
        std::cerr << "Failed to initialize DHT11 device: " << static_cast<int>(ret) << std::endl;
        return;
    }
    std::cout << "Device initialized: " << sensor.getDeviceName() << std::endl;
    std::cout << "Device ready: " << (sensor.isReady() ? "Yes" : "No") << std::endl;

    // 读取传感器数据
    std::cout << "\n[2] Reading sensor data (10 times)..." << std::endl;
    std::cout << "Note: DHT11 requires at least 2 seconds between readings" << std::endl;
    DHT11Data data;

    for (int i = 0; i < 10; ++i)
    {
        ret = sensor.readData(data);
        if (ret != ErrorCode::Ok)
        {
            std::cerr << "Failed to read sensor data: " << static_cast<int>(ret) << std::endl;
            // 如果是设备未就绪或IO错误，可能需要等待
            if (ret == ErrorCode::DevIo || ret == ErrorCode::DevNotReady)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            continue;
        }

        std::cout << "\nRead #" << (i + 1) << ":" << std::endl;
        std::cout << "  Humidity:    " << static_cast<int>(data.humidity_int) 
                  << "." << static_cast<int>(data.humidity_decimal) << "% RH" << std::endl;
        std::cout << "  Temperature: " << static_cast<int>(data.temperature_int) 
                  << "." << static_cast<int>(data.temperature_decimal) << "°C" << std::endl;

        // DHT11需要至少2秒的间隔
        std::this_thread::sleep_for(std::chrono::milliseconds(2100));
    }

    std::cout << "\n=== Test Completed ===" << std::endl;
}

int main()
{
    try
    {
        testDHT11();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

