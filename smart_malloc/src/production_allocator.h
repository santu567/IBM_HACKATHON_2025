#ifndef PRODUCTION_ALLOCATOR_H
#define PRODUCTION_ALLOCATOR_H

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <thread>
#include <atomic>
#include <vector>
#include <memory>

// Memory pool configuration
constexpr size_t POOL_SIZE = 2 * 1024 * 1024;  // 2MB
constexpr size_t BLOCK_SIZE = 1024;             // 1KB blocks
constexpr size_t NUM_BLOCKS = POOL_SIZE / BLOCK_SIZE;
constexpr size_t MAX_THREADS = 64;

// Block header structure
struct BlockHeader {
    std::atomic<bool> is_free;
    size_t size;
    BlockHeader* next;
    uint32_t block_id;
    std::atomic<uint32_t> ref_count;  // For thread safety
};

// Production-ready memory allocator with multiple strategies
class ProductionAllocator {
public:
    enum class ThreadSafety {
        SINGLE_THREADED,    // Fast, no overhead
        MUTEX_PROTECTED,    // Simple thread safety
        THREAD_LOCAL,       // Each thread gets its own allocator
        LOCK_FREE,          // Advanced lock-free implementation
        HYBRID              // Automatic strategy selection
    };

private:
    // Strategy 1: Mutex-protected allocator
    class MutexProtectedAllocator {
    private:
        static char memory_pool[POOL_SIZE];
        static BlockHeader* free_list;
        static uint32_t next_block_id;
        static bool initialized;
        static std::mutex allocator_mutex;
        static std::mutex init_mutex;

    public:
        static void initialize_pool();
        static void* xmalloc(size_t size);
        static void xfree(void* ptr);
        static void leakcheck();
        static void stats();
    };

    // Strategy 2: Thread-local allocator
    class ThreadLocalAllocator {
    private:
        static thread_local char memory_pool[POOL_SIZE];
        static thread_local BlockHeader* free_list;
        static thread_local uint32_t next_block_id;
        static thread_local bool initialized;

    public:
        static void initialize_pool();
        static void* xmalloc(size_t size);
        static void xfree(void* ptr);
        static void leakcheck();
        static void stats();
    };

    // Strategy 3: Lock-free allocator
    class LockFreeAllocator {
    private:
        static char memory_pool[POOL_SIZE];
        static std::atomic<BlockHeader*> free_list;
        static std::atomic<uint32_t> next_block_id;
        static std::atomic<bool> initialized;

    public:
        static void initialize_pool();
        static void* xmalloc(size_t size);
        static void xfree(void* ptr);
        static void leakcheck();
        static void stats();
    };

    // Strategy 4: Hybrid allocator (auto-selects best strategy)
    class HybridAllocator {
    private:
        static ThreadSafety current_strategy;
        static std::atomic<int> thread_count;
        static std::mutex strategy_mutex;

    public:
        static void set_strategy(ThreadSafety strategy);
        static ThreadSafety get_strategy();
        static void* xmalloc(size_t size);
        static void xfree(void* ptr);
        static void leakcheck();
        static void stats();
        static void register_thread();
        static void unregister_thread();
    };

    static ThreadSafety default_strategy;

public:
    // Main interface - automatically selects best strategy
    static void* xmalloc(size_t size);
    static void xfree(void* ptr);
    static void leakcheck();
    static void stats();
    
    // Configuration
    static void set_thread_safety(ThreadSafety strategy);
    static ThreadSafety get_thread_safety();
    
    // Performance monitoring
    static void enable_performance_monitoring(bool enable);
    static void get_performance_stats();
    
    // Thread management
    static void register_thread();
    static void unregister_thread();
    
    // Memory pool management
    static void reset_pool();
    static size_t get_pool_size();
    static size_t get_free_memory();
    static size_t get_used_memory();
};

// Performance monitoring
class PerformanceMonitor {
private:
    static std::atomic<uint64_t> total_allocations;
    static std::atomic<uint64_t> total_frees;
    static std::atomic<uint64_t> total_allocation_time_ns;
    static std::atomic<uint64_t> total_free_time_ns;
    static std::atomic<uint64_t> peak_memory_usage;
    static bool monitoring_enabled;

public:
    static void record_allocation(size_t size, uint64_t time_ns);
    static void record_free(uint64_t time_ns);
    static void record_memory_usage(size_t bytes);
    static void enable_monitoring(bool enable);
    static void print_stats();
    static void reset_stats();
};

// Thread-safe memory pool manager
class MemoryPoolManager {
private:
    static std::vector<std::unique_ptr<char[]>> thread_pools;
    static std::mutex pool_mutex;
    static std::atomic<size_t> active_pools;

public:
    static char* get_thread_pool();
    static void release_thread_pool(char* pool);
    static size_t get_active_pool_count();
    static void cleanup_unused_pools();
};

#endif // PRODUCTION_ALLOCATOR_H 