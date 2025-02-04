# Compiler and flags
CC = cc
CFLAGS = -std=c11 -Isrc/include -Isrc/thirdparty
WFLAGS = -Wall -Wextra -Wpedantic -Wconversion
WNOFLOAGS = -Wno-gnu-zero-variadic-macro-arguments
LDFLAGS = 
TARGET = build/main
TESTS = build/tests

# Default target
all: $(TARGET)
	./build/main

# Test target
test: $(TESTS)
	./build/tests

# Build main executable
$(TARGET): src/main.c | build
	$(CC) $(CFLAGS) $(WFLAGS) $(WNOFLOAGS) $< -o $@ $(LDFLAGS)

# Build test executable
$(TESTS): src/tests.c | build
	$(CC) $(CFLAGS) $(WFLAGS) $(WNOFLOAGS) -Wno-sign-conversion $< -o $@ $(LDFLAGS)

# Create build directory
build:
	mkdir -p build

# Clean build artifacts
clean:
	rm -rf build

.PHONY: all clean
