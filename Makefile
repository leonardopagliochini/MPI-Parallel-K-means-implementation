# Compiler
CC = mpic++

SRC_DIR = ./src
INC_DIR = ./include
BUILD_DIR = ./build

# Compiler flags
CFLAGS = -std=c++20 -I${mkBoostInc} -L${mkBoostLib} -I$(INC_DIR)

# Libraries
LIBS = -lutil -lboost_iostreams -lboost_system -lboost_filesystem

# Target executable name
TARGET = exe

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
CFLAGS = -Wall -std=c++20 -I${mkBoostInc} -L${mkBoostLib} -I$(INC_DIR)


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(OBJS)
