cmake_minimum_required(VERSION 3.9)
project(MN_MACD)

set(CMAKE_CXX_STANDARD 17)
add_executable(MN_MACD main.cpp macd.cpp macd.hpp)
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} stdc++fs ${PYTHON_LIBRARIES})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
