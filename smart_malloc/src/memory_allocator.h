#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H

#include <cstddef>
#include <cstdint>

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
};

// Memory allocator class
class MemoryAllocator {
private:
    static char memory_pool[POOL_SIZE];
    static BlockHeader* free_list;
    static uint32_t next_block_id;
    static bool initialized;
    
    static void initialize_pool();
    static BlockHeader* find_free_block(size_t size);
    static void insert_free_block(BlockHeader* block);
    static void remove_from_free_list(BlockHeader* block);
    static bool is_valid_pointer(void* ptr);
    static BlockHeader* get_block_header(void* ptr);

public:
    // Core allocation functions
    static void* xmalloc(size_t size);
    static void xfree(void* ptr);
    
    // Utility functions
    static void leakcheck();
    static void stats();
    static void print_block_info(BlockHeader* block, const char* operation);
};

#endif // MEMORY_ALLOCATOR_H

