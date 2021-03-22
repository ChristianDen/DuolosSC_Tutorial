cmake_minimum_required(VERSION 3.17)
project(DuolosSC_Tutorial)

set(CMAKE_PREFIX_PATH /net/sw/SystemC/systemc-2.3.3)

include_directories(${CMAKE_PREFIX_PATH}/include)

find_library(systemc systemc ${CMAKE_PREFIX_PATH}/lib-linux64)
link_directories(${CMAKE_PREFIX_PATH}/lib-linux64)

set(CMAKE_CXX_STANDARD 11)

add_executable(DuolosSC_Tutorial main.cpp top.h initiator.h target.h router.h)

target_link_libraries(DuolosSC_Tutorial systemc)