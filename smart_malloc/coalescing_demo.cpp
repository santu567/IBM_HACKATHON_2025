#include "src/memory_allocator.h"
#include <iostream>
#include <vector>

int main() {
    std::cout << "🧪 Block Coalescing (Defragmentation) Demo" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    // Initialize the allocator
    MemoryAllocator::initialize_pool();
    
    std::cout << "\n📋 Test 1: Creating Fragmentation" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    
    // Allocate blocks to create fragmentation
    void* ptr1 = MemoryAllocator::xmalloc(512);   // 512B
    void* ptr2 = MemoryAllocator::xmalloc(256);   // 256B
    void* ptr3 = MemoryAllocator::xmalloc(1024);  // 1KB
    void* ptr4 = MemoryAllocator::xmalloc(512);   // 512B
    
    std::cout << "\n📊 Memory State After Allocations:" << std::endl;
    MemoryAllocator::stats();
    
    std::cout << "\n📋 Test 2: Automatic Coalescing on Free" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // Free the middle blocks to create fragmentation
    MemoryAllocator::xfree(ptr2);  // Free 256B block
    MemoryAllocator::xfree(ptr3);  // Free 1024B block
    
    std::cout << "\n📊 Memory State After Freeing Middle Blocks:" << std::endl;
    MemoryAllocator::stats();
    
    std::cout << "\n📋 Test 3: Manual Defragmentation" << std::endl;
    std::cout << "---------------------------------" << std::endl;
    
    // Run manual defragmentation
    MemoryAllocator::defragment();
    
    std::cout << "\n📊 Memory State After Defragmentation:" << std::endl;
    MemoryAllocator::stats();
    
    std::cout << "\n📋 Test 4: Complex Fragmentation Scenario" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // Free remaining blocks
    MemoryAllocator::xfree(ptr1);
    MemoryAllocator::xfree(ptr4);
    
    std::cout << "\n📊 Memory State After Freeing All Blocks:" << std::endl;
    MemoryAllocator::stats();
    
    // Create a more complex fragmentation pattern
    std::vector<void*> allocations;
    allocations.push_back(MemoryAllocator::xmalloc(256));   // 256B
    allocations.push_back(MemoryAllocator::xmalloc(512));   // 512B
    allocations.push_back(MemoryAllocator::xmalloc(1024));  // 1KB
    allocations.push_back(MemoryAllocator::xmalloc(256));   // 256B
    allocations.push_back(MemoryAllocator::xmalloc(512));   // 512B
    
    std::cout << "\n📊 Memory State After Complex Allocations:" << std::endl;
    MemoryAllocator::stats();
    
    // Free some blocks to create fragmentation
    MemoryAllocator::xfree(allocations[1]);  // Free 512B
    MemoryAllocator::xfree(allocations[3]);  // Free 256B
    
    std::cout << "\n📊 Memory State After Creating Fragmentation:" << std::endl;
    MemoryAllocator::stats();
    
    // Run defragmentation
    MemoryAllocator::defragment();
    
    std::cout << "\n📊 Memory State After Defragmentation:" << std::endl;
    MemoryAllocator::stats();
    
    // Clean up
    for (void* ptr : allocations) {
        if (ptr) MemoryAllocator::xfree(ptr);
    }
    
    std::cout << "\n📋 Final Memory State:" << std::endl;
    std::cout << "---------------------" << std::endl;
    MemoryAllocator::stats();
    MemoryAllocator::leakcheck();
    
    std::cout << "\n✅ Coalescing Demo completed successfully!" << std::endl;
    std::cout << "\n💡 Key Features Demonstrated:" << std::endl;
    std::cout << "   • Automatic coalescing when blocks are freed" << std::endl;
    std::cout << "   • Manual defragmentation with defragment() command" << std::endl;
    std::cout << "   • Reduction of memory fragmentation" << std::endl;
    std::cout << "   • Better memory utilization through block merging" << std::endl;
    
    return 0;
} 