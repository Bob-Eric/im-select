cmake_minimum_required(VERSION 3.10)

project(im-select)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(EXPORT_COMPILE_COMMANDS ON)

add_executable(im-select im-select.cpp stdafx.cpp)
target_compile_options(im-select PRIVATE /utf-8)
target_link_libraries(im-select imm32.lib)