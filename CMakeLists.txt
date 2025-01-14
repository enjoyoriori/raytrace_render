# 基本設定
cmake_minimum_required(VERSION 3.22)
project(raytrace LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)

file(GLOB_RECURSE PROJECT_SOURCES "code/*.cpp" "code/*.hpp")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})
target_compile_options(raytrace PUBLIC "/utf-8")

# Lib
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# Include
target_include_directories(${PROJECT_NAME} PRIVATE $ENV{VULKAN_SDK}/Include)