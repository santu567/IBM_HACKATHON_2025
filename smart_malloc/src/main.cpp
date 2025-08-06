#include "memory_allocator.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

void print_banner() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    Custom Memory Allocator                   ║" << std::endl;
    std::cout << "║                        Version 1.0                          ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

void print_help() {
    std::cout << "📋 Available Commands:" << std::endl;
    std::cout << "   alloc <size>     - Allocate memory block of specified size (bytes)" << std::endl;
    std::cout << "   free <address>    - Free memory block at specified address (hex)" << std::endl;
    std::cout << "   leakcheck         - Check for memory leaks" << std::endl;
    std::cout << "   stats             - Show memory allocation statistics" << std::endl;
    std::cout << "   help              - Show this help message" << std::endl;
    std::cout << "   exit              - Exit the program" << std::endl;
    std::cout << std::endl;
    std::cout << "💡 Examples:" << std::endl;
    std::cout << "   alloc 1024       - Allocate 1KB block" << std::endl;
    std::cout << "   free 0x12345678  - Free block at address 0x12345678" << std::endl;
    std::cout << std::endl;
}

void print_prompt() {
    std::cout << "🔧 allocator> ";
}

std::vector<std::string> split_command(const std::string& input) {
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    
    while (ss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

uintptr_t parse_hex_address(const std::string& hex_str) {
    if (hex_str.substr(0, 2) == "0x" || hex_str.substr(0, 2) == "0X") {
        return std::stoull(hex_str, nullptr, 16);
    } else {
        return std::stoull(hex_str, nullptr, 16);
    }
}

void handle_alloc_command(const std::vector<std::string>& tokens) {
    if (tokens.size() != 2) {
        std::cout << "❌ Error: Usage: alloc <size>" << std::endl;
        return;
    }
    
    try {
        size_t size = std::stoul(tokens[1]);
        void* ptr = MemoryAllocator::xmalloc(size);
        
        if (ptr != nullptr) {
            std::cout << "✅ Allocation successful" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ Error: Invalid size value" << std::endl;
    }
}

void handle_free_command(const std::vector<std::string>& tokens) {
    if (tokens.size() != 2) {
        std::cout << "❌ Error: Usage: free <address>" << std::endl;
        return;
    }
    
    try {
        uintptr_t addr = parse_hex_address(tokens[1]);
        void* ptr = reinterpret_cast<void*>(addr);
        MemoryAllocator::xfree(ptr);
    } catch (const std::exception& e) {
        std::cout << "❌ Error: Invalid address format" << std::endl;
    }
}

int main() {
    print_banner();
    print_help();
    
    std::string input;
    std::map<std::string, void*> allocated_blocks;  // For demo purposes
    
    while (true) {
        print_prompt();
        
        if (!std::getline(std::cin, input)) {
            break;
        }
        
        if (input.empty()) {
            continue;
        }
        
        std::vector<std::string> tokens = split_command(input);
        if (tokens.empty()) {
            continue;
        }
        
        std::string command = tokens[0];
        
        if (command == "alloc") {
            handle_alloc_command(tokens);
        } else if (command == "free") {
            handle_free_command(tokens);
        } else if (command == "leakcheck") {
            MemoryAllocator::leakcheck();
        } else if (command == "stats") {
            MemoryAllocator::stats();
        } else if (command == "help") {
            print_help();
        } else if (command == "exit") {
            std::cout << "👋 Goodbye!" << std::endl;
            break;
        } else {
            std::cout << "❌ Unknown command: " << command << std::endl;
            std::cout << "💡 Type 'help' for available commands" << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    return 0;
}
