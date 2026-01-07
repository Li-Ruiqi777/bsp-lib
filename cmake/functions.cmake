# sudo apt-get install clang-format-15
# 用法：make format
file(GLOB_RECURSE SOURCE_FILES "*.cpp" "*.h" "*.c" "*.hpp")
add_custom_target(format
    COMMAND clang-format-15 -i ${SOURCE_FILES}
    COMMENT "Formatting code with clang-format"
)