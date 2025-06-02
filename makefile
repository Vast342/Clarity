# Compiler and flags
CXX := clang++

# Architecture options (can be overridden: make ARCH_LEVEL=v3)
ARCH_LEVEL ?= native
ifeq ($(ARCH_LEVEL),native)
    ARCH := -march=native
else ifeq ($(ARCH_LEVEL),v1)
    ARCH := -march=x86-64
else ifeq ($(ARCH_LEVEL),v2)
    ARCH := -march=x86-64-v2
else ifeq ($(ARCH_LEVEL),v3)
    ARCH := -march=x86-64-v3
else ifeq ($(ARCH_LEVEL),v4)
    ARCH := -march=x86-64-v4
else
    $(error Invalid ARCH_LEVEL: $(ARCH_LEVEL). Use native, v1, v2, v3, or v4)
endif

# Build type (can be overridden: make BUILD_TYPE=datagen)
BUILD_TYPE ?= uci

CXXFLAGS := -std=c++20 -flto $(ARCH) -fexceptions -Wall -Wextra -pthread
_THIS     := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
_ROOT     := $(_THIS)
EVALFILE   = $(_ROOT)/src/cn_030.nnue

CXXFLAGS += -DNetworkFile=\"$(EVALFILE)\"

# Debug compiler flags
DEBUG_CXXFLAGS := -g3 -O1 -DDEBUG
BUILD_CXXFLAGS := -DNDEBUG -O3

# Directories
SRC_DIR := src
BUILD_DIR := build

# Base source files (common to both builds)
BASE_SRCS := $(filter-out src/external/fathom/tbchess.cpp, $(wildcard src/external/fathom/*.cpp))
BASE_SRCS += $(filter-out src/magic.cpp src/datagen.cpp src/wdldatagen.cpp src/uci.cpp, $(wildcard src/*.cpp))

# Build-specific source files
ifeq ($(BUILD_TYPE),datagen)
    SRCS := $(BASE_SRCS) src/datagen.cpp
    EXE_SUFFIX := -datagen
else ifeq ($(BUILD_TYPE),uci)
    SRCS := $(BASE_SRCS) src/uci.cpp
    EXE_SUFFIX :=
else
    $(error Invalid BUILD_TYPE: $(BUILD_TYPE). Use 'uci' or 'datagen')
endif

OBJS := $(addprefix $(BUILD_DIR)/,$(notdir $(SRCS:.cpp=.o)))

# Binary name
EXE := Clarity$(EXE_SUFFIX)

# Add architecture suffix if not native
ifneq ($(ARCH_LEVEL),native)
    EXE := $(EXE)-$(ARCH_LEVEL)
endif

# Append .exe to the binary name on Windows
ifeq ($(OS),Windows_NT)
    override EXE := $(EXE).exe
endif

ifeq ($(CXX), clang)
	LDFLAGS := -fuse-ld=lld
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
$(BUILD_DIR)/%.o: src/external/fathom/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -pthread -c -o $@ $<

# Create directories if they don't exist
$(BUILD_DIR):
	mkdir -p $@

# Debug target
debug: CXXFLAGS += $(DEBUG_CXXFLAGS)
debug: $(EXE)

# Convenience targets
uci:
	$(MAKE) BUILD_TYPE=uci

datagen:
	$(MAKE) BUILD_TYPE=datagen

# Architecture-specific targets
v1:
	$(MAKE) ARCH_LEVEL=v1

v2:
	$(MAKE) ARCH_LEVEL=v2

v3:
	$(MAKE) ARCH_LEVEL=v3

v4:
	$(MAKE) ARCH_LEVEL=v4

# Combined targets (examples)
datagen-v3:
	$(MAKE) BUILD_TYPE=datagen ARCH_LEVEL=v3

uci-v4:
	$(MAKE) BUILD_TYPE=uci ARCH_LEVEL=v4

bench: all
	./$(EXE) bench

run: all
	./$(EXE)

# Clean the build
clean:
	rm -rf $(BUILD_DIR) Clarity* 

# Show current configuration
info:
	@echo "Build Configuration:"
	@echo "  BUILD_TYPE: $(BUILD_TYPE)"
	@echo "  ARCH_LEVEL: $(ARCH_LEVEL)"
	@echo "  ARCH FLAGS: $(ARCH)"
	@echo "  EXECUTABLE: $(EXE)"
	@echo "  SOURCES: $(SRCS)"

# Phony targets
.PHONY: all debug clean uci datagen v1 v2 v3 v4 datagen-v3 uci-v4 info bench run

# Disable built-in rules and variables
.SUFFIXES: