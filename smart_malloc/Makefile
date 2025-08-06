# Custom Memory Allocator Makefile
# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -g
TARGET = memory_allocator
SRCDIR = src
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/memory_allocator.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Test with sample input
test: $(TARGET)
	@echo "Running test cases..."
	@echo "alloc 1024" | ./$(TARGET) || true
	@echo "stats" | ./$(TARGET) || true

# Install (copy to /usr/local/bin)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)

# Show help
help:
	@echo "Available targets:"
	@echo "  all       - Build the memory allocator (default)"
	@echo "  clean     - Remove build artifacts"
	@echo "  run       - Build and run the program"
	@echo "  test      - Run basic tests"
	@echo "  install   - Install to /usr/local/bin"
	@echo "  uninstall - Remove from /usr/local/bin"
	@echo "  help      - Show this help message"

.PHONY: all clean run test install uninstall help
