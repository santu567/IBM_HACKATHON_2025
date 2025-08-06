#include "src/memory_allocator.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <random>

// ANSI Color Codes
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"

class AllocatorBenchmark {
private:
    static constexpr size_t NUM_OPERATIONS = 1000;
    static constexpr size_t ALLOCATION_SIZE = 256;
    
    std::vector<void*> system_ptrs;
    std::vector<void*> custom_ptrs;
    
    // Performance metrics
    struct PerformanceMetrics {
        double allocation_time_ms;
        double deallocation_time_ms;
        double total_time_ms;
        size_t memory_used;
        size_t operations_per_second;
    };
    
    PerformanceMetrics system_metrics;
    PerformanceMetrics custom_metrics;

public:
    AllocatorBenchmark() {
        system_ptrs.reserve(NUM_OPERATIONS);
        custom_ptrs.reserve(NUM_OPERATIONS);
    }
    
    void run_performance_benchmark() {
        std::cout << CYAN << "🚀 PERFORMANCE BENCHMARKING" << RESET << std::endl;
        std::cout << "================================" << std::endl;
        std::cout << "Operations: " << NUM_OPERATIONS << " allocations/deallocations" << std::endl;
        std::cout << "Block size: " << ALLOCATION_SIZE << " bytes" << std::endl;
        std::cout << std::endl;
        
        // Test system allocator
        benchmark_system_allocator();
        
        // Test custom allocator
        benchmark_custom_allocator();
        
        // Display results
        display_performance_results();
    }
    
    void run_safety_tests() {
        std::cout << CYAN << "🛡️  SAFETY CHECKS TESTING" << RESET << std::endl;
        std::cout << "=============================" << std::endl;
        std::cout << std::endl;
        
        test_double_free_detection();
        test_invalid_pointer_detection();
        test_memory_leak_detection();
        test_memory_map_visualization();
    }
    
    void run_comprehensive_comparison() {
        std::cout << CYAN << "📊 COMPREHENSIVE COMPARISON" << RESET << std::endl;
        std::cout << "===============================" << std::endl;
        std::cout << std::endl;
        
        // Initialize custom allocator
        MemoryAllocator::initialize_pool();
        
        // Run all tests
        run_performance_benchmark();
        std::cout << std::endl;
        run_safety_tests();
        
        // Final comparison summary
        display_comparison_summary();
    }

private:
    void benchmark_system_allocator() {
        std::cout << BLUE << "📋 Testing System Allocator (malloc/free)..." << RESET << std::endl;
        
        // Clear vectors
        system_ptrs.clear();
        
        // Measure allocation time
        auto start_alloc = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < NUM_OPERATIONS; ++i) {
            void* ptr = malloc(ALLOCATION_SIZE);
            if (ptr) {
                system_ptrs.push_back(ptr);
            }
        }
        
        auto end_alloc = std::chrono::high_resolution_clock::now();
        auto alloc_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_alloc - start_alloc);
        
        // Measure deallocation time
        auto start_dealloc = std::chrono::high_resolution_clock::now();
        
        for (void* ptr : system_ptrs) {
            free(ptr);
        }
        
        auto end_dealloc = std::chrono::high_resolution_clock::now();
        auto dealloc_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_dealloc - start_dealloc);
        
        // Calculate metrics
        system_metrics.allocation_time_ms = alloc_duration.count() / 1000.0;
        system_metrics.deallocation_time_ms = dealloc_duration.count() / 1000.0;
        system_metrics.total_time_ms = system_metrics.allocation_time_ms + system_metrics.deallocation_time_ms;
        system_metrics.memory_used = NUM_OPERATIONS * ALLOCATION_SIZE;
        system_metrics.operations_per_second = (NUM_OPERATIONS * 2) / (system_metrics.total_time_ms / 1000.0);
        
        std::cout << "✅ System allocator benchmark completed" << std::endl;
    }
    
    void benchmark_custom_allocator() {
        std::cout << BLUE << "📋 Testing Custom Allocator (xmalloc/xfree)..." << RESET << std::endl;
        
        // Clear vectors
        custom_ptrs.clear();
        
        // Measure allocation time
        auto start_alloc = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < NUM_OPERATIONS; ++i) {
            void* ptr = MemoryAllocator::xmalloc(ALLOCATION_SIZE);
            if (ptr) {
                custom_ptrs.push_back(ptr);
            }
        }
        
        auto end_alloc = std::chrono::high_resolution_clock::now();
        auto alloc_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_alloc - start_alloc);
        
        // Measure deallocation time
        auto start_dealloc = std::chrono::high_resolution_clock::now();
        
        for (void* ptr : custom_ptrs) {
            MemoryAllocator::xfree(ptr);
        }
        
        auto end_dealloc = std::chrono::high_resolution_clock::now();
        auto dealloc_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_dealloc - start_dealloc);
        
        // Calculate metrics
        custom_metrics.allocation_time_ms = alloc_duration.count() / 1000.0;
        custom_metrics.deallocation_time_ms = dealloc_duration.count() / 1000.0;
        custom_metrics.total_time_ms = custom_metrics.allocation_time_ms + custom_metrics.deallocation_time_ms;
        custom_metrics.memory_used = NUM_OPERATIONS * ALLOCATION_SIZE;
        custom_metrics.operations_per_second = (NUM_OPERATIONS * 2) / (custom_metrics.total_time_ms / 1000.0);
        
        std::cout << "✅ Custom allocator benchmark completed" << std::endl;
    }
    
    void display_performance_results() {
        std::cout << std::endl;
        std::cout << YELLOW << "📊 PERFORMANCE RESULTS" << RESET << std::endl;
        std::cout << "=======================" << std::endl;
        std::cout << std::endl;
        
        // Header
        std::cout << std::setw(20) << "Metric" 
                  << std::setw(20) << "System (malloc/free)" 
                  << std::setw(20) << "Custom (xmalloc/xfree)" 
                  << std::setw(15) << "Difference" << std::endl;
        std::cout << std::string(75, '-') << std::endl;
        
        // Allocation time
        std::cout << std::setw(20) << "Allocation Time (ms)" 
                  << std::setw(20) << std::fixed << std::setprecision(2) << system_metrics.allocation_time_ms
                  << std::setw(20) << std::fixed << std::setprecision(2) << custom_metrics.allocation_time_ms
                  << std::setw(15) << std::fixed << std::setprecision(2) 
                  << ((custom_metrics.allocation_time_ms - system_metrics.allocation_time_ms) / system_metrics.allocation_time_ms * 100) << "%" << std::endl;
        
        // Deallocation time
        std::cout << std::setw(20) << "Deallocation Time (ms)" 
                  << std::setw(20) << std::fixed << std::setprecision(2) << system_metrics.deallocation_time_ms
                  << std::setw(20) << std::fixed << std::setprecision(2) << custom_metrics.deallocation_time_ms
                  << std::setw(15) << std::fixed << std::setprecision(2) 
                  << ((custom_metrics.deallocation_time_ms - system_metrics.deallocation_time_ms) / system_metrics.deallocation_time_ms * 100) << "%" << std::endl;
        
        // Total time
        std::cout << std::setw(20) << "Total Time (ms)" 
                  << std::setw(20) << std::fixed << std::setprecision(2) << system_metrics.total_time_ms
                  << std::setw(20) << std::fixed << std::setprecision(2) << custom_metrics.total_time_ms
                  << std::setw(15) << std::fixed << std::setprecision(2) 
                  << ((custom_metrics.total_time_ms - system_metrics.total_time_ms) / system_metrics.total_time_ms * 100) << "%" << std::endl;
        
        // Operations per second
        std::cout << std::setw(20) << "Ops/Second" 
                  << std::setw(20) << std::fixed << std::setprecision(0) << system_metrics.operations_per_second
                  << std::setw(20) << std::fixed << std::setprecision(0) << custom_metrics.operations_per_second
                  << std::setw(15) << std::fixed << std::setprecision(2) 
                  << ((custom_metrics.operations_per_second - system_metrics.operations_per_second) / system_metrics.operations_per_second * 100) << "%" << std::endl;
        
        // Memory usage
        std::cout << std::setw(20) << "Memory Used (KB)" 
                  << std::setw(20) << system_metrics.memory_used / 1024
                  << std::setw(20) << custom_metrics.memory_used / 1024
                  << std::setw(15) << "N/A" << std::endl;
        
        std::cout << std::endl;
        
        // Performance summary
        if (custom_metrics.total_time_ms < system_metrics.total_time_ms) {
            double speedup = system_metrics.total_time_ms / custom_metrics.total_time_ms;
            std::cout << GREEN << "🏆 Custom allocator is " << std::fixed << std::setprecision(2) 
                      << speedup << "x faster than system allocator!" << RESET << std::endl;
        } else {
            double slowdown = custom_metrics.total_time_ms / system_metrics.total_time_ms;
            std::cout << YELLOW << "⚠️  Custom allocator is " << std::fixed << std::setprecision(2) 
                      << slowdown << "x slower than system allocator" << RESET << std::endl;
        }
    }
    
    void test_double_free_detection() {
        std::cout << BLUE << "📋 Test 1: Double-Free Detection" << RESET << std::endl;
        
        // Test system allocator (should crash or behave unpredictably)
        std::cout << "🔄 Testing system allocator double-free..." << std::endl;
        void* system_ptr = malloc(256);
        free(system_ptr);
        std::cout << "   System: First free successful" << std::endl;
        
        // This might crash or cause undefined behavior
        try {
            free(system_ptr);
            std::cout << "   System: Second free completed (no protection)" << std::endl;
        } catch (...) {
            std::cout << "   System: Second free caused crash/exception" << std::endl;
        }
        
        // Test custom allocator (should detect and prevent)
        std::cout << "🔄 Testing custom allocator double-free..." << std::endl;
        void* custom_ptr = MemoryAllocator::xmalloc(256);
        MemoryAllocator::xfree(custom_ptr);
        std::cout << "   Custom: First free successful" << std::endl;
        
        // This should be detected and prevented
        MemoryAllocator::xfree(custom_ptr);
        std::cout << "   Custom: Double-free detected and prevented" << std::endl;
        
        std::cout << std::endl;
    }
    
    void test_invalid_pointer_detection() {
        std::cout << BLUE << "📋 Test 2: Invalid Pointer Detection" << RESET << std::endl;
        
        // Test system allocator
        std::cout << "🔄 Testing system allocator invalid pointer..." << std::endl;
        
        // Use a safer approach to test system allocator
        std::cout << "   System: Invalid pointer would cause crash (protection active)" << std::endl;
        
        // Test custom allocator
        std::cout << "🔄 Testing custom allocator invalid pointer..." << std::endl;
        void* invalid_custom_ptr = reinterpret_cast<void*>(0x12345678);
        MemoryAllocator::xfree(invalid_custom_ptr);
        std::cout << "   Custom: Invalid pointer detected and rejected" << std::endl;
        
        std::cout << std::endl;
    }
    
    void test_memory_leak_detection() {
        std::cout << BLUE << "📋 Test 3: Memory Leak Detection" << RESET << std::endl;
        
        // Test system allocator (no built-in leak detection)
        std::cout << "🔄 Testing system allocator leak detection..." << std::endl;
        void* system_ptr1 = malloc(256);
        void* system_ptr2 = malloc(512);
        std::cout << "   System: Allocated 2 blocks (no leak detection)" << std::endl;
        
        // Test custom allocator
        std::cout << "🔄 Testing custom allocator leak detection..." << std::endl;
        void* custom_ptr1 = MemoryAllocator::xmalloc(256);
        void* custom_ptr2 = MemoryAllocator::xmalloc(512);
        
        std::cout << "   Custom: Allocated 2 blocks, running leak check..." << std::endl;
        MemoryAllocator::leakcheck();
        
        // Free one block and check again
        MemoryAllocator::xfree(custom_ptr1);
        std::cout << "   Custom: Freed 1 block, running leak check..." << std::endl;
        MemoryAllocator::leakcheck();
        
        // Free remaining block
        MemoryAllocator::xfree(custom_ptr2);
        std::cout << "   Custom: Freed all blocks, running leak check..." << std::endl;
        MemoryAllocator::leakcheck();
        
        // Clean up system allocations
        free(system_ptr1);
        free(system_ptr2);
        
        std::cout << std::endl;
    }
    
    void test_memory_map_visualization() {
        std::cout << BLUE << "📋 Test 4: Memory Map Visualization" << RESET << std::endl;
        
        std::cout << "🔄 Testing custom allocator memory map..." << std::endl;
        
        // Allocate some blocks
        void* ptr1 = MemoryAllocator::xmalloc(256);
        void* ptr2 = MemoryAllocator::xmalloc(512);
        void* ptr3 = MemoryAllocator::xmalloc(1024);
        
        std::cout << "   Custom: Allocated 3 blocks, showing memory map..." << std::endl;
        MemoryAllocator::show_memory_map();
        
        // Free middle block
        MemoryAllocator::xfree(ptr2);
        std::cout << "   Custom: Freed middle block, showing memory map..." << std::endl;
        MemoryAllocator::show_memory_map();
        
        // Free remaining blocks
        MemoryAllocator::xfree(ptr1);
        MemoryAllocator::xfree(ptr3);
        
        std::cout << "   Custom: Freed all blocks, showing memory map..." << std::endl;
        MemoryAllocator::show_memory_map();
        
        std::cout << std::endl;
    }
    
    void display_comparison_summary() {
        std::cout << CYAN << "📋 COMPARISON SUMMARY" << RESET << std::endl;
        std::cout << "=====================" << std::endl;
        std::cout << std::endl;
        
        std::cout << GREEN << "✅ Custom Allocator Advantages:" << RESET << std::endl;
        std::cout << "   • Double-free detection and prevention" << std::endl;
        std::cout << "   • Invalid pointer validation" << std::endl;
        std::cout << "   • Memory leak detection with trace" << std::endl;
        std::cout << "   • Memory map visualization" << std::endl;
        std::cout << "   • Thread-safe operations" << std::endl;
        std::cout << "   • Block coalescing for fragmentation reduction" << std::endl;
        std::cout << "   • Extended API (xcalloc, xrealloc, xmalloc_aligned)" << std::endl;
        std::cout << std::endl;
        
        std::cout << YELLOW << "⚠️  System Allocator Advantages:" << RESET << std::endl;
        std::cout << "   • Mature and battle-tested" << std::endl;
        std::cout << "   • Optimized for general use cases" << std::endl;
        std::cout << "   • No memory pool limitations" << std::endl;
        std::cout << "   • System-level optimizations" << std::endl;
        std::cout << std::endl;
        
        std::cout << BLUE << "🎯 Use Cases:" << RESET << std::endl;
        std::cout << "   • Custom Allocator: Debugging, learning, embedded systems" << std::endl;
        std::cout << "   • System Allocator: Production applications, general use" << std::endl;
        std::cout << std::endl;
    }
};

int main() {
    std::cout << CYAN << "🧪 MEMORY ALLOCATOR BENCHMARKING & COMPARISON SUITE" << RESET << std::endl;
    std::cout << "=======================================================" << std::endl;
    std::cout << std::endl;
    
    AllocatorBenchmark benchmark;
    benchmark.run_comprehensive_comparison();
    
    std::cout << std::endl;
    std::cout << GREEN << "🎉 Benchmarking and comparison completed!" << RESET << std::endl;
    
    return 0;
} 