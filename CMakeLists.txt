# 基本設定
cmake_minimum_required(VERSION 3.22)
project(raytrace LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)

file(GLOB_RECURSE PROJECT_SOURCES "code/*.cpp")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_compile_options(${PROJECT_NAME} PUBLIC "/utf-8")

# Lib
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# Include
find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})
