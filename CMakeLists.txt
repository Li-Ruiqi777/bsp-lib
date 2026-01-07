cmake_minimum_required(VERSION 3.10)
project(bsp-lib VERSION 1.0.0 LANGUAGES CXX)

include(cmake/functions.cmake)
include(cmake/toolchain.cmake)

# 日志级别选项（默认 DEBUG）
option(BSP_LOG_LEVEL_DEBUG "Enable debug logs" ON)
if(BSP_LOG_LEVEL_DEBUG)
    add_definitions(-DBSP_LOG_LEVEL=0)
else()
    add_definitions(-DBSP_LOG_LEVEL=1)
endif()

# 公共基础层源文件
set(COMMON_SOURCES
    src/common/log.cpp
    src/common/error.cpp
    src/common/utils.cpp
)

# 驱动模块源文件
set(DRIVER_SOURCES
    src/driver/led/led.cpp
)

# 所有库源文件
set(LIB_SOURCES
    ${COMMON_SOURCES}
    ${DRIVER_SOURCES}
)

# 创建动态库
add_library(bsp SHARED ${LIB_SOURCES})

# 设置库的头文件包含路径
target_include_directories(bsp PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# 链接库
target_link_libraries(bsp pthread)

# 设置库版本
set_target_properties(bsp PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# 命令行工具源文件
set(CLI_SOURCES
    src/cli/cli_parser.cpp
    src/cli/bsp_tool_main.cpp
)

# 创建命令行工具
add_executable(bsp_tool ${CLI_SOURCES})
target_link_libraries(bsp_tool bsp)


# LED 测试
add_executable(test_led test/test_led.cpp)
target_link_libraries(test_led bsp)


# 安装规则
# DESTINATION为 CMAKE_INSTALL_PREFIX
set(CMAKE_INSTALL_PREFIX /home/lrq/linux/nfs/qtrootfs/usr)

# make install
install(TARGETS bsp 
                bsp_tool 
                test_led
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(FILES bsp.h DESTINATION include)
install(DIRECTORY src/common DESTINATION include/bsp FILES_MATCHING PATTERN "*.h")
install(DIRECTORY src/driver DESTINATION include/bsp FILES_MATCHING PATTERN "*.h")

# 打印配置信息
message(STATUS "BSP Library Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Debug Logs: ${BSP_LOG_LEVEL_DEBUG}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")

