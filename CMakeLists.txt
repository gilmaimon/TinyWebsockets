cmake_minimum_required(VERSION 2.8)
project(any_lang)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")
file(GLOB MAIN "main.cpp")

add_executable(output ${SOURCES} ${MAIN})

target_link_libraries(output wsock32 ws2_32) 