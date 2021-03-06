cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 14)
project(tiny_websockets_tests C CXX)

#catch2
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE catch2/)

find_package (Threads)

# OpenSSL
find_package(OpenSSL REQUIRED)
message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")
include_directories(${OPENSSL_INCLUDE_DIR})
list(APPEND LIB_LIST ${OPENSSL_LIBRARIES})

file(GLOB TEST_SOURCES "src/*.cpp")
foreach(file ${TEST_SOURCES})
  get_filename_component(_F_NAME ${file} NAME_WE)
  add_executable(${_F_NAME} ${file})
  target_link_libraries (${_F_NAME} ${CMAKE_THREAD_LIBS_INIT})
  target_link_libraries(${_F_NAME} tiny_websockets_lib Catch)
  if(WIN32)
    target_link_libraries(${_F_NAME} wsock32 ws2_32)
  endif()
  add_test(NAME ${_F_NAME} COMMAND ${_F_NAME})
endforeach()