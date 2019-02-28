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

file(GLOB_RECURSE tinyws_SOURCES
    "src/*.cpp"
)

file(GLOB_RECURSE tinyws_HEADERS 
    "include/*.h"
    "include/*.hpp"
)

include_directories(include/tiny_websockets)
add_library(tiny_websockets_lib STATIC ${tinyws_HEADERS} ${tinyws_SOURCES})
set_target_properties(tiny_websockets_lib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(tiny_websockets_lib PUBLIC include)

install(TARGETS tiny_websockets_lib DESTINATION lib)
