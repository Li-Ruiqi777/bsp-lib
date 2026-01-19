# 命令行工具
add_executable(bsp_tool
    cli_parser.cpp
    bsp_tool_main.cpp
)

target_include_directories(bsp_tool 
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(bsp_tool 
PRIVATE
    bsp_common
    bsp_driver
    spdlog::spdlog
    pthread
)

