#include "../common/bsp_common.h"
#include "../driver/led/led.h"
#include "cli_parser.h"
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
            std::fprintf(stderr, "Error: Failed to init LED %s: %s\n", cmd.led_args.dev_name.c_str(),
                         errorToString(ret).c_str());
            return 1;
        }

        ret = led.setState(cmd.led_args.state);
        if (ret != ErrorCode::Ok)
        {
            std::fprintf(stderr, "Error: Failed to set LED state: %s\n", errorToString(ret).c_str());
            return 1;
        }

        std::printf("LED %s set to %s\n", cmd.led_args.dev_name.c_str(), cmd.led_args.state ? "on" : "off");
        return 0;
    }

    case CommandType::Unknown:
    default:
        std::fprintf(stderr, "Error: Unknown command\n");
        CliParser::printHelp();
        return 1;
    }
}
