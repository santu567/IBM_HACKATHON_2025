#include "src/memory_allocator.h"
#include <iostream>
#include <vector>

int main() {
    std::cout << "🧪 Best-Fit Allocation & Block Splitting Demo" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    // Initialize the allocator
    MemoryAllocator::initialize_pool();
    
    std::cout << "\n📋 Test 1: Best-Fit Allocation Strategy" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // Allocate blocks of different sizes to demonstrate best-fit
    void* ptr1 = MemoryAllocator::xmalloc(1024);  // 1KB
    void* ptr2 = MemoryAllocator::xmalloc(512);   // 512B
    void* ptr3 = MemoryAllocator::xmalloc(256);   // 256B
    
    std::cout << "\n📊 Current Statistics:" << std::endl;
    MemoryAllocator::stats();
    
    std::cout << "\n📋 Test 2: Block Splitting Demonstration" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // Free the middle block to create fragmentation
    MemoryAllocator::xfree(ptr2);
    
    std::cout << "\nAfter freeing 512B block:" << std::endl;
    MemoryAllocator::stats();
    
    // Now allocate a small block - should use the freed 512B block and split it
    void* ptr4 = MemoryAllocator::xmalloc(128);   // 128B
    
    std::cout << "\nAfter allocating 128B (should split the 512B block):" << std::endl;
    MemoryAllocator::stats();
    
    std::cout << "\n📋 Test 3: Memory Efficiency Comparison" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // Free all blocks
    MemoryAllocator::xfree(ptr1);
    MemoryAllocator::xfree(ptr3);
    MemoryAllocator::xfree(ptr4);
    
    std::cout << "\nAfter freeing all blocks:" << std::endl;
    MemoryAllocator::stats();
    
    // Demonstrate best-fit with multiple allocations
    std::vector<void*> allocations;
    
    std::cout << "\n📋 Test 4: Multiple Allocations (Best-Fit)" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;
    
    // Allocate blocks in random sizes to test best-fit
    allocations.push_back(MemoryAllocator::xmalloc(2048));  // 2KB
    allocations.push_back(MemoryAllocator::xmalloc(512));   // 512B
    allocations.push_back(MemoryAllocator::xmalloc(1024));  // 1KB
    allocations.push_back(MemoryAllocator::xmalloc(256));   // 256B
    allocations.push_back(MemoryAllocator::xmalloc(1536));  // 1.5KB
    
    std::cout << "\nAfter multiple allocations:" << std::endl;
    MemoryAllocator::stats();
    
    // Free some blocks to create fragmentation
    MemoryAllocator::xfree(allocations[1]);  // Free 512B
    MemoryAllocator::xfree(allocations[3]);  // Free 256B
    
    std::cout << "\nAfter freeing 512B and 256B blocks:" << std::endl;
    MemoryAllocator::stats();
    
    // Allocate new blocks - should use best-fit
    void* new_ptr1 = MemoryAllocator::xmalloc(128);   // Should fit in freed 256B block
    void* new_ptr2 = MemoryAllocator::xmalloc(256);   // Should fit in freed 512B block
    
    std::cout << "\nAfter allocating 128B and 256B (best-fit):" << std::endl;
    MemoryAllocator::stats();
    
    // Clean up
    for (void* ptr : allocations) {
        if (ptr) MemoryAllocator::xfree(ptr);
    }
    if (new_ptr1) MemoryAllocator::xfree(new_ptr1);
    if (new_ptr2) MemoryAllocator::xfree(new_ptr2);
    
    std::cout << "\n📋 Final Memory State:" << std::endl;
    std::cout << "----------------------" << std::endl;
    MemoryAllocator::stats();
    MemoryAllocator::leakcheck();
    
    std::cout << "\n✅ Demo completed successfully!" << std::endl;
    std::cout << "\n💡 Key Improvements Demonstrated:" << std::endl;
    std::cout << "   • Best-fit allocation reduces memory waste" << std::endl;
    std::cout << "   • Block splitting creates reusable smaller blocks" << std::endl;
    std::cout << "   • Better memory utilization and fragmentation control" << std::endl;
    
    return 0;
} 