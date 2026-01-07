#include "bsp_common.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>

namespace bsp
{

// 日志级别字符串
static const char *log_level_str[] = {"DEBUG", "INFO", "WARN", "ERROR"};

// 静态成员初始化
LogLevel Logger::current_level_ = LogLevel::Debug;

void Logger::setLevel(LogLevel level)
{
    current_level_ = level;
}

LogLevel Logger::getLevel()
{
    return current_level_;
}

void Logger::log(LogLevel level, const std::string &module, const char *file, int line, const char *fmt, ...)
{
    // 检查日志级别
    if (level < current_level_)
    {
        return;
    }

    // 获取文件名（不含路径）
    const char *filename = std::strrchr(file, '/');
    if (filename == nullptr)
    {
        filename = std::strrchr(file, '\\');
    }
    if (filename == nullptr)
    {
        filename = file;
    }
    else
    {
        filename++; // 跳过 '/' 或 '\'
    }

    // 格式化日志前缀：[模块名][级别] 文件名:行号 -
    char prefix[128];
    if (!module.empty())
    {
        std::snprintf(prefix, sizeof(prefix), "[%s][%s] %s:%d - ", module.c_str(),
                      log_level_str[static_cast<int>(level)], filename, line);
    }
    else
    {
        std::snprintf(prefix, sizeof(prefix), "[%s] %s:%d - ", log_level_str[static_cast<int>(level)],
                      filename, line);
    }

    // 输出日志前缀
    std::fprintf(stderr, "%s", prefix);

    // 输出日志内容
    std::va_list args;
    va_start(args, fmt);
    std::vfprintf(stderr, fmt, args);
    va_end(args);

    // 输出换行
    std::fprintf(stderr, "\n");
    std::fflush(stderr);
}

} // namespace bsp
