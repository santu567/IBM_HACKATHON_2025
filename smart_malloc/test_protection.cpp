#include "src/memory_allocator.h"
#include <iostream>

int main() {
    std::cout << "🧪 Testing Memory Protection and Debugging Features" << std::endl;
    std::cout << "==================================================" << std::endl;
    
    // Initialize the memory pool
    MemoryAllocator::initialize_pool();
    std::cout << std::endl;
    
    // Test 1: Normal allocation and free
    std::cout << "📋 Test 1: Normal allocation and free" << std::endl;
    void* ptr1 = MemoryAllocator::xmalloc(1024);
    if (ptr1) {
        std::cout << "✅ Allocated: " << ptr1 << std::endl;
        MemoryAllocator::xfree(ptr1);
        std::cout << "✅ Freed: " << ptr1 << std::endl;
    }
    std::cout << std::endl;
    
    // Test 2: Double-free detection
    std::cout << "📋 Test 2: Double-free detection" << std::endl;
    void* ptr2 = MemoryAllocator::xmalloc(512);
    if (ptr2) {
        std::cout << "✅ Allocated: " << ptr2 << std::endl;
        MemoryAllocator::xfree(ptr2);
        std::cout << "✅ First free successful" << std::endl;
        
        // Try to free the same pointer again
        std::cout << "🔄 Attempting double-free..." << std::endl;
        MemoryAllocator::xfree(ptr2);
    }
    std::cout << std::endl;
    
    // Test 3: Invalid pointer detection
    std::cout << "📋 Test 3: Invalid pointer detection" << std::endl;
    
    // Test with null pointer
    std::cout << "🔄 Testing null pointer..." << std::endl;
    MemoryAllocator::xfree(nullptr);
    
    // Test with pointer outside memory pool
    std::cout << "🔄 Testing pointer outside memory pool..." << std::endl;
    void* invalid_ptr = reinterpret_cast<void*>(0x12345678);
    MemoryAllocator::xfree(invalid_ptr);
    
    // Test with pointer that doesn't point to a valid block header
    std::cout << "🔄 Testing pointer to invalid block header..." << std::endl;
    void* invalid_block_ptr = reinterpret_cast<void*>(0x12345678); // Invalid pointer
    MemoryAllocator::xfree(invalid_block_ptr);
    std::cout << std::endl;
    
    // Test 4: Memory leak detection
    std::cout << "📋 Test 4: Memory leak detection" << std::endl;
    void* ptr3 = MemoryAllocator::xmalloc(256);
    void* ptr4 = MemoryAllocator::xmalloc(512);
    
    if (ptr3 && ptr4) {
        std::cout << "✅ Allocated two blocks" << std::endl;
        std::cout << "🔄 Running leak check..." << std::endl;
        MemoryAllocator::leakcheck();
        
        // Free one block and check again
        MemoryAllocator::xfree(ptr3);
        std::cout << "🔄 Running leak check after freeing one block..." << std::endl;
        MemoryAllocator::leakcheck();
        
        // Free the remaining block
        MemoryAllocator::xfree(ptr4);
        std::cout << "🔄 Running leak check after freeing all blocks..." << std::endl;
        MemoryAllocator::leakcheck();
    }
    std::cout << std::endl;
    
    // Test 5: Show final memory stats
    std::cout << "📋 Test 5: Final memory statistics" << std::endl;
    MemoryAllocator::stats();
    std::cout << std::endl;
    
    std::cout << "🎉 Protection and debugging tests completed!" << std::endl;
    return 0;
} 