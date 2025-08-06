#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>

// Debug macros for memory leak detection (only for external usage)
#ifdef DEBUG_MEMORY
    #define XMALLOC(size) MemoryAllocator::xmalloc_debug(size, __FILE__, __LINE__)
    #define XFREE(ptr) MemoryAllocator::xfree_debug(ptr, __FILE__, __LINE__)
    #define XCALLOC(num, size) MemoryAllocator::xcalloc_debug(num, size, __FILE__, __LINE__)
    #define XREALLOC(ptr, new_size) MemoryAllocator::xrealloc_debug(ptr, new_size, __FILE__, __LINE__)
    #define XMALLOC_ALIGNED(size, alignment) MemoryAllocator::xmalloc_aligned_debug(size, alignment, __FILE__, __LINE__)
#endif

// Memory pool configuration
constexpr size_t POOL_SIZE = 2 * 1024 * 1024;  // 2MB
constexpr size_t BLOCK_SIZE = 1024;             // 1KB blocks
constexpr size_t NUM_BLOCKS = POOL_SIZE / BLOCK_SIZE;

// Block header structure
struct BlockHeader {
    bool is_free;
    size_t size;
    BlockHeader* next;
    uint32_t block_id;  // For tracking block ID
    size_t alignment;   // For aligned allocations (0 = unaligned)
    size_t padding;     // Padding for alignment (0 = no padding)
    
    // Debug information for memory leak detection
    #ifdef DEBUG_MEMORY
        std::string alloc_file;  // File where allocation occurred
        int alloc_line;          // Line where allocation occurred
        std::string free_file;   // File where deallocation occurred
        int free_line;           // Line where deallocation occurred
    #endif
};

// Memory allocator class
class MemoryAllocator {
private:
    static char memory_pool[POOL_SIZE];
    static BlockHeader* free_list;
    static uint32_t next_block_id;
    static bool initialized;
    
    // Thread safety
    static std::mutex allocator_mutex;

    
    static BlockHeader* find_free_block(size_t size);
    static void insert_free_block(BlockHeader* block);
    static void remove_from_free_list(BlockHeader* block);
    static bool is_valid_pointer(void* ptr);
    static BlockHeader* get_block_header(void* ptr);
    
    // Coalescing helper functions
    static BlockHeader* get_next_block(BlockHeader* block);
    static BlockHeader* get_previous_block(BlockHeader* block);
    static void coalesce_with_next(BlockHeader* block);
    static void coalesce_with_previous(BlockHeader* block);

public:
    // Core allocation functions
    static void* xmalloc(size_t size);
    static void xfree(void* ptr);
    
    // Extended allocation functions
    static void* xcalloc(size_t num, size_t size);
    static void* xrealloc(void* ptr, size_t new_size);
    static void* xmalloc_aligned(size_t size, size_t alignment);
    static void xfree_all();
    
    // Debug allocation functions (when DEBUG_MEMORY is defined)
    #ifdef DEBUG_MEMORY
        static void* xmalloc_debug(size_t size, const char* file, int line);
        static void xfree_debug(void* ptr, const char* file, int line);
        static void* xcalloc_debug(size_t num, size_t size, const char* file, int line);
        static void* xrealloc_debug(void* ptr, size_t new_size, const char* file, int line);
        static void* xmalloc_aligned_debug(size_t size, size_t alignment, const char* file, int line);
    #endif
    
    // Utility functions
    static void initialize_pool();
    static void leakcheck();
    static void stats();
    static void defragment();
    static void print_block_info(BlockHeader* block, const char* operation);
    static void show_memory_map();
};

#endif // MEMORY_ALLOCATOR_H

