#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <string>
#include <vector>

namespace bsp
{

// 命令类型
enum class CommandType
{
    LedSet,
    Version,
    Help,
    Unknown
};

// LED 命令参数
struct LedCommandArgs
{
    std::string dev_name;
    bool state; // true-on, false-off
};

// 解析后的命令结构
struct Command
{
    CommandType type;
    LedCommandArgs led_args;
};

/**
 * @brief 命令行解析器类
 */
class CliParser
{
public:
    /**
     * @brief 解析命令行参数
     * @param argc 参数个数
     * @param argv 参数数组
     * @return 解析后的命令结构
     */
    static Command parse(int argc, char *argv[]);

    /**
     * @brief 打印帮助信息
     */
    static void printHelp();

    /**
     * @brief 打印版本信息
     */
    static void printVersion();
};

} // namespace bsp

#endif // CLI_PARSER_H
