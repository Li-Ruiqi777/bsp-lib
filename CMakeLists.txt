cmake_minimum_required(VERSION 3.10)
project(bsp-lib VERSION 1.0.0 LANGUAGES CXX)

include(cmake/functions.cmake)
include(cmake/toolchain.cmake)

# 查找 spdlog 库
list(APPEND CMAKE_PREFIX_PATH "/home/lrq/linux/nfs/qtrootfs/usr/")
find_package(spdlog REQUIRED)

# ============================================================================
# 源码目录
# ============================================================================
add_subdirectory(src)

# ============================================================================
# 创建bsp动态库
# ============================================================================
add_library(bsp SHARED
    src/bsp.cpp
)

target_link_libraries(bsp 
PUBLIC
    bsp_driver
    bsp_common
    pthread
    spdlog::spdlog
)

# 设置库的头文件包含路径
target_include_directories(bsp 
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# 设置库版本
set_target_properties(bsp PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# ============================================================================
# 测试程序
# ============================================================================
add_subdirectory(test)

# ============================================================================
# 安装规则
# ============================================================================
set(CMAKE_INSTALL_PREFIX /home/lrq/linux/nfs/qtrootfs/usr)

install(TARGETS bsp 
                bsp_tool 
                test_led test_key test_ap3216c test_dht11
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(FILES bsp.h DESTINATION include)
install(DIRECTORY src/common DESTINATION include/bsp FILES_MATCHING PATTERN "*.h")
install(DIRECTORY src/driver DESTINATION include/bsp FILES_MATCHING PATTERN "*.h")

# ============================================================================
# 打印配置信息
# ============================================================================
message(STATUS "BSP Library Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")
