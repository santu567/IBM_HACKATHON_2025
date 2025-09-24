#include <iostream>
#include <vector>
#include "src/memory_allocator.h"

void demonstrate_defragmentation() {
    std::cout << "🧹 DEFRAGMENTATION DEMONSTRATION" << std::endl;
    std::cout << "=================================" << std::endl;
    
    // Initialize the memory pool
    MemoryAllocator::initialize_pool();
    
    std::cout << "\n📋 Step 1: Create fragmented memory layout" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    // Allocate several blocks to create fragmentation
    std::vector<void*> allocated_blocks;
    
    // Allocate 5 blocks
    for (int i = 0; i < 5; i++) {
        void* ptr = MemoryAllocator::xmalloc(256);
        allocated_blocks.push_back(ptr);
        std::cout << "   Allocated block " << i + 1 << ": " << ptr << std::endl;
    }
    
    std::cout << "\n📊 Memory state after allocation:" << std::endl;
    MemoryAllocator::show_memory_map();
    
    std::cout << "\n📋 Step 2: Free blocks to create fragmentation" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    // Free blocks 1, 3, and 5 to create fragmentation
    MemoryAllocator::xfree(allocated_blocks[0]);  // Free block 1
    MemoryAllocator::xfree(allocated_blocks[2]);  // Free block 3
    MemoryAllocator::xfree(allocated_blocks[4]);  // Free block 5
    
    std::cout << "   Freed blocks 1, 3, and 5" << std::endl;
    
    std::cout << "\n📊 Memory state after freeing (FRAGMENTED):" << std::endl;
    MemoryAllocator::show_memory_map();
    
    std::cout << "\n📋 Step 3: Run defragmentation" << std::endl;
    std::cout << "===============================" << std::endl;
    
    // Run defragmentation
    MemoryAllocator::defragment();
    
    std::cout << "\n📊 Memory state after defragmentation:" << std::endl;
    MemoryAllocator::show_memory_map();
    
    std::cout << "\n📋 Step 4: Free remaining blocks" << std::endl;
    std::cout << "=================================" << std::endl;
    
    // Free remaining blocks
    MemoryAllocator::xfree(allocated_blocks[1]);  // Free block 2
    MemoryAllocator::xfree(allocated_blocks[3]);  // Free block 4
    
    std::cout << "   Freed remaining blocks" << std::endl;
    
    std::cout << "\n📊 Final memory state:" << std::endl;
    MemoryAllocator::show_memory_map();
}

void explain_defragmentation() {
    std::cout << "\n🎓 DEFRAGMENTATION EXPLANATION" << std::endl;
    std::cout << "=============================" << std::endl;
    
    std::cout << "\n🔍 What is Memory Fragmentation?" << std::endl;
    std::cout << "   • Memory becomes fragmented when free blocks are scattered" << std::endl;
    std::cout << "   • Large allocations may fail even if total free space is sufficient" << std::endl;
    std::cout << "   • Reduces memory utilization efficiency" << std::endl;
    
    std::cout << "\n🧹 What is Defragmentation?" << std::endl;
    std::cout << "   • Process of merging adjacent free blocks" << std::endl;
    std::cout << "   • Creates larger, more useful free regions" << std::endl;
    std::cout << "   • Improves allocation success rate" << std::endl;
    
    std::cout << "\n⚙️  How Our Defragmentation Works:" << std::endl;
    std::cout << "   1. Scan through all blocks in memory pool" << std::endl;
    std::cout << "   2. Find adjacent free blocks" << std::endl;
    std::cout << "   3. Merge them into larger free blocks" << std::endl;
    std::cout << "   4. Update block headers and free list" << std::endl;
    
    std::cout << "\n📊 Before Defragmentation:" << std::endl;
    std::cout << "   [USED][FREE][USED][FREE][USED][FREE]" << std::endl;
    std::cout << "   ↑ Small scattered free blocks" << std::endl;
    
    std::cout << "\n📊 After Defragmentation:" << std::endl;
    std::cout << "   [USED][FREE][USED][FREE]" << std::endl;
    std::cout << "   ↑ Larger consolidated free blocks" << std::endl;
    
    std::cout << "\n✅ Benefits of Defragmentation:" << std::endl;
    std::cout << "   • Better memory utilization" << std::endl;
    std::cout << "   • Higher allocation success rate" << std::endl;
    std::cout << "   • Reduced memory waste" << std::endl;
    std::cout << "   • Improved performance for large allocations" << std::endl;
}

void show_automatic_vs_manual() {
    std::cout << "\n🔄 AUTOMATIC vs MANUAL DEFRAGMENTATION" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    std::cout << "\n🤖 Automatic Defragmentation:" << std::endl;
    std::cout << "   • Happens automatically during xfree()" << std::endl;
    std::cout << "   • Merges adjacent blocks when freeing" << std::endl;
    std::cout << "   • Immediate but limited scope" << std::endl;
    
    std::cout << "\n👤 Manual Defragmentation:" << std::endl;
    std::cout << "   • Triggered by calling defragment()" << std::endl;
    std::cout << "   • Scans entire memory pool" << std::endl;
    std::cout << "   • Comprehensive but more expensive" << std::endl;
    
    std::cout << "\n🎯 When to Use Manual Defragmentation:" << std::endl;
    std::cout << "   • After many allocation/deallocation cycles" << std::endl;
    std::cout << "   • Before large allocations" << std::endl;
    std::cout << "   • When memory utilization is low" << std::endl;
    std::cout << "   • For performance optimization" << std::endl;
}

int main() {
    std::cout << "🧹 MEMORY DEFRAGMENTATION DEMO" << std::endl;
    std::cout << "==============================" << std::endl;
    
    demonstrate_defragmentation();
    explain_defragmentation();
    show_automatic_vs_manual();
    
    std::cout << "\n🎉 Defragmentation demonstration completed!" << std::endl;
    std::cout << "   This shows how our allocator handles memory fragmentation" << std::endl;
    std::cout << "   and improves memory utilization through defragmentation." << std::endl;
    
    return 0;
} 