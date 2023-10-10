cmake_minimum_required(VERSION 3.11)
project(Clarity VERSION 0.1.1 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(Clarity_BMI2 src/board.cpp src/globals.cpp src/globals.h src/bmi2.h src/bmi2.cpp src/move.cpp src/movegen.cpp src/psqt.h src/psqt.cpp src/search.h src/search.cpp src/testessentials.h src/tests.cpp src/testsuites.h src/tt.h src/tt.cpp src/tuner.cpp src/tuner.h src/uci.cpp )
add_executable(Clarity_Native src/board.cpp src/globals.cpp src/globals.h src/bmi2.h src/bmi2.cpp src/move.cpp src/movegen.cpp src/psqt.h src/psqt.cpp src/search.h src/search.cpp src/testessentials.h src/tests.cpp src/testsuites.h src/tt.h src/tt.cpp src/tuner.cpp src/tuner.h src/uci.cpp )

target_compile_options(Clarity_BMI2 PRIVATE -Wall -Wextra -march=haswell)
target_compile_options(Clarity_Native PRIVATE -Wall -Wextra -march=native)
