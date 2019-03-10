cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 14)
project(tiny_websockets_lib C CXX)

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel -Wundef -Wno-unused -Wno-variadic-macros -Wno-parentheses -fdiagnostics-show-option")
endif()

option(_WS_CONFIG_NO_TRUE_RANDOMNESS "Test config dont use on release" OFF)
option(_WS_CONFIG_SKIP_HANDSHAKE_ACCEPT_VALIDATION "Test config dont use on release" OFF)

configure_file(
  "include/tiny_websockets/ws_config_defs.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/tiny_websockets/ws_config_defs.hpp"
)

file(GLOB_RECURSE tinyws_SOURCES
  "src/*.cpp"
)

file(GLOB_RECURSE tinyws_HEADERS 
  "include/*.h"
  "include/*.hpp"
  "include/*.hpp.in"
)

include_directories(
  "include/tiny_websockets"
  "${CMAKE_CURRENT_BINARY_DIR}/include"
)

add_library(tiny_websockets_lib STATIC ${tinyws_HEADERS} ${tinyws_SOURCES})
set_target_properties(tiny_websockets_lib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(tiny_websockets_lib PUBLIC "include" "${CMAKE_CURRENT_BINARY_DIR}/include")

install(TARGETS tiny_websockets_lib DESTINATION lib)
