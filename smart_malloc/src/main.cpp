#include "memory_allocator.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <limits>

//colors

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"

void print_banner() {
    std::cout << CYAN << "╔══════════════════════════════════════════════════════════════╗" << RESET << std::endl;
    std::cout << CYAN << "║                    Custom Memory Allocator                   ║" << RESET << std::endl;
    std::cout << CYAN << "║                        Version 2.0                           ║" << RESET << std::endl;
    std::cout << CYAN << "╚══════════════════════════════════════════════════════════════╝" << RESET << std::endl;
    std::cout << std::endl;
}

void print_menu() {
    std::cout << BLUE << "========== Custom Memory Allocator ==========" << RESET << std::endl;
    std::cout << "1. Allocate memory (xmalloc)" << std::endl;
    std::cout << "2. Free memory (xfree)" << std::endl;
    std::cout << "3. Show memory stats" << std::endl;
    std::cout << "4. Run leak checker" << std::endl;
    std::cout << "5. Show memory map" << std::endl;
    std::cout << "6. Defragment memory" << std::endl;
    std::cout << "7. Allocate zeroed memory (xcalloc)" << std::endl;
    std::cout << "8. Resize memory (xrealloc)" << std::endl;
    std::cout << "9. Allocate aligned memory (xmalloc_aligned)" << std::endl;
    std::cout << "10. Free all memory (xfree_all)" << std::endl;
    std::cout << "11. Exit" << std::endl;
    std::cout << BLUE << "=============================================" << RESET << std::endl;
    std::cout << "Enter your choice: ";
}

void print_success(const std::string& message) {
    std::cout << GREEN << "✅ " << message << RESET << std::endl;
}

void print_error(const std::string& message) {
    std::cout << RED << "❌ " << message << RESET << std::endl;
}

void print_warning(const std::string& message) {
    std::cout << YELLOW << "⚠️  " << message << RESET << std::endl;
}

void print_info(const std::string& message) {
    std::cout << BLUE << "ℹ️  " << message << RESET << std::endl;
}

uintptr_t parse_hex_address(const std::string& hex_str) {
    if (hex_str.substr(0, 2) == "0x" || hex_str.substr(0, 2) == "0X") {
        return std::stoull(hex_str, nullptr, 16);
    } else {
        return std::stoull(hex_str, nullptr, 16);
    }
}

void show_memory_map() {
    MemoryAllocator::show_memory_map();
}

void handle_allocate_memory() {
    std::cout << "Enter size to allocate (in bytes): ";
    std::string input;
    std::getline(std::cin, input);
    
    try {
        size_t size = std::stoul(input);
        void* ptr = MemoryAllocator::xmalloc(size);
        
        if (ptr != nullptr) {
            print_success("Allocation successful");
        } else {
            print_error("Allocation failed - insufficient memory");
        }
    } catch (const std::exception& e) {
        print_error("Invalid size value");
    }
}

void handle_free_memory() {
    std::cout << "Enter memory address to free (hex format, e.g., 0x12345678): ";
    std::string input;
    std::getline(std::cin, input);
    
    try {
        uintptr_t addr = parse_hex_address(input);
        void* ptr = reinterpret_cast<void*>(addr);
        MemoryAllocator::xfree(ptr);
        print_success("Memory freed successfully");
    } catch (const std::exception& e) {
        print_error("Invalid address format");
    }
}

void handle_show_stats() {
    print_info("Memory Statistics:");
    MemoryAllocator::stats();
}

void handle_leak_check() {
    print_info("Running memory leak check...");
    MemoryAllocator::leakcheck();
}

void handle_defragment() {
    print_info("Starting memory defragmentation...");
    MemoryAllocator::defragment();
}

void handle_xcalloc() {
    std::cout << "Enter number of elements: ";
    std::string num_input;
    std::getline(std::cin, num_input);
    
    std::cout << "Enter size of each element (in bytes): ";
    std::string size_input;
    std::getline(std::cin, size_input);
    
    try {
        size_t num = std::stoul(num_input);
        size_t size = std::stoul(size_input);
        void* ptr = MemoryAllocator::xcalloc(num, size);
        
        if (ptr != nullptr) {
            print_success("xcalloc successful");
        } else {
            print_error("xcalloc failed");
        }
    } catch (const std::exception& e) {
        print_error("Invalid input values");
    }
}

void handle_xrealloc() {
    std::cout << "Enter memory address to resize (hex format, e.g., 0x12345678): ";
    std::string addr_input;
    std::getline(std::cin, addr_input);
    
    std::cout << "Enter new size (in bytes): ";
    std::string size_input;
    std::getline(std::cin, size_input);
    
    try {
        uintptr_t addr = parse_hex_address(addr_input);
        void* ptr = reinterpret_cast<void*>(addr);
        size_t new_size = std::stoul(size_input);
        
        void* new_ptr = MemoryAllocator::xrealloc(ptr, new_size);
        
        if (new_ptr != nullptr) {
            print_success("xrealloc successful");
        } else {
            print_error("xrealloc failed");
        }
    } catch (const std::exception& e) {
        print_error("Invalid input values");
    }
}

void handle_xmalloc_aligned() {
    std::cout << "Enter size to allocate (in bytes): ";
    std::string size_input;
    std::getline(std::cin, size_input);
    
    std::cout << "Enter alignment (power of 2, e.g., 8, 16, 32): ";
    std::string align_input;
    std::getline(std::cin, align_input);
    
    try {
        size_t size = std::stoul(size_input);
        size_t alignment = std::stoul(align_input);
        
        void* ptr = MemoryAllocator::xmalloc_aligned(size, alignment);
        
        if (ptr != nullptr) {
            print_success("xmalloc_aligned successful");
        } else {
            print_error("xmalloc_aligned failed");
        }
    } catch (const std::exception& e) {
        print_error("Invalid input values");
    }
}

void handle_xfree_all() {
    print_info("Freeing all memory allocations...");
    MemoryAllocator::xfree_all();
    print_success("All memory freed");
}

int get_user_choice() {
    std::string input;
    std::getline(std::cin, input);
    
    try {
        int choice = std::stoi(input);
        if (choice >= 1 && choice <= 11) {
            return choice;
        } else {
            print_error("Invalid choice. Please enter a number between 1 and 11.");
            return -1;
        }
    } catch (const std::exception& e) {
        print_error("Invalid input. Please enter a number between 1 and 11.");
        return -1;
    }
}

int main() {
    print_banner();
    
    // Initialize the memory pool
    MemoryAllocator::initialize_pool();
    std::cout << std::endl;
    
    int choice;
    
    while (true) {
        print_menu();
        choice = get_user_choice();
        
        if (choice == -1) {
            continue;
        }
        
        std::cout << std::endl;
        
        switch (choice) {
            case 1:
                handle_allocate_memory();
                break;
            case 2:
                handle_free_memory();
                break;
            case 3:
                handle_show_stats();
                break;
            case 4:
                handle_leak_check();
                break;
            case 5:
                show_memory_map();
                break;
            case 6:
                handle_defragment();
                break;
            case 7:
                handle_xcalloc();
                break;
            case 8:
                handle_xrealloc();
                break;
            case 9:
                handle_xmalloc_aligned();
                break;
            case 10:
                handle_xfree_all();
                break;
            case 11:
                print_success("Goodbye!");
                return 0;
            default:
                print_error("Invalid choice");
                break;
        }
        
        std::cout << std::endl;
    }
    
    return 0;
}
