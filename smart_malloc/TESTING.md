# Testing Guide for Memory Allocator

This document explains how to test the custom memory allocator project.

## 🧪 Testing Overview

The project supports multiple testing approaches:

1. **Manual Testing** - Interactive CLI testing
2. **Unit Testing** - Automated tests with Google Test
3. **Integration Testing** - End-to-end functionality tests
4. **Performance Testing** - Benchmarking and stress tests

## 📋 Quick Start

### Basic Testing (No Dependencies)
```bash
# Build the project
make clean && make all

# Run basic tests
make test

# Run the test runner script
./test/run_tests.sh
```

### Advanced Testing (With Google Test)
```bash
# Install Google Test (macOS)
brew install googletest

# Install Google Test (Ubuntu/Debian)
sudo apt-get install libgtest-dev

# Build and run unit tests
make unit-test

# Run all tests
make test-all
```

## 🔧 Manual Testing

### Interactive Testing
```bash
# Start the interactive allocator
./memory_allocator

# Available commands:
alloc 1024        # Allocate 1KB
free 0x12345678   # Free block at address
stats             # Show memory statistics
leakcheck         # Check for memory leaks
help              # Show help
exit              # Exit
```

### Automated Manual Tests
```bash
# Run predefined test cases
echo "alloc 1024" | ./memory_allocator
echo "stats" | ./memory_allocator
echo "leakcheck" | ./memory_allocator
```

## 🧪 Unit Testing

### Test Categories

The unit tests cover the following areas:

#### 1. Basic Functionality
- **BasicAllocation**: Simple memory allocation
- **MultipleAllocations**: Multiple block allocations
- **MemoryReuse**: Memory reuse after freeing

#### 2. Edge Cases
- **ZeroSizeAllocation**: Allocating 0 bytes
- **LargeAllocation**: Allocating more than pool size
- **NullPointerFree**: Freeing null pointer
- **InvalidPointerFree**: Freeing invalid pointer
- **DoubleFree**: Double free detection

#### 3. Advanced Features
- **FragmentationTest**: Memory fragmentation handling
- **StressTest**: Random allocation/deallocation patterns
- **BoundaryTest**: Allocation at pool boundaries

#### 4. Utility Functions
- **StatisticsTest**: Memory statistics accuracy
- **LeakCheckTest**: Memory leak detection

#### 5. Performance
- **PerformanceTest**: Allocation/free performance metrics

### Running Unit Tests

```bash
# Build and run unit tests
make unit-test

# Run specific test
./unit_tests --gtest_filter=MemoryAllocatorTest.BasicAllocation

# Run tests with verbose output
./unit_tests --gtest_verbose

# Run tests with color output
./unit_tests --gtest_color=yes
```

### Test Output Example
```
[==========] Running 12 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 12 tests from MemoryAllocatorTest
[ RUN      ] MemoryAllocatorTest.BasicAllocation
[       OK ] MemoryAllocatorTest.BasicAllocation (0 ms)
[ RUN      ] MemoryAllocatorTest.MultipleAllocations
[       OK ] MemoryAllocatorTest.MultipleAllocations (1 ms)
...
[----------] 12 tests from MemoryAllocatorTest (15 ms total)
[----------] Global test environment tear-down
[==========] 12 tests from 1 test suite ran. (16 ms total)
[  PASSED  ] 12 tests.
```

## 🏗️ Building with CMake

If you prefer CMake over Make:

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build all targets
make build-all

# Run tests
make test-run
```

## 📊 Test Coverage

### What's Tested

✅ **Core Functions**
- `xmalloc()` - Memory allocation
- `xfree()` - Memory deallocation
- `leakcheck()` - Memory leak detection
- `stats()` - Memory statistics

✅ **Edge Cases**
- Zero-size allocations
- Oversized allocations
- Invalid pointer handling
- Double free detection

✅ **Memory Management**
- Block splitting
- Memory reuse
- Fragmentation handling
- Boundary conditions

✅ **Performance**
- Allocation speed
- Deallocation speed
- Memory overhead

### What's Not Tested (Future Improvements)

❌ **Advanced Features**
- Block merging (coalescing)
- Different allocation strategies (best-fit, worst-fit)
- Thread safety
- Memory alignment requirements

❌ **Integration Tests**
- Integration with real applications
- System-level memory pressure
- Long-running stress tests

## 🔍 Debugging Tests

### Common Issues

1. **Google Test Not Found**
   ```bash
   # Install Google Test
   brew install googletest  # macOS
   sudo apt-get install libgtest-dev  # Ubuntu
   ```

2. **Compilation Errors**
   ```bash
   # Clean and rebuild
   make clean
   make all
   ```

3. **Test Failures**
   ```bash
   # Run with verbose output
   ./unit_tests --gtest_verbose
   
   # Run specific failing test
   ./unit_tests --gtest_filter=MemoryAllocatorTest.SpecificTest
   ```

### Debugging Tips

1. **Enable Debug Output**
   ```cpp
   // Add to test file for debugging
   std::cout << "Debug: " << variable << std::endl;
   ```

2. **Use GDB with Tests**
   ```bash
   gdb --args ./unit_tests --gtest_filter=MemoryAllocatorTest.BasicAllocation
   ```

3. **Check Memory Layout**
   ```bash
   # Add to tests to inspect memory
   MemoryAllocator::stats();
   ```

## 📈 Performance Testing

### Benchmarking

The performance test measures:
- Allocation time per block
- Deallocation time per block
- Memory overhead
- Fragmentation impact

### Running Benchmarks

```bash
# Run performance test only
./unit_tests --gtest_filter=MemoryAllocatorTest.PerformanceTest

# Expected output:
# Performance Test Results:
#   Allocations: 1000 in 1234 μs
#   Frees: 1000 in 567 μs
#   Average allocation time: 1.234 μs
#   Average free time: 0.567 μs
```

## 🚀 Continuous Integration

### GitHub Actions Example

```yaml
name: Tests
on: [push, pull_request]
jobs:
  test:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v2
    - name: Install dependencies
      run: sudo apt-get install libgtest-dev
    - name: Build and test
      run: |
        make clean
        make unit-test
        ./unit_tests
```

## 📝 Adding New Tests

### Creating a New Test

```cpp
TEST_F(MemoryAllocatorTest, NewFeatureTest) {
    // Setup
    void* ptr = MemoryAllocator::xmalloc(1024);
    ASSERT_NE(ptr, nullptr);
    
    // Test the feature
    // ... your test logic ...
    
    // Cleanup
    MemoryAllocator::xfree(ptr);
}
```

### Test Best Practices

1. **Isolation**: Each test should be independent
2. **Cleanup**: Always free allocated memory
3. **Assertions**: Use specific assertions (ASSERT_NE, ASSERT_EQ, etc.)
4. **Naming**: Use descriptive test names
5. **Documentation**: Add comments for complex tests

## 🎯 Test Results Interpretation

### Success Indicators
- ✅ All tests pass
- ✅ No memory leaks detected
- ✅ Performance within expected ranges
- ✅ Edge cases handled gracefully

### Failure Indicators
- ❌ Test failures
- ❌ Memory leaks detected
- ❌ Performance degradation
- ❌ Crashes or segmentation faults

## 📚 Additional Resources

- [Google Test Documentation](https://google.github.io/googletest/)
- [Memory Allocation Strategies](https://en.wikipedia.org/wiki/Memory_management)
- [C++ Testing Best Practices](https://github.com/google/googletest/blob/master/googletest/docs/primer.md) 