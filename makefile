# Compiler and flags
CXX := clang++-17
ARCH := -march=native
CXXFLAGS := -std=c++20 -flto $(ARCH) -fexceptions -Wall -Wextra
_THIS     := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
_ROOT     := $(_THIS)
EVALFILE   = $(_ROOT)/src/nets/clarity_net007.nnue

CXXFLAGS += -DNetworkFile=\"$(EVALFILE)\"
LDFLAGS :=

# Debug compiler flags
DEBUG_CXXFLAGS := -g3 -O1 -DDEBUG -fsanitize=address -fsanitize=undefined 

BUILD_CXXFLAGS := -DNDEBUG -O3

# Directories
SRC_DIR := src
BUILD_DIR := build

# Source files
SRCS := $(filter-out src/magic.cpp src/datagen.cpp, $(wildcard src/*.cpp))
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Binary name (set to Clarity)
EXE := Clarity

# Append .exe to the binary name on Windows
ifeq ($(OS),Windows_NT)
	CXXFLAGS += -fuse-ld=lld
    override EXE := $(EXE).exe
endif

# Default target
all: CXXFLAGS += $(BUILD_CXXFLAGS)
all: $(EXE) 

# Rule to build the target binary
$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJS)

# Rule to build object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Create directories if they don't exist
$(BUILD_DIR):
	mkdir -p $@

# Debug target
debug: CXXFLAGS += $(DEBUG_CXXFLAGS)
debug: $(EXE)

# Clean the build
clean:
	rm -rf $(BUILD_DIR) $(EXE) $(PGO_DIR) 

# Phony targets
.PHONY: all debug clean pgo-generate

# Disable built-in rules and variables
.SUFFIXES:
