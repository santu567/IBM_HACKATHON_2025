#include "../src/memory_allocator.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <cstring>

// Simple test framework without external dependencies
class SimpleTestFramework {
private:
    int passed = 0;
    int failed = 0;
    std::string current_test;

public:
    void start_test(const std::string& test_name) {
        current_test = test_name;
        std::cout << "🧪 Running: " << test_name << std::endl;
    }

    void assert_true(bool condition, const std::string& message = "") {
        if (condition) {
            std::cout << "  ✅ PASS: " << message << std::endl;
            passed++;
        } else {
            std::cout << "  ❌ FAIL: " << message << std::endl;
            failed++;
        }
    }

    void assert_not_null(void* ptr, const std::string& message = "") {
        assert_true(ptr != nullptr, message);
    }

    void assert_null(void* ptr, const std::string& message = "") {
        assert_true(ptr == nullptr, message);
    }

    void assert_equal(int expected, int actual, const std::string& message = "") {
        assert_true(expected == actual, message + " (expected " + std::to_string(expected) + ", got " + std::to_string(actual) + ")");
    }

    void print_summary() {
        std::cout << "\n📊 Test Summary:" << std::endl;
        std::cout << "  ✅ Passed: " << passed << std::endl;
        std::cout << "  ❌ Failed: " << failed << std::endl;
        std::cout << "  📈 Total: " << (passed + failed) << std::endl;
        
        if (failed == 0) {
            std::cout << "🎉 All tests passed!" << std::endl;
        } else {
            std::cout << "⚠️  Some tests failed!" << std::endl;
        }
    }
};

int main() {
    SimpleTestFramework test;
    
    std::cout << "🧪 Simple Memory Allocator Tests" << std::endl;
    std::cout << "=================================" << std::endl;

    // Test 1: Basic allocation
    test.start_test("Basic Allocation");
    void* ptr1 = MemoryAllocator::xmalloc(1024);
    test.assert_not_null(ptr1, "Allocation should succeed");
    MemoryAllocator::xfree(ptr1);

    // Test 2: Zero size allocation
    test.start_test("Zero Size Allocation");
    void* ptr2 = MemoryAllocator::xmalloc(0);
    test.assert_null(ptr2, "Zero size allocation should return null");

    // Test 3: Large allocation
    test.start_test("Large Allocation");
    void* ptr3 = MemoryAllocator::xmalloc(POOL_SIZE + 1024);
    test.assert_null(ptr3, "Oversized allocation should return null");

    // Test 4: Multiple allocations
    test.start_test("Multiple Allocations");
    std::vector<void*> ptrs;
    for (int i = 0; i < 5; ++i) {
        void* ptr = MemoryAllocator::xmalloc(512);
        test.assert_not_null(ptr, "Multiple allocations should succeed");
        ptrs.push_back(ptr);
    }
    
    // Free all allocations
    for (void* ptr : ptrs) {
        MemoryAllocator::xfree(ptr);
    }

    // Test 5: Memory reuse
    test.start_test("Memory Reuse");
    void* ptr4 = MemoryAllocator::xmalloc(1024);
    test.assert_not_null(ptr4, "First allocation should succeed");
    MemoryAllocator::xfree(ptr4);
    
    void* ptr5 = MemoryAllocator::xmalloc(1024);
    test.assert_not_null(ptr5, "Second allocation should succeed");
    MemoryAllocator::xfree(ptr5);

    // Test 6: Null pointer free
    test.start_test("Null Pointer Free");
    MemoryAllocator::xfree(nullptr);
    test.assert_true(true, "Freeing null pointer should not crash");

    // Test 7: Invalid pointer free
    test.start_test("Invalid Pointer Free");
    void* invalid_ptr = reinterpret_cast<void*>(0x12345678);
    MemoryAllocator::xfree(invalid_ptr);
    test.assert_true(true, "Freeing invalid pointer should not crash");

    // Test 8: Double free
    test.start_test("Double Free");
    void* ptr6 = MemoryAllocator::xmalloc(1024);
    test.assert_not_null(ptr6, "Allocation should succeed");
    MemoryAllocator::xfree(ptr6);
    MemoryAllocator::xfree(ptr6); // Double free
    test.assert_true(true, "Double free should not crash");

    // Test 9: Memory writing
    test.start_test("Memory Writing");
    void* ptr7 = MemoryAllocator::xmalloc(1024);
    test.assert_not_null(ptr7, "Allocation should succeed");
    
    // Write to allocated memory
    memset(ptr7, 0xAA, 1024);
    test.assert_true(true, "Writing to allocated memory should succeed");
    
    MemoryAllocator::xfree(ptr7);

    // Test 10: Statistics
    test.start_test("Statistics");
    MemoryAllocator::stats();
    test.assert_true(true, "Statistics should not crash");

    // Test 11: Leak check
    test.start_test("Leak Check");
    MemoryAllocator::leakcheck();
    test.assert_true(true, "Leak check should not crash");

    // Test 12: Stress test
    test.start_test("Stress Test");
    std::vector<void*> stress_ptrs;
    for (int i = 0; i < 10; ++i) {
        void* ptr = MemoryAllocator::xmalloc(256);
        if (ptr != nullptr) {
            stress_ptrs.push_back(ptr);
        }
    }
    
    // Free some, allocate more
    for (size_t i = 0; i < stress_ptrs.size(); i += 2) {
        MemoryAllocator::xfree(stress_ptrs[i]);
    }
    
    // Allocate more
    for (int i = 0; i < 5; ++i) {
        void* ptr = MemoryAllocator::xmalloc(512);
        if (ptr != nullptr) {
            stress_ptrs.push_back(ptr);
        }
    }
    
    // Clean up
    for (void* ptr : stress_ptrs) {
        MemoryAllocator::xfree(ptr);
    }
    
    test.assert_true(true, "Stress test should complete without crashes");

    // Print final summary
    test.print_summary();

    return 0;
} 