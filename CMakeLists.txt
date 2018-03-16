cmake_minimum_required(VERSION 3.3)

# Name of the project (will be the name of the plugin)
project(node-addon-api-test)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

list (APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
find_package(NodeJS REQUIRED COMPONENTS NAPI)

add_subdirectory("test")
