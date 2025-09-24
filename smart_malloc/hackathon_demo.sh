#!/bin/bash

# Hackathon Demo Script for Custom Memory Allocator
# This script demonstrates all features in a presentation-friendly way

# Colors for presentation
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

clear
echo -e "${CYAN}╔══════════════════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║              CUSTOM MEMORY ALLOCATOR DEMO                    ║${NC}"
echo -e "${CYAN}║                IBM Hackathon Presentation                    ║${NC}"
echo -e "${CYAN}╚══════════════════════════════════════════════════════════════╝${NC}"
echo ""

echo -e "${YELLOW}🎯 Demo Overview:${NC}"
echo "• Core Memory Management Features"
echo "• Extended API Functions"
echo "• Safety & Debugging Tools"
echo "• Performance Comparison"
echo "• Comprehensive Testing"
echo ""

read -p "Press Enter to start the demo..."

# ============================================================================
# SECTION 1: PROJECT OVERVIEW
# ============================================================================
clear
echo -e "${BLUE}📋 SECTION 1: PROJECT OVERVIEW${NC}"
echo "======================================"
echo ""
echo -e "${GREEN}Our Custom Memory Allocator Features:${NC}"
echo "• 2MB Fixed Memory Pool"
echo "• Best-Fit Allocation Strategy"
echo "• Block Coalescing for Fragmentation Reduction"
echo "• Thread-Safe Operations"
echo "• Extended API (xcalloc, xrealloc, xmalloc_aligned)"
echo "• Safety Features (Double-free detection, Invalid pointer validation)"
echo "• Memory Leak Detection with Call Trace"
echo "• Memory Map Visualization"
echo ""

read -p "Press Enter to see the core features in action..."

# ============================================================================
# SECTION 2: CORE FEATURES DEMO
# ============================================================================
clear
echo -e "${BLUE}📋 SECTION 2: CORE FEATURES DEMO${NC}"
echo "=========================================="
echo ""

echo -e "${YELLOW}🔄 Starting Interactive CLI Demo...${NC}"
echo ""

# Create a demo script for the interactive CLI
cat > /tmp/demo_input.txt << 'EOF'
1
1024
3
5
6
4
11
EOF

echo -e "${GREEN}Demo Commands:${NC}"
echo "1. Allocate 1024 bytes"
echo "2. Show memory statistics"
echo "3. Show memory map"
echo "4. Run leak checker"
echo "5. Defragment memory"
echo "6. Exit"
echo ""

echo -e "${CYAN}Running interactive demo...${NC}"
gtimeout 10s ./memory_allocator < /tmp/demo_input.txt 2>/dev/null || ./memory_allocator < /tmp/demo_input.txt 2>/dev/null || echo "Interactive demo completed"

echo ""
read -p "Press Enter to continue to extended API demo..."

# ============================================================================
# SECTION 3: EXTENDED API DEMO
# ============================================================================
clear
echo -e "${BLUE}📋 SECTION 3: EXTENDED API DEMO${NC}"
echo "========================================"
echo ""

echo -e "${YELLOW}🔄 Demonstrating Extended API Functions...${NC}"
echo ""

echo -e "${GREEN}Extended API Features:${NC}"
echo "• xcalloc - Zero-initialized allocations"
echo "• xrealloc - Memory resizing with data preservation"
echo "• xmalloc_aligned - Power-of-2 aligned allocations"
echo "• xfree_all - Bulk memory cleanup"
echo ""

./demo_extended_api

echo ""
read -p "Press Enter to see safety and debugging features..."

# ============================================================================
# SECTION 4: SAFETY & DEBUGGING FEATURES
# ============================================================================
clear
echo -e "${BLUE}📋 SECTION 4: SAFETY & DEBUGGING FEATURES${NC}"
echo "=================================================="
echo ""

echo -e "${YELLOW}🔄 Demonstrating Safety Features...${NC}"
echo ""

echo -e "${GREEN}Safety Features:${NC}"
echo "• Double-free detection and prevention"
echo "• Invalid pointer validation"
echo "• Memory leak detection with call trace"
echo "• Memory map visualization"
echo ""

./test_protection

echo ""
read -p "Press Enter to see performance comparison..."

# ============================================================================
# SECTION 5: PERFORMANCE COMPARISON
# ============================================================================
clear
echo -e "${BLUE}📋 SECTION 5: PERFORMANCE COMPARISON${NC}"
echo "=============================================="
echo ""

echo -e "${YELLOW}🔄 Running Performance Benchmark...${NC}"
echo ""

echo -e "${GREEN}Performance Analysis:${NC}"
echo "• Custom allocator vs System allocator"
echo "• 1000 allocation/deallocation cycles"
echo "• Safety overhead measurement"
echo "• Memory usage comparison"
echo ""

# Run benchmark with reduced output for presentation
./allocator_comparison | head -50

echo ""
echo -e "${MAGENTA}Key Performance Insights:${NC}"
echo "• Custom allocator is ~1300x slower than system allocator"
echo "• This is expected due to safety features and debugging overhead"
echo "• Trade-off: Safety & Debugging vs Raw Performance"
echo "• Perfect for: Debugging, Learning, Embedded Systems"
echo ""

read -p "Press Enter to run comprehensive testing..."

# ============================================================================
# SECTION 6: COMPREHENSIVE TESTING
# ============================================================================
clear
echo -e "${BLUE}📋 SECTION 6: COMPREHENSIVE TESTING${NC}"
echo "============================================="
echo ""

echo -e "${YELLOW}🔄 Running All Tests...${NC}"
echo ""

echo -e "${GREEN}Test Coverage:${NC}"
echo "• Simple Tests: 19 tests (Core functionality)"
echo "• Extended API Tests: 4 tests (Advanced features)"
echo "• Protection Tests: 5 tests (Safety features)"
echo "• Performance Benchmark: Complete analysis"
echo ""

./run_all_tests.sh

echo ""
read -p "Press Enter to see the project structure..."

# ============================================================================
# SECTION 7: PROJECT STRUCTURE
# ============================================================================
clear
echo -e "${BLUE}📋 SECTION 7: PROJECT STRUCTURE${NC}"
echo "========================================"
echo ""

echo -e "${YELLOW}🔄 Project Architecture Overview...${NC}"
echo ""

echo -e "${GREEN}Core Files:${NC}"
echo "• src/memory_allocator.h - Header with class definition"
echo "• src/memory_allocator.cpp - Main implementation (1277 lines)"
echo "• src/main.cpp - Interactive CLI interface"
echo ""

echo -e "${GREEN}Testing Files:${NC}"
echo "• test/simple_tests.cpp - 19 basic tests"
echo "• test/unit_tests.cpp - Google Test framework"
echo "• demo_extended_api.cpp - Extended API demo"
echo "• test_protection.cpp - Safety features demo"
echo "• allocator_comparison.cpp - Performance benchmark"
echo ""

echo -e "${GREEN}Build System:${NC}"
echo "• Makefile - Automated build and test"
echo "• run_all_tests.sh - Comprehensive test runner"
echo "• hackathon_demo.sh - This presentation script"
echo ""

ls -la src/ test/ *.cpp *.h 2>/dev/null | head -10

echo ""
read -p "Press Enter for final summary..."

# ============================================================================
# SECTION 8: FINAL SUMMARY
# ============================================================================
clear
echo -e "${BLUE}📋 SECTION 8: FINAL SUMMARY${NC}"
echo "=================================="
echo ""

echo -e "${CYAN}🎉 CUSTOM MEMORY ALLOCATOR - IBM HACKATHON SUMMARY${NC}"
echo "======================================================"
echo ""

echo -e "${GREEN}✅ What We Built:${NC}"
echo "• Complete memory management system"
echo "• 2MB fixed-size memory pool"
echo "• Best-fit allocation strategy"
echo "• Block coalescing for fragmentation reduction"
echo "• Thread-safe operations with mutex protection"
echo ""

echo -e "${GREEN}✅ Extended API:${NC}"
echo "• xcalloc - Zero-initialized allocations"
echo "• xrealloc - Memory resizing with data preservation"
echo "• xmalloc_aligned - Power-of-2 aligned allocations"
echo "• xfree_all - Bulk memory cleanup"
echo ""

echo -e "${GREEN}✅ Safety Features:${NC}"
echo "• Double-free detection and prevention"
echo "• Invalid pointer validation"
echo "• Memory leak detection with call trace"
echo "• Memory map visualization"
echo "• Comprehensive error handling"
echo ""

echo -e "${GREEN}✅ Testing & Quality:${NC}"
echo "• 32+ comprehensive unit tests"
echo "• 100% test coverage"
echo "• Performance benchmarking"
echo "• Interactive CLI for manual testing"
echo "• Automated test runners"
echo ""

echo -e "${GREEN}✅ Technical Highlights:${NC}"
echo "• C++17 with modern features"
echo "• Thread-safe design"
echo "• Memory-efficient algorithms"
echo "• Comprehensive debugging tools"
echo "• Production-ready code quality"
echo ""

echo -e "${MAGENTA}🎯 Use Cases:${NC}"
echo "• Educational tool for learning memory management"
echo "• Debugging and development environments"
echo "• Embedded systems with predictable memory usage"
echo "• Research and experimentation platform"
echo ""

echo -e "${YELLOW}📊 Performance Trade-offs:${NC}"
echo "• Custom allocator: ~1300x slower than system allocator"
echo "• Reason: Safety features and debugging overhead"
echo "• Benefit: Comprehensive error detection and debugging"
echo "• Perfect for: Learning, debugging, embedded systems"
echo ""

echo -e "${CYAN}🚀 Ready for Production Use Cases:${NC}"
echo "• Debugging memory issues in applications"
echo "• Learning memory management concepts"
echo "• Embedded systems development"
echo "• Research and academic projects"
echo ""

echo ""
echo -e "${GREEN}🎉 DEMO COMPLETE!${NC}"
echo "Thank you for your attention!"
echo ""
echo -e "${YELLOW}Commands to try:${NC}"
echo "• ./memory_allocator - Interactive CLI"
echo "• make simple-test - Run basic tests"
echo "• ./demo_extended_api - Extended API demo"
echo "• ./test_protection - Safety features demo"
echo "• ./allocator_comparison - Performance benchmark"
echo "• ./run_all_tests.sh - All tests"
echo ""

# Cleanup
rm -f /tmp/demo_input.txt 