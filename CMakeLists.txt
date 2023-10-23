cmake_minimum_required(VERSION 3.11)
project(Clarity VERSION 0.1.1 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(Common_SRC_Files 
src/board.cpp 
src/globals.cpp 
src/move.cpp 
src/movegen.cpp 
src/psqt.cpp
src/search.cpp
src/tests.cpp
src/tt.cpp 
src/uci.cpp)

add_executable(Clarity_Pext ${Common_SRC_Files} src/bmi2.cpp)
add_executable(Clarity_Magic ${Common_SRC_Files} src/magic.cpp)
add_executable(Clarity_Native ${Common_SRC_Files} src/bmi2.cpp)

target_compile_options(Clarity_Pext PRIVATE -Wall -Wextra -march=haswell)
target_compile_options(Clarity_Magic PRIVATE -Wall -Wextra -march=x86-64)
target_compile_options(Clarity_Native PRIVATE -Wall -Wextra -march=native)
target_link_libraries(Clarity_Pext -static)
target_link_libraries(Clarity_Magic -static)
target_link_libraries(Clarity_Native -static)