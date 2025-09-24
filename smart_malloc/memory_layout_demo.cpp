#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/mman.h>
#include "src/memory_allocator.h"

void show_memory_layout() {
    std::cout << "🏗️  MEMORY LAYOUT IN REAL PROGRAM" << std::endl;
    std::cout << "=================================" << std::endl;
    
    // Show program's memory layout
    std::cout << "\n📊 Program Memory Layout:" << std::endl;
    std::cout << "┌─────────────────────────────────────┐" << std::endl;
    std::cout << "│ High Address                       │" << std::endl;
    std::cout << "├─────────────────────────────────────┤" << std::endl;
    std::cout << "│ Stack (local variables)            │" << std::endl;
    std::cout << "├─────────────────────────────────────┤" << std::endl;
    std::cout << "│ ↓ (grows downward)                 │" << std::endl;
    std::cout << "├─────────────────────────────────────┤" << std::endl;
    std::cout << "│ Heap (malloc/calloc)               │" << std::endl;
    std::cout << "│ ↑ (grows upward)                   │" << std::endl;
    std::cout << "├─────────────────────────────────────┤" << std::endl;
    std::cout << "│ Data Segment (global variables)    │" << std::endl;
    std::cout << "├─────────────────────────────────────┤" << std::endl;
    std::cout << "│ Text Segment (code)                │" << std::endl;
    std::cout << "└─────────────────────────────────────┘" << std::endl;
    std::cout << "│ Low Address                        │" << std::endl;
    std::cout << "" << std::endl;
}

void demonstrate_system_allocation() {
    std::cout << "🔧 SYSTEM ALLOCATION (malloc/calloc):" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // Show system allocation process
    std::cout << "\n1️⃣  Program requests memory:" << std::endl;
    std::cout << "   malloc(1024) called" << std::endl;
    
    std::cout << "\n2️⃣  System call chain:" << std::endl;
    std::cout << "   malloc() → vm_allocate() → macOS kernel" << std::endl;
    
    std::cout << "\n3️⃣  Kernel actions:" << std::endl;
    std::cout << "   • Allocates virtual memory page" << std::endl;
    std::cout << "   • Maps to physical memory (on first access)" << std::endl;
    std::cout << "   • Returns virtual address" << std::endl;
    
    std::cout << "\n4️⃣  Memory returned to program:" << std::endl;
    std::cout << "   • Virtual address returned" << std::endl;
    std::cout << "   • Physical memory assigned lazily" << std::endl;
    std::cout << "   • Page faults handled by kernel" << std::endl;
    
    // Demonstrate actual allocation
    void* system_ptr = malloc(1024);
    std::cout << "\n✅ System allocation result:" << std::endl;
    std::cout << "   Address: " << system_ptr << std::endl;
    std::cout << "   Size: 1024 bytes" << std::endl;
    free(system_ptr);
}

void demonstrate_custom_allocation() {
    std::cout << "\n🎯 CUSTOM ALLOCATION (Our Pool):" << std::endl;
    std::cout << "=================================" << std::endl;
    
    // Initialize our allocator
    MemoryAllocator::initialize_pool();
    
    std::cout << "\n1️⃣  Our memory pool:" << std::endl;
    std::cout << "   • 2MB static array (already allocated)" << std::endl;
    std::cout << "   • No system calls during allocation" << std::endl;
    std::cout << "   • Fixed size, predictable performance" << std::endl;
    
    std::cout << "\n2️⃣  Allocation process:" << std::endl;
    std::cout << "   xmalloc(1024) → Find free block → Split if needed" << std::endl;
    
    std::cout << "\n3️⃣  Memory management:" << std::endl;
    std::cout << "   • Best-fit algorithm" << std::endl;
    std::cout << "   • Block coalescing" << std::endl;
    std::cout << "   • Thread-safe operations" << std::endl;
    
    // Demonstrate actual allocation
    void* custom_ptr = MemoryAllocator::xmalloc(1024);
    std::cout << "\n✅ Custom allocation result:" << std::endl;
    std::cout << "   Address: " << custom_ptr << std::endl;
    std::cout << "   Size: 1024 bytes" << std::endl;
    MemoryAllocator::xfree(custom_ptr);
}

void compare_allocation_methods() {
    std::cout << "\n📊 COMPARISON: System vs Custom Allocation" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    std::cout << "\n🔧 System Allocation (malloc):" << std::endl;
    std::cout << "   ✅ Unlimited memory" << std::endl;
    std::cout << "   ✅ OS-optimized" << std::endl;
    std::cout << "   ✅ Mature and battle-tested" << std::endl;
    std::cout << "   ❌ System call overhead" << std::endl;
    std::cout << "   ❌ Unpredictable timing" << std::endl;
    std::cout << "   ❌ Can cause page faults" << std::endl;
    
    std::cout << "\n🎯 Custom Allocation (Our Pool):" << std::endl;
    std::cout << "   ✅ Predictable performance" << std::endl;
    std::cout << "   ✅ No system calls" << std::endl;
    std::cout << "   ✅ Safety features (double-free detection)" << std::endl;
    std::cout << "   ✅ Debug capabilities" << std::endl;
    std::cout << "   ❌ Limited to 2MB" << std::endl;
    std::cout << "   ❌ Manual memory management" << std::endl;
    std::cout << "   ❌ Not optimized for general use" << std::endl;
}

void show_real_world_usage() {
    std::cout << "\n🌍 REAL-WORLD USAGE SCENARIOS" << std::endl;
    std::cout << "===============================" << std::endl;
    
    std::cout << "\n🎯 When to use Custom Allocator:" << std::endl;
    std::cout << "   • Embedded systems (limited memory)" << std::endl;
    std::cout << "   • Real-time systems (predictable timing)" << std::endl;
    std::cout << "   • Debugging and learning" << std::endl;
    std::cout << "   • Memory-constrained environments" << std::endl;
    std::cout << "   • Safety-critical applications" << std::endl;
    
    std::cout << "\n🔧 When to use System Allocator:" << std::endl;
    std::cout << "   • General applications" << std::endl;
    std::cout << "   • Large memory requirements" << std::endl;
    std::cout << "   • Production environments" << std::endl;
    std::cout << "   • Multi-process applications" << std::endl;
}

int main() {
    std::cout << "🚀 MEMORY ALLOCATION IN REAL PROGRAMS" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    show_memory_layout();
    demonstrate_system_allocation();
    demonstrate_custom_allocation();
    compare_allocation_methods();
    show_real_world_usage();
    
    std::cout << "\n🎉 Demo completed! This shows how memory allocation" << std::endl;
    std::cout << "   works in real programs and how our custom allocator" << std::endl;
    std::cout << "   fits into the bigger picture." << std::endl;
    
    return 0;
} 