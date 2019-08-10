cmake_minimum_required(VERSION 3.11)
project(gametools)

option(GT_ENABLE_TEST "Builds the test project" ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)
if (GT_ENABLE_TEST)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()