#include "memory_allocator.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <vector>
#include <mutex>

// Static member initialization
char MemoryAllocator::memory_pool[POOL_SIZE];
BlockHeader* MemoryAllocator::free_list = nullptr;
uint32_t MemoryAllocator::next_block_id = 0;
bool MemoryAllocator::initialized = false;
std::mutex MemoryAllocator::allocator_mutex;


void MemoryAllocator::initialize_pool() {
    if (initialized) return;
    
    // Initialize the first block header
    BlockHeader* first_block = reinterpret_cast<BlockHeader*>(memory_pool);
    first_block->is_free = true;
    first_block->size = POOL_SIZE - sizeof(BlockHeader);
    first_block->next = nullptr;
    first_block->block_id = next_block_id++;
    first_block->alignment = 0;
    first_block->padding = 0;
    
    #ifdef DEBUG_MEMORY
        first_block->alloc_file = "";
        first_block->alloc_line = 0;
        first_block->free_file = "";
        first_block->free_line = 0;
    #endif
    
    free_list = first_block;
    initialized = true;
    
    std::cout << "🔧 Memory pool initialized: " << POOL_SIZE / (1024 * 1024) 
              << "MB (" << NUM_BLOCKS << " blocks of " << BLOCK_SIZE << " bytes each)" << std::endl;
}

BlockHeader* MemoryAllocator::find_free_block(size_t size) {
    BlockHeader* current = free_list;
    BlockHeader* best_fit = nullptr;
    size_t best_fit_size = SIZE_MAX;
    
    // Best-fit strategy: find the smallest block that fits
    while (current != nullptr) {
        if (current->is_free && current->size >= size) {
            if (current->size < best_fit_size) {
                best_fit = current;
                best_fit_size = current->size;
            }
        }
        current = current->next;
    }
    
    if (best_fit != nullptr) {
        // Calculate block index for debug output
        char* pool_start = memory_pool;
        char* block_start = reinterpret_cast<char*>(best_fit);
        size_t block_index = (block_start - pool_start) / BLOCK_SIZE;
        
        std::cout << "Best-fit block found: index = " << block_index 
                  << ", size = " << best_fit->size << " bytes" << std::endl;
    }
    
    return best_fit;  // Best fit found or nullptr
}

void MemoryAllocator::insert_free_block(BlockHeader* block) {
    block->next = free_list;
    free_list = block;
}

void MemoryAllocator::remove_from_free_list(BlockHeader* block) {
    if (free_list == block) {
        free_list = block->next;
        return;
    }
    
    BlockHeader* current = free_list;
    while (current != nullptr && current->next != block) {
        current = current->next;
    }
    
    if (current != nullptr) {
        current->next = block->next;
    }
}

bool MemoryAllocator::is_valid_pointer(void* ptr) {
    if (ptr == nullptr) return false;
    
    char* char_ptr = static_cast<char*>(ptr);
    char* pool_start = memory_pool;
    char* pool_end = memory_pool + POOL_SIZE;
    
    return (char_ptr >= pool_start && char_ptr < pool_end);
}

BlockHeader* MemoryAllocator::get_block_header(void* ptr) {
    if (!is_valid_pointer(ptr)) return nullptr;
    
    // Search through all blocks to find the one containing this pointer
    char* pool_start = memory_pool;
    char* current_pos = pool_start;
    
    while (current_pos < pool_start + POOL_SIZE) {
        BlockHeader* header = reinterpret_cast<BlockHeader*>(current_pos);
        char* block_data = current_pos + sizeof(BlockHeader);
        
        if (ptr >= block_data && ptr < block_data + header->size) {
            return header;
        }
        
        current_pos += sizeof(BlockHeader) + header->size;
    }
    
    return nullptr;
}

void* MemoryAllocator::xmalloc(size_t size) {
    std::lock_guard<std::mutex> lock(allocator_mutex);
    
    if (!initialized) {
        initialize_pool();
    }
    
    if (size == 0) {
        std::cout << "❌ Error: Cannot allocate 0 bytes" << std::endl;
        return nullptr;
    }
    
    if (size > POOL_SIZE - sizeof(BlockHeader)) {
        std::cout << "❌ Error: Requested size " << size << " bytes exceeds pool capacity" << std::endl;
        return nullptr;
    }
    
    BlockHeader* block = find_free_block(size);
    if (block == nullptr) {
        std::cout << "❌ Error: No free block available for " << size << " bytes" << std::endl;
        return nullptr;
    }
    
    // Mark block as allocated
    block->is_free = false;
    block->alignment = 0;  // Regular allocation, no alignment
    block->padding = 0;    // No padding for regular allocation
    remove_from_free_list(block);
    
    // Check if block splitting is beneficial
    size_t min_split_size = sizeof(BlockHeader) + 32;  // Minimum useful split size
    if (block->size > size + min_split_size) {
        // Split the block: allocated portion + free portion
        BlockHeader* split_block = reinterpret_cast<BlockHeader*>(
            reinterpret_cast<char*>(block) + sizeof(BlockHeader) + size);
        
        // Initialize the split (free) block
        split_block->is_free = true;
        split_block->size = block->size - size - sizeof(BlockHeader);
        split_block->next = nullptr;
        split_block->block_id = next_block_id++;
        split_block->alignment = 0;
        split_block->padding = 0;
        
        // Update the allocated block size
        block->size = size;
        
        // Insert the split block into free list
        insert_free_block(split_block);
        
        std::cout << "Block split: allocated = " << size << " bytes, leftover = " 
                  << split_block->size << " bytes" << std::endl;
    }
    
    void* result = reinterpret_cast<char*>(block) + sizeof(BlockHeader);
    print_block_info(block, "ALLOCATED");
    
    return result;
}

void MemoryAllocator::xfree(void* ptr) {
    std::lock_guard<std::mutex> lock(allocator_mutex);
    
    if (!initialized) {
        std::cout << "❌ Error: Memory pool not initialized" << std::endl;
        return;
    }
    
    if (ptr == nullptr) {
        std::cout << "❌ Error: Cannot free null pointer" << std::endl;
        return;
    }
    
    // Enhanced invalid pointer detection
    if (!is_valid_pointer(ptr)) {
        std::cout << "\033[31mERROR: Invalid pointer passed to xfree()!\033[0m" << std::endl;
        std::cout << "   Pointer: " << ptr << " is outside memory pool bounds" << std::endl;
        return;
    }
    
    BlockHeader* block = get_block_header(ptr);
    if (block == nullptr) {
        std::cout << "\033[31mERROR: Invalid pointer passed to xfree()!\033[0m" << std::endl;
        std::cout << "   Pointer: " << ptr << " does not point to a valid block header" << std::endl;
        return;
    }
    
    // Enhanced double-free detection
    if (block->is_free) {
        std::cout << "\033[31mERROR: Double-free detected at address " << ptr << "!\033[0m" << std::endl;
        std::cout << "   Block ID: " << block->block_id << std::endl;
        std::cout << "   Block Size: " << block->size << " bytes" << std::endl;
        return;
    }
    
    // Mark as free
    block->is_free = true;
    insert_free_block(block);
    
    print_block_info(block, "FREED");
    
    // Automatic coalescing with adjacent free blocks
    coalesce_with_next(block);
    coalesce_with_previous(block);
}

void MemoryAllocator::leakcheck() {
    std::lock_guard<std::mutex> lock(allocator_mutex);
    
    if (!initialized) {
        std::cout << "🔍 Memory pool not initialized" << std::endl;
        return;
    }
    
    std::vector<BlockHeader*> leaked_blocks;
    char* current_pos = memory_pool;
    
    while (current_pos < memory_pool + POOL_SIZE) {
        BlockHeader* header = reinterpret_cast<BlockHeader*>(current_pos);
        
        if (!header->is_free) {
            leaked_blocks.push_back(header);
        }
        
        current_pos += sizeof(BlockHeader) + header->size;
    }
    
    if (leaked_blocks.empty()) {
        std::cout << "✅ No memory leaks detected" << std::endl;
    } else {
        std::cout << "⚠️  Memory leak detected! " << leaked_blocks.size() << " blocks not freed:" << std::endl;
        for (auto block : leaked_blocks) {
            print_block_info(block, "LEAKED");
            #ifdef DEBUG_MEMORY
                if (!block->alloc_file.empty()) {
                    std::cout << "   🔍 Allocated at: " << block->alloc_file << ":" << block->alloc_line << std::endl;
                }
            #endif
        }
    }
}

void MemoryAllocator::stats() {
    std::lock_guard<std::mutex> lock(allocator_mutex);
    
    if (!initialized) {
        std::cout << "📊 Memory pool not initialized" << std::endl;
        return;
    }
    
    size_t total_blocks = 0;
    size_t free_blocks = 0;
    size_t used_blocks = 0;
    size_t total_free_size = 0;
    size_t total_used_size = 0;
    
    char* current_pos = memory_pool;
    
    while (current_pos < memory_pool + POOL_SIZE) {
        BlockHeader* header = reinterpret_cast<BlockHeader*>(current_pos);
        total_blocks++;
        
        if (header->is_free) {
            free_blocks++;
            total_free_size += header->size;
        } else {
            used_blocks++;
            total_used_size += header->size;
        }
        
        current_pos += sizeof(BlockHeader) + header->size;
    }
    
    std::cout << "📊 Memory Allocator Statistics:" << std::endl;
    std::cout << "   Total Pool Size: " << std::setw(8) << POOL_SIZE << " bytes (" 
              << POOL_SIZE / (1024 * 8) << " MB)" << std::endl;
    std::cout << "   Total Blocks:    " << std::setw(8) << total_blocks << std::endl;
    std::cout << "   Used Blocks:     " << std::setw(8) << used_blocks 
              << " (" << (used_blocks * 100 / total_blocks) << "%)" << std::endl;
    std::cout << "   Free Blocks:     " << std::setw(8) << free_blocks 
              << " (" << (free_blocks * 100 / total_blocks) << "%)" << std::endl;
    std::cout << "   Used Memory:     " << std::setw(8) << total_used_size << " bytes" << std::endl;
    std::cout << "   Free Memory:     " << std::setw(8) << total_free_size << " bytes" << std::endl;
}

void MemoryAllocator::print_block_info(BlockHeader* block, const char* operation) {
    void* data_ptr = reinterpret_cast<char*>(block) + sizeof(BlockHeader);
    
    std::cout << "[" << operation << "] Block #" << std::setw(3) << block->block_id
              << " | Addr: " << std::hex << std::setw(12) << std::setfill('0') 
              << reinterpret_cast<uintptr_t>(data_ptr) << std::dec << std::setfill(' ')
              << " | Size: " << std::setw(6) << block->size << " bytes" << std::endl;
}

// Coalescing helper functions
BlockHeader* MemoryAllocator::get_next_block(BlockHeader* block) {
    char* next_pos = reinterpret_cast<char*>(block) + sizeof(BlockHeader) + block->size;
    char* pool_end = memory_pool + POOL_SIZE;
    
    if (next_pos >= pool_end) {
        return nullptr;  // No next block
    }
    
    return reinterpret_cast<BlockHeader*>(next_pos);
}

BlockHeader* MemoryAllocator::get_previous_block(BlockHeader* block) {
    char* pool_start = memory_pool;
    char* current_pos = pool_start;
    
    while (current_pos < reinterpret_cast<char*>(block)) {
        BlockHeader* header = reinterpret_cast<BlockHeader*>(current_pos);
        char* next_pos = current_pos + sizeof(BlockHeader) + header->size;
        
        if (next_pos == reinterpret_cast<char*>(block)) {
            return header;  // Found previous block
        }
        
        current_pos = next_pos;
    }
    
    return nullptr;  // No previous block
}

void MemoryAllocator::coalesce_with_next(BlockHeader* block) {
    BlockHeader* next_block = get_next_block(block);
    
    if (next_block != nullptr && next_block->is_free) {
        // Calculate block indices for debug output
        char* pool_start = memory_pool;
        size_t block_index = (reinterpret_cast<char*>(block) - pool_start) / BLOCK_SIZE;
        size_t next_index = (reinterpret_cast<char*>(next_block) - pool_start) / BLOCK_SIZE;
        
        std::cout << "🔗 Merging block at index " << block_index 
                  << " with next block at index " << next_index << std::endl;
        
        // Remove next block from free list
        remove_from_free_list(next_block);
        
        // Merge the blocks
        block->size += sizeof(BlockHeader) + next_block->size;
        
        // Update the next block's metadata to indicate it's been merged
        next_block->is_free = false;  // Mark as invalid
        next_block->size = 0;
        next_block->next = nullptr;
        
        std::cout << "   ✅ Merged into single block of " << block->size << " bytes" << std::endl;
    }
}

void MemoryAllocator::coalesce_with_previous(BlockHeader* block) {
    BlockHeader* prev_block = get_previous_block(block);
    
    if (prev_block != nullptr && prev_block->is_free) {
        // Calculate block indices for debug output
        char* pool_start = memory_pool;
        size_t block_index = (reinterpret_cast<char*>(block) - pool_start) / BLOCK_SIZE;
        size_t prev_index = (reinterpret_cast<char*>(prev_block) - pool_start) / BLOCK_SIZE;
        
        std::cout << "🔗 Merging block at index " << block_index 
                  << " with previous block at index " << prev_index << std::endl;
        
        // Remove current block from free list
        remove_from_free_list(block);
        
        // Merge the blocks (previous block absorbs current block)
        prev_block->size += sizeof(BlockHeader) + block->size;
        
        // Update the current block's metadata to indicate it's been merged
        block->is_free = false;  // Mark as invalid
        block->size = 0;
        block->next = nullptr;
        
        std::cout << "   ✅ Merged into single block of " << prev_block->size << " bytes" << std::endl;
    }
}

void MemoryAllocator::defragment() {
    std::lock_guard<std::mutex> lock(allocator_mutex);
    
    if (!initialized) {
        std::cout << "🔍 Memory pool not initialized" << std::endl;
        return;
    }
    
    std::cout << "🧹 Starting memory defragmentation..." << std::endl;
    
    char* current_pos = memory_pool;
    int merge_count = 0;
    
    while (current_pos < memory_pool + POOL_SIZE) {
        BlockHeader* header = reinterpret_cast<BlockHeader*>(current_pos);
        
        if (header->is_free) {
            // Try to coalesce with next block
            BlockHeader* next_block = get_next_block(header);
            if (next_block != nullptr && next_block->is_free) {
                coalesce_with_next(header);
                merge_count++;
            }
        }
        
        current_pos += sizeof(BlockHeader) + header->size;
    }
    
    if (merge_count == 0) {
        std::cout << "✅ No adjacent free blocks found - memory already defragmented" << std::endl;
    } else {
        std::cout << "✅ Defragmentation complete! Merged " << merge_count << " adjacent free blocks" << std::endl;
    }
}

void MemoryAllocator::show_memory_map() {
    std::lock_guard<std::mutex> lock(allocator_mutex);
    
    if (!initialized) {
        std::cout << "🔍 Memory pool not initialized" << std::endl;
        return;
    }
    
    std::cout << "🗺️  Memory Map (showing first 50 blocks):" << std::endl;
    
    char* current_pos = memory_pool;
    int block_count = 0;
    const int max_blocks = 50;
    
    std::cout << "Memory Layout: ";
    
    while (current_pos < memory_pool + POOL_SIZE && block_count < max_blocks) {
        BlockHeader* header = reinterpret_cast<BlockHeader*>(current_pos);
        
        if (header->is_free) {
            std::cout << "\033[32m[FREE]\033[0m";  // Green
        } else {
            std::cout << "\033[31m[USED]\033[0m";  // Red
        }
        
        // Move to next block
        current_pos += sizeof(BlockHeader) + header->size;
        block_count++;
    }
    
    if (block_count >= max_blocks) {
        std::cout << "\033[34m...\033[0m";  // Blue
    }
    
    std::cout << std::endl;
    std::cout << "Legend: \033[32m[FREE]\033[0m = Free block, \033[31m[USED]\033[0m = Allocated block" << std::endl;
}

// Extended allocation functions implementation

void* MemoryAllocator::xcalloc(size_t num, size_t size) {
    std::lock_guard<std::mutex> lock(allocator_mutex);
    
    if (!initialized) {
        initialize_pool();
    }
    
    // Check for overflow
    if (num > 0 && size > SIZE_MAX / num) {
        std::cout << "❌ Error: xcalloc overflow - num * size exceeds maximum size" << std::endl;
        return nullptr;
    }
    
    size_t total_size = num * size;
    if (total_size == 0) {
        std::cout << "❌ Error: Cannot allocate 0 bytes with xcalloc" << std::endl;
        return nullptr;
    }
    
    if (total_size > POOL_SIZE - sizeof(BlockHeader)) {
        std::cout << "❌ Error: Requested size " << total_size << " bytes exceeds pool capacity" << std::endl;
        return nullptr;
    }
    
    // Find a free block
    BlockHeader* block = find_free_block(total_size);
    if (block == nullptr) {
        std::cout << "❌ Error: No free block available for " << total_size << " bytes" << std::endl;
        return nullptr;
    }
    
    // Mark block as allocated
    block->is_free = false;
    block->alignment = 0;  // Regular allocation, no alignment
    block->padding = 0;    // No padding for regular allocation
    remove_from_free_list(block);
    
    // Check if block splitting is beneficial
    size_t min_split_size = sizeof(BlockHeader) + 32;  // Minimum useful split size
    if (block->size > total_size + min_split_size) {
        // Split the block: allocated portion + free portion
        BlockHeader* split_block = reinterpret_cast<BlockHeader*>(
            reinterpret_cast<char*>(block) + sizeof(BlockHeader) + total_size);
        
        // Initialize the split (free) block
        split_block->is_free = true;
        split_block->size = block->size - total_size - sizeof(BlockHeader);
        split_block->next = nullptr;
        split_block->block_id = next_block_id++;
        split_block->alignment = 0;
        split_block->padding = 0;
        
        // Update the allocated block size
        block->size = total_size;
        
        // Insert the split block into free list
        insert_free_block(split_block);
        
        std::cout << "Block split: allocated = " << total_size << " bytes, leftover = " 
                  << split_block->size << " bytes" << std::endl;
    }
    
    void* result = reinterpret_cast<char*>(block) + sizeof(BlockHeader);
    print_block_info(block, "CALLOC");
    
    // Initialize all bytes to 0
    std::memset(result, 0, total_size);
    
    std::cout << "✅ xcalloc successful: " << num << " * " << size << " = " << total_size << " bytes (zeroed)" << std::endl;
    return result;
}

void* MemoryAllocator::xrealloc(void* ptr, size_t new_size) {
    std::lock_guard<std::mutex> lock(allocator_mutex);
    
    if (!initialized) {
        initialize_pool();
    }
    
    // If ptr is nullptr, behave like xmalloc
    if (ptr == nullptr) {
        std::cout << "🔄 xrealloc: nullptr provided, allocating " << new_size << " bytes" << std::endl;
        
        if (new_size == 0) {
            std::cout << "❌ Error: Cannot allocate 0 bytes" << std::endl;
            return nullptr;
        }
        
        if (new_size > POOL_SIZE - sizeof(BlockHeader)) {
            std::cout << "❌ Error: Requested size " << new_size << " bytes exceeds pool capacity" << std::endl;
            return nullptr;
        }
        
        BlockHeader* block = find_free_block(new_size);
        if (block == nullptr) {
            std::cout << "❌ Error: No free block available for " << new_size << " bytes" << std::endl;
            return nullptr;
        }
        
        // Mark block as allocated
        block->is_free = false;
        block->alignment = 0;
        block->padding = 0;
        remove_from_free_list(block);
        
        // Check if block splitting is beneficial
        size_t min_split_size = sizeof(BlockHeader) + 32;
        if (block->size > new_size + min_split_size) {
            BlockHeader* split_block = reinterpret_cast<BlockHeader*>(
                reinterpret_cast<char*>(block) + sizeof(BlockHeader) + new_size);
            
            split_block->is_free = true;
            split_block->size = block->size - new_size - sizeof(BlockHeader);
            split_block->next = nullptr;
            split_block->block_id = next_block_id++;
            split_block->alignment = 0;
            split_block->padding = 0;
            
            block->size = new_size;
            insert_free_block(split_block);
        }
        
        void* result = reinterpret_cast<char*>(block) + sizeof(BlockHeader);
        print_block_info(block, "REALLOC_NEW");
        return result;
    }
    
    // If new_size is 0, behave like xfree
    if (new_size == 0) {
        std::cout << "🔄 xrealloc: new_size is 0, freeing pointer" << std::endl;
        
        if (!is_valid_pointer(ptr)) {
            std::cout << "❌ Error: Invalid pointer address" << std::endl;
            return nullptr;
        }
        
        BlockHeader* block = get_block_header(ptr);
        if (block == nullptr) {
            std::cout << "❌ Error: Pointer not found in memory pool" << std::endl;
            return nullptr;
        }
        
        if (block->is_free) {
            std::cout << "❌ Error: Double free detected" << std::endl;
            return nullptr;
        }
        
        // Mark as free
        block->is_free = true;
        insert_free_block(block);
        print_block_info(block, "REALLOC_FREE");
        
        // Automatic coalescing with adjacent free blocks
        coalesce_with_next(block);
        coalesce_with_previous(block);
        
        return nullptr;
    }
    
    // Validate the pointer
    if (!is_valid_pointer(ptr)) {
        std::cout << "❌ Error: xrealloc - invalid pointer address" << std::endl;
        return nullptr;
    }
    
    BlockHeader* block = get_block_header(ptr);
    if (block == nullptr) {
        std::cout << "❌ Error: xrealloc - pointer not found in memory pool" << std::endl;
        return nullptr;
    }
    
    size_t old_size = block->size;
    
    // If new size is smaller or equal, keep the existing block
    if (new_size <= old_size) {
        std::cout << "🔄 xrealloc: new size (" << new_size << ") <= old size (" << old_size << "), keeping existing block" << std::endl;
        return ptr;
    }
    
    // Try to extend in-place if there's a free block right after
    BlockHeader* next_block = get_next_block(block);
    if (next_block != nullptr && next_block->is_free) {
        size_t available_space = next_block->size + sizeof(BlockHeader);
        if (new_size <= old_size + available_space) {
            // We can extend in-place
            size_t needed_extra = new_size - old_size;
            
            // Remove next block from free list
            remove_from_free_list(next_block);
            
            // Extend current block
            block->size = new_size;
            
            // If there's still space left, create a new free block
            if (available_space > needed_extra + sizeof(BlockHeader)) {
                BlockHeader* new_free_block = reinterpret_cast<BlockHeader*>(
                    reinterpret_cast<char*>(block) + sizeof(BlockHeader) + new_size);
                
                new_free_block->is_free = true;
                new_free_block->size = available_space - needed_extra - sizeof(BlockHeader);
                new_free_block->next = nullptr;
                new_free_block->block_id = next_block_id++;
                new_free_block->alignment = 0;
                new_free_block->padding = 0;
                
                insert_free_block(new_free_block);
            }
            
            std::cout << "✅ xrealloc: extended in-place from " << old_size << " to " << new_size << " bytes" << std::endl;
            return ptr;
        }
    }
    
    // Cannot extend in-place, need to allocate new block and copy
    std::cout << "🔄 xrealloc: allocating new block of " << new_size << " bytes and copying data" << std::endl;
    
    // Allocate new block
    BlockHeader* new_block = find_free_block(new_size);
    if (new_block == nullptr) {
        std::cout << "❌ Error: xrealloc failed - no free block available" << std::endl;
        return nullptr;
    }
    
    // Mark new block as allocated
    new_block->is_free = false;
    new_block->alignment = 0;
    new_block->padding = 0;
    remove_from_free_list(new_block);
    
    // Check if block splitting is beneficial
    size_t min_split_size = sizeof(BlockHeader) + 32;
    if (new_block->size > new_size + min_split_size) {
        BlockHeader* split_block = reinterpret_cast<BlockHeader*>(
            reinterpret_cast<char*>(new_block) + sizeof(BlockHeader) + new_size);
        
        split_block->is_free = true;
        split_block->size = new_block->size - new_size - sizeof(BlockHeader);
        split_block->next = nullptr;
        split_block->block_id = next_block_id++;
        split_block->alignment = 0;
        split_block->padding = 0;
        
        new_block->size = new_size;
        insert_free_block(split_block);
    }
    
    void* new_ptr = reinterpret_cast<char*>(new_block) + sizeof(BlockHeader);
    print_block_info(new_block, "REALLOC_NEW");
    
    // Copy old data to new location
    std::memcpy(new_ptr, ptr, old_size);
    
    // Free the old block
    block->is_free = true;
    insert_free_block(block);
    print_block_info(block, "REALLOC_FREE");
    
    // Automatic coalescing with adjacent free blocks
    coalesce_with_next(block);
    coalesce_with_previous(block);
    
    std::cout << "✅ xrealloc: copied " << old_size << " bytes to new location" << std::endl;
    return new_ptr;
}

void* MemoryAllocator::xmalloc_aligned(size_t size, size_t alignment) {
    std::lock_guard<std::mutex> lock(allocator_mutex);
    
    if (!initialized) {
        initialize_pool();
    }
    
    if (size == 0) {
        std::cout << "❌ Error: Cannot allocate 0 bytes with xmalloc_aligned" << std::endl;
        return nullptr;
    }
    
    // Check if alignment is a power of 2
    if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
        std::cout << "❌ Error: xmalloc_aligned - alignment must be a power of 2" << std::endl;
        return nullptr;
    }
    
    // Calculate required size including padding for alignment
    size_t header_size = sizeof(BlockHeader);
    size_t total_size = size + header_size + alignment - 1;
    
    if (total_size > POOL_SIZE - sizeof(BlockHeader)) {
        std::cout << "❌ Error: xmalloc_aligned - requested size exceeds pool capacity" << std::endl;
        return nullptr;
    }
    
    // Find a block that can accommodate the aligned allocation
    BlockHeader* block = find_free_block(total_size);
    if (block == nullptr) {
        std::cout << "❌ Error: xmalloc_aligned - no free block available" << std::endl;
        return nullptr;
    }
    
    // Calculate the aligned address
    char* block_start = reinterpret_cast<char*>(block) + header_size;
    size_t offset = reinterpret_cast<size_t>(block_start) % alignment;
    size_t padding = (offset == 0) ? 0 : (alignment - offset);
    
    // Calculate the actual aligned address
    char* aligned_ptr = block_start + padding;
    
    // Mark block as allocated
    block->is_free = false;
    block->alignment = alignment;
    block->padding = padding;
    remove_from_free_list(block);
    
    // Check if we need to split the block
    size_t used_size = size + padding;
    size_t min_split_size = sizeof(BlockHeader) + 32;
    
    if (block->size > used_size + min_split_size) {
        // Create a free block after the aligned allocation
        BlockHeader* split_block = reinterpret_cast<BlockHeader*>(aligned_ptr + size);
        
        split_block->is_free = true;
        split_block->size = block->size - used_size - sizeof(BlockHeader);
        split_block->next = nullptr;
        split_block->block_id = next_block_id++;
        split_block->alignment = 0;
        split_block->padding = 0;
        
        block->size = used_size;
        insert_free_block(split_block);
        
        std::cout << "Block split for aligned allocation: used = " << used_size 
                  << " bytes, leftover = " << split_block->size << " bytes" << std::endl;
    } else {
        block->size = used_size;
    }
    
    std::cout << "✅ xmalloc_aligned: " << size << " bytes aligned to " << alignment 
              << " bytes (padding: " << padding << " bytes)" << std::endl;
    
    return aligned_ptr;
}

void MemoryAllocator::xfree_all() {
    std::lock_guard<std::mutex> lock(allocator_mutex);
    
    if (!initialized) {
        std::cout << "🔍 Memory pool not initialized" << std::endl;
        return;
    }
    
    std::cout << "🧹 Freeing all allocations..." << std::endl;
    
    // Reset the memory pool to initial state
    char* current_pos = memory_pool;
    int freed_count = 0;
    
    while (current_pos < memory_pool + POOL_SIZE) {
        BlockHeader* header = reinterpret_cast<BlockHeader*>(current_pos);
        
        if (!header->is_free) {
            header->is_free = true;
            freed_count++;
        }
        
        current_pos += sizeof(BlockHeader) + header->size;
    }
    
    // Reinitialize the free list
    BlockHeader* first_block = reinterpret_cast<BlockHeader*>(memory_pool);
    first_block->is_free = true;
    first_block->size = POOL_SIZE - sizeof(BlockHeader);
    first_block->next = nullptr;
    first_block->block_id = 0;
    first_block->alignment = 0;
    first_block->padding = 0;
    
    free_list = first_block;
    next_block_id = 1;
    
    std::cout << "✅ xfree_all: freed " << freed_count << " blocks, reset pool to initial state" << std::endl;
}

// Debug allocation functions implementation
#ifdef DEBUG_MEMORY

void* MemoryAllocator::xmalloc_debug(size_t size, const char* file, int line) {
    std::lock_guard<std::mutex> lock(allocator_mutex);
    
    if (!initialized) {
        initialize_pool();
    }
    
    if (size == 0) {
        std::cout << "❌ Error: Cannot allocate 0 bytes" << std::endl;
        return nullptr;
    }
    
    if (size > POOL_SIZE - sizeof(BlockHeader)) {
        std::cout << "❌ Error: Requested size " << size << " bytes exceeds pool capacity" << std::endl;
        return nullptr;
    }
    
    BlockHeader* block = find_free_block(size);
    if (block == nullptr) {
        std::cout << "❌ Error: No free block available for " << size << " bytes" << std::endl;
        return nullptr;
    }
    
    // Mark block as allocated
    block->is_free = false;
    block->alignment = 0;
    block->padding = 0;
    
    // Store debug information
    block->alloc_file = std::string(file);
    block->alloc_line = line;
    block->free_file = "";
    block->free_line = 0;
    
    remove_from_free_list(block);
    
    // Check if block splitting is beneficial
    size_t min_split_size = sizeof(BlockHeader) + 32;
    if (block->size > size + min_split_size) {
        BlockHeader* split_block = reinterpret_cast<BlockHeader*>(
            reinterpret_cast<char*>(block) + sizeof(BlockHeader) + size);
        
        split_block->is_free = true;
        split_block->size = block->size - size - sizeof(BlockHeader);
        split_block->next = nullptr;
        split_block->block_id = next_block_id++;
        split_block->alignment = 0;
        split_block->padding = 0;
        
        block->size = size;
        insert_free_block(split_block);
        
        std::cout << "Block split: allocated = " << size << " bytes, leftover = " 
                  << split_block->size << " bytes" << std::endl;
    }
    
    void* result = reinterpret_cast<char*>(block) + sizeof(BlockHeader);
    print_block_info(block, "ALLOCATED");
    
    std::cout << "🔍 Debug: Allocated at " << file << ":" << line << std::endl;
    return result;
}

void MemoryAllocator::xfree_debug(void* ptr, const char* file, int line) {
    std::lock_guard<std::mutex> lock(allocator_mutex);
    
    if (!initialized) {
        std::cout << "❌ Error: Memory pool not initialized" << std::endl;
        return;
    }
    
    if (ptr == nullptr) {
        std::cout << "❌ Error: Cannot free null pointer" << std::endl;
        return;
    }
    
    // Enhanced invalid pointer detection
    if (!is_valid_pointer(ptr)) {
        std::cout << "\033[31mERROR: Invalid pointer passed to xfree()!\033[0m" << std::endl;
        std::cout << "   Pointer: " << ptr << " is outside memory pool bounds" << std::endl;
        std::cout << "   Called from: " << file << ":" << line << std::endl;
        return;
    }
    
    BlockHeader* block = get_block_header(ptr);
    if (block == nullptr) {
        std::cout << "\033[31mERROR: Invalid pointer passed to xfree()!\033[0m" << std::endl;
        std::cout << "   Pointer: " << ptr << " does not point to a valid block header" << std::endl;
        std::cout << "   Called from: " << file << ":" << line << std::endl;
        return;
    }
    
    // Enhanced double-free detection
    if (block->is_free) {
        std::cout << "\033[31mERROR: Double-free detected at address " << ptr << "!\033[0m" << std::endl;
        std::cout << "   Block ID: " << block->block_id << std::endl;
        std::cout << "   Block Size: " << block->size << " bytes" << std::endl;
        std::cout << "   Originally allocated at: " << block->alloc_file << ":" << block->alloc_line << std::endl;
        std::cout << "   Previously freed at: " << block->free_file << ":" << block->free_line << std::endl;
        std::cout << "   Called from: " << file << ":" << line << std::endl;
        return;
    }
    
    // Store debug information
    block->free_file = std::string(file);
    block->free_line = line;
    
    // Mark as free
    block->is_free = true;
    insert_free_block(block);
    
    print_block_info(block, "FREED");
    std::cout << "🔍 Debug: Freed at " << file << ":" << line << std::endl;
    
    // Automatic coalescing with adjacent free blocks
    coalesce_with_next(block);
    coalesce_with_previous(block);
}

void* MemoryAllocator::xcalloc_debug(size_t num, size_t size, const char* file, int line) {
    std::lock_guard<std::mutex> lock(allocator_mutex);
    
    if (!initialized) {
        initialize_pool();
    }
    
    // Check for overflow
    if (num > 0 && size > SIZE_MAX / num) {
        std::cout << "❌ Error: xcalloc overflow - num * size exceeds maximum size" << std::endl;
        return nullptr;
    }
    
    size_t total_size = num * size;
    if (total_size == 0) {
        std::cout << "❌ Error: Cannot allocate 0 bytes with xcalloc" << std::endl;
        return nullptr;
    }
    
    if (total_size > POOL_SIZE - sizeof(BlockHeader)) {
        std::cout << "❌ Error: Requested size " << total_size << " bytes exceeds pool capacity" << std::endl;
        return nullptr;
    }
    
    // Find a free block
    BlockHeader* block = find_free_block(total_size);
    if (block == nullptr) {
        std::cout << "❌ Error: No free block available for " << total_size << " bytes" << std::endl;
        return nullptr;
    }
    
    // Mark block as allocated
    block->is_free = false;
    block->alignment = 0;
    block->padding = 0;
    
    // Store debug information
    block->alloc_file = std::string(file);
    block->alloc_line = line;
    block->free_file = "";
    block->free_line = 0;
    
    remove_from_free_list(block);
    
    // Check if block splitting is beneficial
    size_t min_split_size = sizeof(BlockHeader) + 32;
    if (block->size > total_size + min_split_size) {
        BlockHeader* split_block = reinterpret_cast<BlockHeader*>(
            reinterpret_cast<char*>(block) + sizeof(BlockHeader) + total_size);
        
        split_block->is_free = true;
        split_block->size = block->size - total_size - sizeof(BlockHeader);
        split_block->next = nullptr;
        split_block->block_id = next_block_id++;
        split_block->alignment = 0;
        split_block->padding = 0;
        
        block->size = total_size;
        insert_free_block(split_block);
        
        std::cout << "Block split: allocated = " << total_size << " bytes, leftover = " 
                  << split_block->size << " bytes" << std::endl;
    }
    
    void* result = reinterpret_cast<char*>(block) + sizeof(BlockHeader);
    print_block_info(block, "CALLOC");
    
    // Initialize all bytes to 0
    std::memset(result, 0, total_size);
    
    std::cout << "✅ xcalloc successful: " << num << " * " << size << " = " << total_size << " bytes (zeroed)" << std::endl;
    std::cout << "🔍 Debug: Allocated at " << file << ":" << line << std::endl;
    return result;
}

void* MemoryAllocator::xrealloc_debug(void* ptr, size_t new_size, const char* file, int line) {
    std::lock_guard<std::mutex> lock(allocator_mutex);
    
    if (!initialized) {
        initialize_pool();
    }
    
    // If ptr is nullptr, behave like xmalloc
    if (ptr == nullptr) {
        std::cout << "🔄 xrealloc: nullptr provided, allocating " << new_size << " bytes" << std::endl;
        return xmalloc_debug(new_size, file, line);
    }
    
    // If new_size is 0, behave like xfree
    if (new_size == 0) {
        std::cout << "🔄 xrealloc: new_size is 0, freeing pointer" << std::endl;
        xfree_debug(ptr, file, line);
        return nullptr;
    }
    
    // Validate the pointer
    if (!is_valid_pointer(ptr)) {
        std::cout << "❌ Error: xrealloc - invalid pointer address" << std::endl;
        return nullptr;
    }
    
    BlockHeader* block = get_block_header(ptr);
    if (block == nullptr) {
        std::cout << "❌ Error: xrealloc - pointer not found in memory pool" << std::endl;
        return nullptr;
    }
    
    size_t old_size = block->size;
    
    // If new size is smaller or equal, keep the existing block
    if (new_size <= old_size) {
        std::cout << "🔄 xrealloc: new size (" << new_size << ") <= old size (" << old_size << "), keeping existing block" << std::endl;
        return ptr;
    }
    
    // Try to extend in-place if there's a free block right after
    BlockHeader* next_block = get_next_block(block);
    if (next_block != nullptr && next_block->is_free) {
        size_t available_space = next_block->size + sizeof(BlockHeader);
        if (new_size <= old_size + available_space) {
            // We can extend in-place
            size_t needed_extra = new_size - old_size;
            
            // Remove next block from free list
            remove_from_free_list(next_block);
            
            // Extend current block
            block->size = new_size;
            
            // If there's still space left, create a new free block
            if (available_space > needed_extra + sizeof(BlockHeader)) {
                BlockHeader* new_free_block = reinterpret_cast<BlockHeader*>(
                    reinterpret_cast<char*>(block) + sizeof(BlockHeader) + new_size);
                
                new_free_block->is_free = true;
                new_free_block->size = available_space - needed_extra - sizeof(BlockHeader);
                new_free_block->next = nullptr;
                new_free_block->block_id = next_block_id++;
                new_free_block->alignment = 0;
                new_free_block->padding = 0;
                
                insert_free_block(new_free_block);
            }
            
            std::cout << "✅ xrealloc: extended in-place from " << old_size << " to " << new_size << " bytes" << std::endl;
            return ptr;
        }
    }
    
    // Cannot extend in-place, need to allocate new block and copy
    std::cout << "🔄 xrealloc: allocating new block of " << new_size << " bytes and copying data" << std::endl;
    
    // Allocate new block
    BlockHeader* new_block = find_free_block(new_size);
    if (new_block == nullptr) {
        std::cout << "❌ Error: xrealloc failed - no free block available" << std::endl;
        return nullptr;
    }
    
    // Mark new block as allocated
    new_block->is_free = false;
    new_block->alignment = 0;
    new_block->padding = 0;
    
    // Store debug information
    new_block->alloc_file = std::string(file);
    new_block->alloc_line = line;
    new_block->free_file = "";
    new_block->free_line = 0;
    
    remove_from_free_list(new_block);
    
    // Check if block splitting is beneficial
    size_t min_split_size = sizeof(BlockHeader) + 32;
    if (new_block->size > new_size + min_split_size) {
        BlockHeader* split_block = reinterpret_cast<BlockHeader*>(
            reinterpret_cast<char*>(new_block) + sizeof(BlockHeader) + new_size);
        
        split_block->is_free = true;
        split_block->size = new_block->size - new_size - sizeof(BlockHeader);
        split_block->next = nullptr;
        split_block->block_id = next_block_id++;
        split_block->alignment = 0;
        split_block->padding = 0;
        
        new_block->size = new_size;
        insert_free_block(split_block);
    }
    
    void* new_ptr = reinterpret_cast<char*>(new_block) + sizeof(BlockHeader);
    print_block_info(new_block, "REALLOC_NEW");
    
    // Copy old data to new location
    std::memcpy(new_ptr, ptr, old_size);
    
    // Free the old block
    block->is_free = true;
    insert_free_block(block);
    print_block_info(block, "REALLOC_FREE");
    
    // Automatic coalescing with adjacent free blocks
    coalesce_with_next(block);
    coalesce_with_previous(block);
    
    std::cout << "✅ xrealloc: copied " << old_size << " bytes to new location" << std::endl;
    std::cout << "🔍 Debug: Reallocated at " << file << ":" << line << std::endl;
    return new_ptr;
}

void* MemoryAllocator::xmalloc_aligned_debug(size_t size, size_t alignment, const char* file, int line) {
    std::lock_guard<std::mutex> lock(allocator_mutex);
    
    if (!initialized) {
        initialize_pool();
    }
    
    if (size == 0) {
        std::cout << "❌ Error: Cannot allocate 0 bytes with xmalloc_aligned" << std::endl;
        return nullptr;
    }
    
    // Check if alignment is a power of 2
    if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
        std::cout << "❌ Error: xmalloc_aligned - alignment must be a power of 2" << std::endl;
        return nullptr;
    }
    
    // Calculate required size including padding for alignment
    size_t header_size = sizeof(BlockHeader);
    size_t total_size = size + header_size + alignment - 1;
    
    if (total_size > POOL_SIZE - sizeof(BlockHeader)) {
        std::cout << "❌ Error: xmalloc_aligned - requested size exceeds pool capacity" << std::endl;
        return nullptr;
    }
    
    // Find a block that can accommodate the aligned allocation
    BlockHeader* block = find_free_block(total_size);
    if (block == nullptr) {
        std::cout << "❌ Error: xmalloc_aligned - no free block available" << std::endl;
        return nullptr;
    }
    
    // Calculate the aligned address
    char* block_start = reinterpret_cast<char*>(block) + header_size;
    size_t offset = reinterpret_cast<size_t>(block_start) % alignment;
    size_t padding = (offset == 0) ? 0 : (alignment - offset);
    
    // Calculate the actual aligned address
    char* aligned_ptr = block_start + padding;
    
    // Mark block as allocated
    block->is_free = false;
    block->alignment = alignment;
    block->padding = padding;
    
    // Store debug information
    block->alloc_file = std::string(file);
    block->alloc_line = line;
    block->free_file = "";
    block->free_line = 0;
    
    remove_from_free_list(block);
    
    // Check if we need to split the block
    size_t used_size = size + padding;
    size_t min_split_size = sizeof(BlockHeader) + 32;
    
    if (block->size > used_size + min_split_size) {
        // Create a free block after the aligned allocation
        BlockHeader* split_block = reinterpret_cast<BlockHeader*>(aligned_ptr + size);
        
        split_block->is_free = true;
        split_block->size = block->size - used_size - sizeof(BlockHeader);
        split_block->next = nullptr;
        split_block->block_id = next_block_id++;
        split_block->alignment = 0;
        split_block->padding = 0;
        
        block->size = used_size;
        insert_free_block(split_block);
        
        std::cout << "Block split for aligned allocation: used = " << used_size 
                  << " bytes, leftover = " << split_block->size << " bytes" << std::endl;
    } else {
        block->size = used_size;
    }
    
    std::cout << "✅ xmalloc_aligned: " << size << " bytes aligned to " << alignment 
              << " bytes (padding: " << padding << " bytes)" << std::endl;
    std::cout << "🔍 Debug: Allocated at " << file << ":" << line << std::endl;
    
    return aligned_ptr;
}

#endif // DEBUG_MEMORY
