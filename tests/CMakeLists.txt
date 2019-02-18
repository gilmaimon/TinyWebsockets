cmake_minimum_required(VERSION 2.8)
project(tiny_websockets_tests C CXX)

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wextra -Wall -Wno-long-long -pedantic")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wundef -Wno-unused -Wno-variadic-macros -Wno-parentheses -fdiagnostics-show-option")
endif()

#catch2
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE catch2/)


file(GLOB TEST_SOURCES "src/*.cpp")
foreach(file ${TEST_SOURCES})
  get_filename_component(_F_NAME ${file} NAME_WE)
  add_executable(${_F_NAME} ${file})
  target_link_libraries(${_F_NAME} Catch)
  target_include_directories(${_F_NAME} PUBLIC ${tiny_websockets_demo_SOURCE_DIR}/tiny_websockets_lib/include)
  target_link_libraries(${_F_NAME} tiny_websockets_lib wsock32 ws2_32)
  add_test(NAME ${_F_NAME} COMMAND ${_F_NAME})
endforeach()

#add_test(NAME CryptoTest COMMAND tests)

#include_directories(include)
#add_library(tiny_websockets_lib STATIC ${tinyws_HEADERS} ${tinyws_SOURCES})
#set_target_properties(tiny_websockets_lib PROPERTIES LINKER_LANGUAGE CXX)

#install(TARGETS tiny_websockets_lib DESTINATION lib)