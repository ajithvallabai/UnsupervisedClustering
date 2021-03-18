cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

link_directories(/usr/bin/X11)
link_directories(/lib/x86_64-linux-gnu/libpthread.so.0)

project(src)

add_executable(
    sample
    main.cpp 
)