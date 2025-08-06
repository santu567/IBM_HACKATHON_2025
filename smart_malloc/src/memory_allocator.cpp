#include "memory_allocator.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <vector>

// Static member initialization
char MemoryAllocator::memory_pool[POOL_SIZE];
BlockHeader* MemoryAllocator::free_list = nullptr;
uint32_t MemoryAllocator::next_block_id = 0;
bool MemoryAllocator::initialized = false;

void MemoryAllocator::initialize_pool() {
    if (initialized) return;
    
    // Initialize the first block header
    BlockHeader* first_block = reinterpret_cast<BlockHeader*>(memory_pool);
    first_block->is_free = true;
    first_block->size = POOL_SIZE - sizeof(BlockHeader);
    first_block->next = nullptr;
    first_block->block_id = next_block_id++;
    
    free_list = first_block;
    initialized = true;
    
    std::cout << "🔧 Memory pool initialized: " << POOL_SIZE / (1024 * 1024) 
              << "MB (" << NUM_BLOCKS << " blocks of " << BLOCK_SIZE << " bytes each)" << std::endl;
}

BlockHeader* MemoryAllocator::find_free_block(size_t size) {
    BlockHeader* current = free_list;
    
    // First-fit strategy
    while (current != nullptr) {
        if (current->is_free && current->size >= size) {
            return current;  // First fit found
        }
        current = current->next;
    }
    
    return nullptr;  // No suitable block found
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
    remove_from_free_list(block);
    
    // If the block is much larger than needed, split it
    if (block->size > size + sizeof(BlockHeader) + 64) {  // Minimum split size
        BlockHeader* new_block = reinterpret_cast<BlockHeader*>(
            reinterpret_cast<char*>(block) + sizeof(BlockHeader) + size);
        
        new_block->is_free = true;
        new_block->size = block->size - size - sizeof(BlockHeader);
        new_block->next = nullptr;
        new_block->block_id = next_block_id++;
        
        block->size = size;
        insert_free_block(new_block);
    }
    
    void* result = reinterpret_cast<char*>(block) + sizeof(BlockHeader);
    print_block_info(block, "ALLOCATED");
    
    return result;
}

void MemoryAllocator::xfree(void* ptr) {
    if (!initialized) {
        std::cout << "❌ Error: Memory pool not initialized" << std::endl;
        return;
    }
    
    if (ptr == nullptr) {
        std::cout << "❌ Error: Cannot free null pointer" << std::endl;
        return;
    }
    
    if (!is_valid_pointer(ptr)) {
        std::cout << "❌ Error: Invalid pointer address" << std::endl;
        return;
    }
    
    BlockHeader* block = get_block_header(ptr);
    if (block == nullptr) {
        std::cout << "❌ Error: Pointer not found in memory pool" << std::endl;
        return;
    }
    
    if (block->is_free) {
        std::cout << "❌ Error: Double free detected" << std::endl;
        return;
    }
    
    // Mark as free
    block->is_free = true;
    insert_free_block(block);
    
    print_block_info(block, "FREED");
    
    // Merge with adjacent free blocks
    // This is a simplified merge - in a real implementation you'd need more complex logic
}

void MemoryAllocator::leakcheck() {
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
        }
    }
}

void MemoryAllocator::stats() {
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
              << POOL_SIZE / (1024 * 1024) << " MB)" << std::endl;
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
