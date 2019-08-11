cmake_minimum_required(VERSION 3.11)
project(test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DSDL_MAIN_HANDLED)

file(GLOB_RECURSE SRC "*.cpp" "*.c")

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE gt)