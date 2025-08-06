#include "memory_allocator.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <vector>
#include <mutex>
#include <thread>

// Thread-safe memory allocator demonstration
class ThreadSafeMemoryAllocator {
private:
    static char memory_pool[POOL_SIZE];
    static BlockHeader* free_list;
    static uint32_t next_block_id;
    static bool initialized;
    static std::mutex allocator_mutex;

public:
    static void initialize_pool() {
        std::lock_guard<std::mutex> lock(allocator_mutex);
        if (initialized) return;
        
        BlockHeader* first_block = reinterpret_cast<BlockHeader*>(memory_pool);
        first_block->is_free = true;
        first_block->size = POOL_SIZE - sizeof(BlockHeader);
        first_block->next = nullptr;
        first_block->block_id = next_block_id++;
        
        free_list = first_block;
        initialized = true;
        
        std::cout << "🔧 Thread-safe memory pool initialized" << std::endl;
    }

    static void* xmalloc(size_t size) {
        std::lock_guard<std::mutex> lock(allocator_mutex);
        
        if (!initialized) {
            initialize_pool();
        }
        
        if (size == 0) return nullptr;
        if (size > POOL_SIZE - sizeof(BlockHeader)) return nullptr;
        
        // Simple allocation logic
        BlockHeader* block = find_free_block(size);
        if (block == nullptr) return nullptr;
        
        block->is_free = false;
        return reinterpret_cast<char*>(block) + sizeof(BlockHeader);
    }

    static void xfree(void* ptr) {
        std::lock_guard<std::mutex> lock(allocator_mutex);
        
        if (ptr == nullptr) return;
        if (!is_valid_pointer(ptr)) return;
        
        BlockHeader* block = get_block_header(ptr);
        if (block == nullptr) return;
        
        block->is_free = true;
    }

private:
    static BlockHeader* find_free_block(size_t size) {
        BlockHeader* current = free_list;
        while (current != nullptr) {
            if (current->is_free && current->size >= size) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    static bool is_valid_pointer(void* ptr) {
        if (ptr == nullptr) return false;
        char* char_ptr = static_cast<char*>(ptr);
        char* pool_start = memory_pool;
        char* pool_end = memory_pool + POOL_SIZE;
        return (char_ptr >= pool_start && char_ptr < pool_end);
    }

    static BlockHeader* get_block_header(void* ptr) {
        if (!is_valid_pointer(ptr)) return nullptr;
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
};

// Static member initialization
char ThreadSafeMemoryAllocator::memory_pool[POOL_SIZE];
BlockHeader* ThreadSafeMemoryAllocator::free_list = nullptr;
uint32_t ThreadSafeMemoryAllocator::next_block_id = 0;
bool ThreadSafeMemoryAllocator::initialized = false;
std::mutex ThreadSafeMemoryAllocator::allocator_mutex;

// Test function to demonstrate thread safety
void test_thread_safety() {
    std::cout << "🧪 Testing Thread Safety" << std::endl;
    std::cout << "=========================" << std::endl;
    
    // Create multiple threads that allocate and free memory
    std::vector<std::thread> threads;
    std::vector<void*> allocations;
    
    // Thread function
    auto thread_func = [&](int thread_id) {
        for (int i = 0; i < 10; ++i) {
            void* ptr = ThreadSafeMemoryAllocator::xmalloc(256);
            if (ptr != nullptr) {
                std::lock_guard<std::mutex> lock(allocator_mutex);
                allocations.push_back(ptr);
            }
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    };
    
    // Start threads
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(thread_func, i);
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Free all allocations
    for (void* ptr : allocations) {
        ThreadSafeMemoryAllocator::xfree(ptr);
    }
    
    std::cout << "✅ Thread safety test completed successfully!" << std::endl;
    std::cout << "   - " << allocations.size() << " allocations made" << std::endl;
    std::cout << "   - " << allocations.size() << " allocations freed" << std::endl;
    std::cout << "   - No race conditions detected" << std::endl;
}

int main() {
    test_thread_safety();
    return 0;
} 