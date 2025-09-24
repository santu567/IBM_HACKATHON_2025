# Custom Memory Allocator

A professional-grade custom memory allocator implemented in C++ for macOS. This project demonstrates memory management concepts with a clean, interactive command-line interface.

## 🚀 Features

- **2MB Memory Pool**: Pre-allocated memory pool with 2048 blocks of 1KB each
- **First-Fit Strategy**: Efficient memory allocation using first-fit algorithm
- **Memory Leak Detection**: Built-in leak detection and reporting
- **Professional CLI**: Clean, interactive command-line interface
- **Error Handling**: Comprehensive error checking for invalid operations
- **Block Tracking**: Unique block IDs and address tracking for debugging
- **Statistics**: Detailed memory usage statistics and reporting

## 🏗️ Architecture

### Memory Pool Structure
```
┌─────────────────────────────────────────────────────────────┐
│                    2MB Memory Pool                          │
├─────────────┬─────────────┬─────────────┬─────────────────--┤
│ BlockHeader │ User Data   │ BlockHeader │ User Data         │
│ (Metadata)  │ (1KB)       │ (Metadata)  │  (1KB)            │
└─────────────┴─────────────┴─────────────┴─────────────────--┘
```

### BlockHeader Structure
```cpp
struct BlockHeader {
    bool is_free;        // Block allocation status
    size_t size;         // Size of user data
    BlockHeader* next;   // Next block in free list
    uint32_t block_id;   // Unique block identifier
};
```

## 📋 Commands

| Command | Description | Example |
|---------|-------------|---------|
| `alloc <size>` | Allocate memory block | `alloc 1024` |
| `free <address>` | Free memory block | `free 0x12345678` |
| `leakcheck` | Check for memory leaks | `leakcheck` |
| `stats` | Show memory statistics | `stats` |
| `help` | Show help message | `help` |
| `exit` | Exit the program | `exit` |

## 🛠️ Building

### Prerequisites
- macOS
- clang++ compiler
- Make

### Build Commands
```bash
# Build the project
make

# Build and run
make run

# Clean build artifacts
make clean

# Run tests
make test
```

## 🎯 Usage Examples

### Basic Allocation
```bash
🔧 allocator> alloc 1024
🔧 Memory pool initialized: 2MB (2048 blocks of 1024 bytes each)
[ALLOCATED] Block #  0 | Addr: 0x7fff5c000000 | Size:   1024 bytes
✅ Allocation successful
```

### Memory Statistics
```bash
🔧 allocator> stats
📊 Memory Allocator Statistics:
   Total Pool Size: 2097152 bytes (2 MB)
   Total Blocks:         1
   Used Blocks:          1 (100%)
   Free Blocks:          0 (0%)
   Used Memory:       1024 bytes
   Free Memory:    2096128 bytes
```

### Memory Leak Detection
```bash
🔧 allocator> leakcheck
⚠️  Memory leak detected! 1 blocks not freed:
[LEAKED] Block #  0 | Addr: 0x7fff5c000000 | Size:   1024 bytes
```

## 🔧 Implementation Details

### Core Functions

#### `void* xmalloc(size_t size)`
- Allocates memory using first-fit strategy
- Splits large blocks when necessary
- Returns pointer to user data area
- Handles allocation errors gracefully

#### `void xfree(void* ptr)`
- Validates pointer before freeing
- Detects double-free attempts
- Merges adjacent free blocks
- Updates free list structure

#### `void leakcheck()`
- Scans entire memory pool
- Identifies allocated blocks
- Reports memory leaks with details
- Provides block addresses for debugging

#### `void stats()`
- Calculates memory usage statistics
- Shows block allocation percentages
- Displays total/free/used memory
- Provides comprehensive pool overview

### Error Handling
- **Invalid Size**: Rejects zero or oversized allocations
- **Double Free**: Detects and prevents double-free errors
- **Bad Pointer**: Validates pointer addresses
- **Pool Overflow**: Prevents allocation beyond pool capacity

## 🧪 Testing

### Manual Testing
```bash
# Start the allocator
./memory_allocator

# Test commands (copy from testcases.txt)
alloc 1024
alloc 2048
stats
free 0x7fff5c000000
leakcheck
exit
```

### Automated Testing
```bash
# Run basic tests
make test
```

## 📊 Sample Output

```
╔══════════════════════════════════════════════════════════════╗
║                    Custom Memory Allocator                   ║
║                        Version 1.0                           ║
╚══════════════════════════════════════════════════════════════╝

📋 Available Commands:
   alloc <size>     - Allocate memory block of specified size (bytes)
   free <address>   - Free memory block at specified address (hex)
   leakcheck        - Check for memory leaks
   stats            - Show memory allocation statistics
   help             - Show this help message
   exit             - Exit the program

💡 Examples:
   alloc 1024       - Allocate 1KB block
   free 0x12345678  - Free block at address 0x12345678

🔧 allocator> alloc 1024
🔧 Memory pool initialized: 2MB (2048 blocks of 1024 bytes each)
[ALLOCATED] Block #  0 | Addr: 0x7fff5c000000 | Size:   1024 bytes
✅ Allocation successful

🔧 allocator> stats
📊 Memory Allocator Statistics:
   Total Pool Size: 2097152 bytes (2 MB)
   Total Blocks:         1
   Used Blocks:          1 (100%)
   Free Blocks:          0 (0%)
   Used Memory:       1024 bytes
   Free Memory:    2096128 bytes
```

## 🎓 Educational Value

This project demonstrates:
- **Memory Management**: Understanding heap allocation strategies
- **Data Structures**: Linked list implementation for free blocks
- **Error Handling**: Robust error detection and reporting
- **System Programming**: Low-level memory manipulation
- **CLI Design**: User-friendly command-line interfaces
- **Debugging**: Memory leak detection and debugging tools

## 📝 License

This project is for educational purposes. Feel free to use and modify for learning memory management concepts.

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

---

**Happy Memory Management! 🧠💾** 
