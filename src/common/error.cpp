#include "bsp_common.h"
#include <string>

namespace bsp
{

std::string errorToString(ErrorCode err)
{
    switch (err)
    {
    case ErrorCode::Ok:
        return "Success";
    case ErrorCode::InvalidParam:
        return "Invalid parameter";
    case ErrorCode::DevOpen:
        return "Device open failed";
    case ErrorCode::DevIo:
        return "Device I/O failed";
    case ErrorCode::DevNotReady:
        return "Device not ready";
    case ErrorCode::MemAlloc:
        return "Memory allocation failed";
    case ErrorCode::Unsupported:
        return "Unsupported operation";
    default:
        return "Unknown error";
    }
}

} // namespace bsp
