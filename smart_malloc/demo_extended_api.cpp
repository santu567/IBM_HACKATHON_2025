#include "src/memory_allocator.h"
#include <iostream>
#include <cstring>

int main() {
    std::cout << "🧪 Extended Memory Allocator API Demo" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // Initialize memory pool
    MemoryAllocator::initialize_pool();
    std::cout << std::endl;
    
    // Test 1: xcalloc
    std::cout << "📋 Test 1: xcalloc" << std::endl;
    void* ptr1 = MemoryAllocator::xcalloc(5, 200);
    if (ptr1) {
        std::cout << "✅ xcalloc: allocated 5 * 200 = 1000 bytes" << std::endl;
        
        // Verify zeroed memory
        char* check = static_cast<char*>(ptr1);
        bool is_zero = true;
        for (int i = 0; i < 1000; i++) {
            if (check[i] != 0) {
                is_zero = false;
                break;
            }
        }
        std::cout << "🔍 Memory verification: " << (is_zero ? "✅ Zeroed" : "❌ Not zeroed") << std::endl;
    }
    std::cout << std::endl;
    
    // Test 2: xmalloc_aligned
    std::cout << "📋 Test 2: xmalloc_aligned" << std::endl;
    void* ptr2 = MemoryAllocator::xmalloc_aligned(256, 16);
    if (ptr2) {
        std::cout << "✅ xmalloc_aligned: 256 bytes aligned to 16 bytes" << std::endl;
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr2);
        std::cout << "🔍 Address: " << ptr2 << " (aligned: " << (addr % 16 == 0 ? "✅" : "❌") << ")" << std::endl;
    }
    std::cout << std::endl;
    
    // Test 3: xrealloc
    std::cout << "📋 Test 3: xrealloc" << std::endl;
    void* ptr3 = MemoryAllocator::xmalloc(512);
    if (ptr3) {
        std::cout << "✅ Initial allocation: 512 bytes" << std::endl;
        
        // Fill with data
        std::memset(ptr3, 'X', 512);
        
        // Resize
        void* new_ptr = MemoryAllocator::xrealloc(ptr3, 1024);
        if (new_ptr) {
            std::cout << "✅ xrealloc: resized to 1024 bytes" << std::endl;
            
            // Check data preservation
            char* check = static_cast<char*>(new_ptr);
            bool preserved = (check[0] == 'X' && check[511] == 'X');
            std::cout << "🔍 Data preservation: " << (preserved ? "✅" : "❌") << std::endl;
        }
    }
    std::cout << std::endl;
    
    // Show stats before xfree_all
    std::cout << "📊 Memory stats before xfree_all:" << std::endl;
    MemoryAllocator::stats();
    std::cout << std::endl;
    
    // Test 4: xfree_all
    std::cout << "📋 Test 4: xfree_all" << std::endl;
    MemoryAllocator::xfree_all();
    std::cout << std::endl;
    
    // Show stats after xfree_all
    std::cout << "📊 Memory stats after xfree_all:" << std::endl;
    MemoryAllocator::stats();
    std::cout << std::endl;
    
    std::cout << "🎉 Demo completed successfully!" << std::endl;
    return 0;
} 