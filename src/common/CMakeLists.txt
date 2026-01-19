# 公共基础层库
add_library(bsp_common STATIC
    error.cpp
    utils.cpp
)

target_include_directories(bsp_common 
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)


