cmake_minimum_required(VERSION 3.11)
project(test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DSDL_MAIN_HANDLED)

file(GLOB_RECURSE STB "stb/*.cpp")

add_executable(bubbles "bubbles.cpp" ${STB})
target_link_libraries(bubbles PRIVATE gt)

add_executable(car "car.cpp" ${STB})
target_link_libraries(car PRIVATE gt)