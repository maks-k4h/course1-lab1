cmake_minimum_required(VERSION 3.20)
project(test_prog5 VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)
configure_file(Config.h.in Config.h)


add_executable(test_prog5 k17_konevych.cpp)


target_include_directories(test_prog5 PUBLIC
    "${PROJECT_BINARY_DIR}" ,
    "${PROJECT_BINARY_DIR}/cmake-build-debug")
