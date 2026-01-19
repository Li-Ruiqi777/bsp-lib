#include "../common/bsp_common.h"
#include "../driver/led/led.h"
#include "../driver/ap3216c/ap3216c.h"
#include "cli_parser.h"
#include <spdlog/spdlog.h>
#include <cstdio>
#include <cstdlib>

int main(int argc, char *argv[])
{
    using namespace bsp;

    // 解析命令行参数
    Command cmd = CliParser::parse(argc, argv);

    // 处理命令
    switch (cmd.type)
    {
    case CommandType::Help:
        CliParser::printHelp();
        return 0;

    case CommandType::Version:
        CliParser::printVersion();
        return 0;

    case CommandType::LedSet:
    {
        Led led(cmd.led_args.dev_name);
        ErrorCode ret = led.init();
        if (ret != ErrorCode::Ok)
        {
            spdlog::error("Failed to init LED {}: {}", cmd.led_args.dev_name, errorToString(ret));
            return 1;
        }

        ret = led.setState(cmd.led_args.state);
        if (ret != ErrorCode::Ok)
        {
            spdlog::error("Failed to set LED state: {}", errorToString(ret));
            return 1;
        }

        spdlog::info("LED {} set to {}", cmd.led_args.dev_name, cmd.led_args.state ? "on" : "off");
        return 0;
    }

    case CommandType::AP3216CRead:
    {
        AP3216C sensor(cmd.ap3216c_args.dev_name);
        ErrorCode ret = sensor.init();
        if (ret != ErrorCode::Ok)
        {
            spdlog::error("Failed to init AP3216C {}: {}", cmd.ap3216c_args.dev_name, errorToString(ret));
            return 1;
        }

        AP3216CData data;
        ret = sensor.readData(data);
        if (ret != ErrorCode::Ok)
        {
            spdlog::error("Failed to read AP3216C data: {}", errorToString(ret));
            return 1;
        }

        spdlog::info("AP3216C Sensor Data:");
        spdlog::info("  IR (Infrared):  {}", data.ir);
        spdlog::info("  ALS (Light):    {}", data.als);
        spdlog::info("  PS (Proximity): {}", data.ps);
        return 0;
    }

    case CommandType::Unknown:
    default:
        spdlog::error("Unknown command");
        CliParser::printHelp();
        return 1;
    }
}
