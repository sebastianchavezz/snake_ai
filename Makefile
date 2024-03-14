# Compiler
CC = g++

# Directories
SRC_DIR = src
INCLUDE_DIR = includes
BUILD_DIR = build
WEIGHTS_DIR = weights

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Executable name
TARGET = snake_game

# Compiler flags
CFLAGS = -std=c++11 -I$(INCLUDE_DIR) -I$(WEIGHTS_DIR) -g

# SDL library
SDL_LIBS = -lSDL2

# Phony targets
.PHONY: all clean

# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(SDL_LIBS)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

debug: $(TARGET)
	gdb ./$(TARGET)