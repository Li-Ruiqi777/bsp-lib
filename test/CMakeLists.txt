# LED 测试
add_executable(test_led test_led.cpp)
target_link_libraries(test_led bsp)

# KEY 测试
add_executable(test_key test_key.cpp)
target_link_libraries(test_key bsp)

# AP3216C 测试
add_executable(test_ap3216c test_ap3216c.cpp)
target_link_libraries(test_ap3216c bsp)

# DHT11 测试
add_executable(test_dht11 test_dht11.cpp)
target_link_libraries(test_dht11 bsp)
