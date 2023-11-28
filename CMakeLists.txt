# initializing stuff
cmake_minimum_required(VERSION 3.11)
project(Clarity VERSION 0.1.1 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

# all of the main typical files, used for all builds
set(Common_SRC_Files 
src/board.cpp 
src/globals.cpp 
src/move.cpp 
src/movegen.cpp 
src/eval.cpp
src/search.cpp
src/tests.cpp
src/tt.cpp)

# 3 seperate builds, one using BMI2's pext, one using typical magic bitboards, and one that's native. The native doesn't detect what your cpu uses though.
add_executable(Clarity_Pext ${Common_SRC_Files} src/bmi2.cpp src/uci.cpp)
add_executable(Clarity_Magic ${Common_SRC_Files} src/magic.cpp src/uci.cpp)
add_executable(Clarity_Native ${Common_SRC_Files} src/bmi2.cpp src/uci.cpp)

# data generator build
add_executable(Clarity_Datagen ${Common_SRC_Files} src/bmi2.cpp src/datagen.cpp)

# profiling build
add_executable(Clarity_Profiling ${Common_SRC_Files} src/bmi2.cpp src/uci.cpp)

# building with all warnings for all of these so that I can try to pick up on issues
target_compile_options(Clarity_Pext PRIVATE -Wall -Wextra -Wpedantic -march=haswell -flto)
target_compile_options(Clarity_Magic PRIVATE -Wall -Wextra -Wpedantic -march=x86-64 -flto)
target_compile_options(Clarity_Native PRIVATE -Wall -Wextra -Wpedantic -march=native -flto)

target_compile_options(Clarity_Datagen PRIVATE -Wall -Wextra -Wpedantic -march=native -flto)
target_compile_options(Clarity_Profiling PRIVATE -Wall -Wextra -Wpedantic -march=native -g -flto)

# making the entire binary need only itself to run, for ease of use
target_link_libraries(Clarity_Pext -static)
target_link_libraries(Clarity_Magic -static)
target_link_libraries(Clarity_Native -static)

target_link_libraries(Clarity_Datagen -static)
target_link_libraries(Clarity_Profiling -static)