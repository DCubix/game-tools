cmake_minimum_required(VERSION 3.11)
project(gt)

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)

add_definitions(-DSDL_MAIN_HANDLED)

file(GLOB_RECURSE SRC "*.cpp" "*.c")

add_library(${PROJECT_NAME} ${SRC})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/
)

find_package(SDL2 CONFIG REQUIRED)
if (NOT UNIX)
	target_link_libraries(${PROJECT_NAME} PRIVATE SDL2)
else()
	target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
endif()

if (CMAKE_DL_LIBS)
	target_link_libraries(${PROJECT_NAME}
		${CMAKE_DL_LIBS}
		pthread
	)
endif()