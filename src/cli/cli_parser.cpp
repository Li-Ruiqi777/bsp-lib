#include "cli_parser.h"
#include "../common/bsp_common.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace bsp
{

Command CliParser::parse(int argc, char *argv[])
{
    Command cmd;
    cmd.type = CommandType::Unknown;

    if (argc < 2)
    {
        return cmd;
    }

    // 处理 -h 和 --help
    if (std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0)
    {
        cmd.type = CommandType::Help;
        return cmd;
    }

    // 处理 -v 和 --version
    if (std::strcmp(argv[1], "-v") == 0 || std::strcmp(argv[1], "--version") == 0)
    {
        cmd.type = CommandType::Version;
        return cmd;
    }

    // 解析 LED 命令: bsp_tool led set <dev_name> <on/off>
    if (argc >= 5 && std::strcmp(argv[1], "led") == 0)
    {
        if (std::strcmp(argv[2], "set") == 0)
        {
            cmd.type = CommandType::LedSet;
            cmd.led_args.dev_name = argv[3];

            if (std::strcmp(argv[4], "on") == 0)
            {
                cmd.led_args.state = true;
            }
            else if (std::strcmp(argv[4], "off") == 0)
            {
                cmd.led_args.state = false;
            }
            else
            {
                cmd.type = CommandType::Unknown; // 无效的状态参数
            }
            return cmd;
        }
    }

    return cmd;
}

void CliParser::printHelp()
{
    std::printf("bsp_tool - BSP Hardware Test Tool\n");
    std::printf("Version: %d.%d.%d\n\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    std::printf("Usage: bsp_tool <command> [options]\n\n");
    std::printf("Commands:\n");
    std::printf("  led set <dev_name> <on|off>    Set LED state\n");
    std::printf("                                 Example: bsp_tool led set led0 on\n");
    std::printf("  -h, --help                     Show this help message\n");
    std::printf("  -v, --version                  Show version information\n");
    std::printf("\n");
}

void CliParser::printVersion()
{
    std::printf("bsp_tool version %d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
}

} // namespace bsp
