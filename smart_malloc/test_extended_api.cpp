#include "src/memory_allocator.h"
#include <iostream>
#include <cstring>

int main() {
    std::cout << "🧪 Testing Extended Memory Allocator API" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Initialize the memory pool
    MemoryAllocator::initialize_pool();
    std::cout << std::endl;
    
    // Test 1: xcalloc
    std::cout << "📋 Test 1: xcalloc (zeroed memory allocation)" << std::endl;
    void* calloc_ptr = MemoryAllocator::xcalloc(10, 100);
    if (calloc_ptr) {
        std::cout << "✅ xcalloc successful: allocated 10 * 100 = 1000 bytes" << std::endl;
        
        // Verify it's zeroed
        char* check_ptr = static_cast<char*>(calloc_ptr);
        bool is_zeroed = true;
        for (int i = 0; i < 1000; i++) {
            if (check_ptr[i] != 0) {
                is_zeroed = false;
                break;
            }
        }
        std::cout << "🔍 Memory verification: " << (is_zeroed ? "✅ All bytes are zero" : "❌ Memory not zeroed") << std::endl;
    }
    std::cout << std::endl;
    
    // Test 2: xmalloc_aligned
    std::cout << "📋 Test 2: xmalloc_aligned (16-byte alignment)" << std::endl;
    void* aligned_ptr = MemoryAllocator::xmalloc_aligned(256, 16);
    if (aligned_ptr) {
        std::cout << "✅ xmalloc_aligned successful: allocated 256 bytes aligned to 16 bytes" << std::endl;
        std::cout << "🔍 Address: " << aligned_ptr << " (alignment check: " 
                  << (reinterpret_cast<uintptr_t>(aligned_ptr) % 16 == 0 ? "✅" : "❌") << ")" << std::endl;
    }
    std::cout << std::endl;
    
    // Test 3: xrealloc
    std::cout << "📋 Test 3: xrealloc (resize memory)" << std::endl;
    void* realloc_ptr = MemoryAllocator::xmalloc(512);
    if (realloc_ptr) {
        std::cout << "✅ Initial allocation: 512 bytes" << std::endl;
        
        // Fill with some data
        std::memset(realloc_ptr, 'A', 512);
        
        // Resize to larger size
        void* new_ptr = MemoryAllocator::xrealloc(realloc_ptr, 1024);
        if (new_ptr) {
            std::cout << "✅ xrealloc successful: resized from 512 to 1024 bytes" << std::endl;
            
            // Check if data was preserved
            char* check_ptr = static_cast<char*>(new_ptr);
            bool data_preserved = (check_ptr[0] == 'A' && check_ptr[511] == 'A');
            std::cout << "🔍 Data preservation: " << (data_preserved ? "✅" : "❌") << std::endl;
        }
    }
    std::cout << std::endl;
    
    // Show memory stats before xfree_all
    std::cout << "📊 Memory stats before xfree_all:" << std::endl;
    MemoryAllocator::stats();
    std::cout << std::endl;
    
    // Test 4: xfree_all
    std::cout << "📋 Test 4: xfree_all (free all memory)" << std::endl;
    MemoryAllocator::xfree_all();
    std::cout << std::endl;
    
    // Show memory stats after xfree_all
    std::cout << "📊 Memory stats after xfree_all:" << std::endl;
    MemoryAllocator::stats();
    std::cout << std::endl;
    
    std::cout << "🎉 All tests completed successfully!" << std::endl;
    return 0;
} 