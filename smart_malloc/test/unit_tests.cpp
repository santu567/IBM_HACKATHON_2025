#include <gtest/gtest.h>
#include "../src/memory_allocator.h"
#include <vector>
#include <cstring>
#include <random>

class MemoryAllocatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset the allocator state before each test
        MemoryAllocator::leakcheck(); // This will initialize if needed
    }
    
    void TearDown() override {
        // Clean up after each test
        MemoryAllocator::leakcheck();
    }
};

// Basic allocation tests
TEST_F(MemoryAllocatorTest, BasicAllocation) {
    void* ptr = MemoryAllocator::xmalloc(1024);
    ASSERT_NE(ptr, nullptr);
    
    // Test that we can write to the allocated memory
    memset(ptr, 0xAA, 1024);
    
    MemoryAllocator::xfree(ptr);
}

TEST_F(MemoryAllocatorTest, MultipleAllocations) {
    std::vector<void*> ptrs;
    
    // Allocate multiple blocks
    for (int i = 0; i < 10; ++i) {
        void* ptr = MemoryAllocator::xmalloc(512);
        ASSERT_NE(ptr, nullptr);
        ptrs.push_back(ptr);
    }
    
    // Free all blocks
    for (void* ptr : ptrs) {
        MemoryAllocator::xfree(ptr);
    }
}

TEST_F(MemoryAllocatorTest, ZeroSizeAllocation) {
    void* ptr = MemoryAllocator::xmalloc(0);
    ASSERT_EQ(ptr, nullptr);
}

TEST_F(MemoryAllocatorTest, LargeAllocation) {
    // Try to allocate more than pool size
    void* ptr = MemoryAllocator::xmalloc(POOL_SIZE + 1024);
    ASSERT_EQ(ptr, nullptr);
}

TEST_F(MemoryAllocatorTest, NullPointerFree) {
    // Should handle null pointer gracefully
    MemoryAllocator::xfree(nullptr);
    // Test should not crash
}

TEST_F(MemoryAllocatorTest, InvalidPointerFree) {
    // Try to free a pointer outside the pool
    void* invalid_ptr = reinterpret_cast<void*>(0x12345678);
    MemoryAllocator::xfree(invalid_ptr);
    // Test should not crash
}

TEST_F(MemoryAllocatorTest, DoubleFree) {
    void* ptr = MemoryAllocator::xmalloc(1024);
    ASSERT_NE(ptr, nullptr);
    
    MemoryAllocator::xfree(ptr);
    MemoryAllocator::xfree(ptr); // Should handle double free gracefully
}

TEST_F(MemoryAllocatorTest, MemoryReuse) {
    void* ptr1 = MemoryAllocator::xmalloc(1024);
    ASSERT_NE(ptr1, nullptr);
    
    MemoryAllocator::xfree(ptr1);
    
    void* ptr2 = MemoryAllocator::xmalloc(1024);
    ASSERT_NE(ptr2, nullptr);
    
    // The second allocation should reuse the freed block
    // In a simple implementation, they might be the same address
    MemoryAllocator::xfree(ptr2);
}

TEST_F(MemoryAllocatorTest, FragmentationTest) {
    std::vector<void*> ptrs;
    
    // Allocate many small blocks
    for (int i = 0; i < 20; ++i) {
        void* ptr = MemoryAllocator::xmalloc(256);
        ASSERT_NE(ptr, nullptr);
        ptrs.push_back(ptr);
    }
    
    // Free every other block
    for (size_t i = 0; i < ptrs.size(); i += 2) {
        MemoryAllocator::xfree(ptrs[i]);
    }
    
    // Try to allocate a larger block
    void* large_ptr = MemoryAllocator::xmalloc(1024);
    ASSERT_NE(large_ptr, nullptr);
    
    // Clean up
    MemoryAllocator::xfree(large_ptr);
    for (size_t i = 1; i < ptrs.size(); i += 2) {
        MemoryAllocator::xfree(ptrs[i]);
    }
}

TEST_F(MemoryAllocatorTest, StressTest) {
    std::vector<void*> ptrs;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> size_dist(64, 2048);
    std::uniform_int_distribution<> action_dist(0, 1);
    
    // Random allocation and deallocation
    for (int i = 0; i < 100; ++i) {
        if (action_dist(gen) == 0 || ptrs.empty()) {
            // Allocate
            size_t size = size_dist(gen);
            void* ptr = MemoryAllocator::xmalloc(size);
            if (ptr != nullptr) {
                ptrs.push_back(ptr);
            }
        } else {
            // Free random block
            size_t index = std::uniform_int_distribution<>(0, ptrs.size() - 1)(gen);
            MemoryAllocator::xfree(ptrs[index]);
            ptrs.erase(ptrs.begin() + index);
        }
    }
    
    // Clean up remaining allocations
    for (void* ptr : ptrs) {
        MemoryAllocator::xfree(ptr);
    }
}

TEST_F(MemoryAllocatorTest, BoundaryTest) {
    // Test allocation at the boundary of what's possible
    void* ptr = MemoryAllocator::xmalloc(POOL_SIZE - sizeof(BlockHeader));
    ASSERT_NE(ptr, nullptr);
    MemoryAllocator::xfree(ptr);
}

TEST_F(MemoryAllocatorTest, StatisticsTest) {
    // Test that statistics work correctly
    MemoryAllocator::stats();
    
    void* ptr = MemoryAllocator::xmalloc(1024);
    ASSERT_NE(ptr, nullptr);
    
    MemoryAllocator::stats();
    
    MemoryAllocator::xfree(ptr);
    
    MemoryAllocator::stats();
}

TEST_F(MemoryAllocatorTest, LeakCheckTest) {
    // Test leak detection
    MemoryAllocator::leakcheck(); // Should show no leaks initially
    
    void* ptr = MemoryAllocator::xmalloc(1024);
    ASSERT_NE(ptr, nullptr);
    
    MemoryAllocator::leakcheck(); // Should show one leak
    
    MemoryAllocator::xfree(ptr);
    
    MemoryAllocator::leakcheck(); // Should show no leaks again
}

// Performance tests
TEST_F(MemoryAllocatorTest, PerformanceTest) {
    const int num_allocations = 1000;
    std::vector<void*> ptrs;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_allocations; ++i) {
        void* ptr = MemoryAllocator::xmalloc(512);
        ASSERT_NE(ptr, nullptr);
        ptrs.push_back(ptr);
    }
    
    auto mid = std::chrono::high_resolution_clock::now();
    
    for (void* ptr : ptrs) {
        MemoryAllocator::xfree(ptr);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    
    auto alloc_time = std::chrono::duration_cast<std::chrono::microseconds>(mid - start);
    auto free_time = std::chrono::duration_cast<std::chrono::microseconds>(end - mid);
    
    std::cout << "Performance Test Results:" << std::endl;
    std::cout << "  Allocations: " << num_allocations << " in " << alloc_time.count() << " μs" << std::endl;
    std::cout << "  Frees: " << num_allocations << " in " << free_time.count() << " μs" << std::endl;
    std::cout << "  Average allocation time: " << alloc_time.count() / num_allocations << " μs" << std::endl;
    std::cout << "  Average free time: " << free_time.count() / num_allocations << " μs" << std::endl;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 