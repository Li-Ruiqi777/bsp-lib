#include "../bsp.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace bsp;

// 测试结果统计
static int test_count = 0;
static int pass_count = 0;
static int fail_count = 0;

#define TEST_ASSERT(condition, msg)                                                                          \
    do                                                                                                       \
    {                                                                                                        \
        test_count++;                                                                                        \
        if (condition)                                                                                       \
        {                                                                                                    \
            pass_count++;                                                                                    \
            std::printf("[PASS] %s\n", msg);                                                                 \
        }                                                                                                    \
        else                                                                                                 \
        {                                                                                                    \
            fail_count++;                                                                                    \
            std::fprintf(stderr, "[FAIL] %s\n", msg);                                                        \
        }                                                                                                    \
    } while (0)

// 测试正常功能（如果设备存在）
void test_normal_function()
{
    std::printf("\n=== Testing Normal Function ===\n");

    // 尝试初始化 led（如果设备存在）
    Led led("led");
    ErrorCode ret = led.init();

    if (ret == ErrorCode::Ok)
    {
        TEST_ASSERT(led.isReady(), "led.init() returns ready state");

        // 测试设置 LED 为打开
        ret = led.turnOn();
        TEST_ASSERT(ret == ErrorCode::Ok, "led.turnOn()");

        // 测试设置 LED 为关闭
        ret = led.turnOff();
        TEST_ASSERT(ret == ErrorCode::Ok, "led.turnOff()");

        // 测试 setState
        ret = led.setState(true);
        TEST_ASSERT(ret == ErrorCode::Ok, "led.setState(true)");

        ret = led.setState(false);
        TEST_ASSERT(ret == ErrorCode::Ok, "led.setState(false)");

        // 测试获取设备名
        std::string name = led.getDeviceName();
        TEST_ASSERT(name == "led", "led.getDeviceName()");
    }
    else
    {
        std::printf("[SKIP] Device /dev/led not available, skipping normal function test\n");
    }
}

// 测试移动语义
void test_move_semantics()
{
    std::printf("\n=== Testing Move Semantics ===\n");

    Led led1("led");
    if (led1.init() == ErrorCode::Ok)
    {
        Led led2 = std::move(led1);
        TEST_ASSERT(!led1.isReady(), "moved-from object is not ready");
        TEST_ASSERT(led2.isReady(), "moved-to object is ready");

        std::string name = led2.getDeviceName();
        TEST_ASSERT(name == "led", "moved object keeps device name");
    }
    else
    {
        std::printf("[SKIP] Device /dev/led not available, skipping move semantics test\n");
    }
}

// 测试设备不存在的情况
void test_device_not_found()
{
    std::printf("\n=== Testing Device Not Found ===\n");

    Led led("led_nonexistent");
    ErrorCode ret = led.init();
    TEST_ASSERT(ret == ErrorCode::DevOpen, "led.init() with non-existent device");
    TEST_ASSERT(!led.isReady(), "device not ready after init failure");
}

int main(int argc, char *argv[])
{
    std::printf("========================================\n");
    std::printf("BSP LED Module Test Suite\n");
    std::printf("========================================\n");

    // 运行所有测试
    test_normal_function();
    test_move_semantics();
    test_device_not_found();

    // 输出测试结果
    std::printf("\n========================================\n");
    std::printf("Test Summary:\n");
    std::printf("  Total:  %d\n", test_count);
    std::printf("  Passed: %d\n", pass_count);
    std::printf("  Failed: %d\n", fail_count);
    std::printf("========================================\n");

    return (fail_count == 0) ? 0 : 1;
}
