cmake_minimum_required(VERSION 3.11)
project(gt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DSDL_MAIN_HANDLED)

file(GLOB_RECURSE SRC "*.cpp" "*.c")

add_library(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/
)

find_package(SDL2 CONFIG REQUIRED)
if (NOT UNIX)
	target_link_libraries(${PROJECT_NAME} PRIVATE SDL2)
else()
	target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 ${CMAKE_DL_LIBS} pthread)
endif()
