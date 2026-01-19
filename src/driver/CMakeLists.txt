add_library(bsp_driver STATIC
    led/led.cpp
    key/key.cpp
    ap3216c/ap3216c.cpp
    dht11/dht11.cpp
)

target_include_directories(bsp_driver 
PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(bsp_driver 
PRIVATE
    bsp_common
    pthread
    spdlog::spdlog
)
