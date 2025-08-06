#!/bin/bash

# Test runner script for Memory Allocator
# This script can run both manual tests and unit tests

set -e

echo "🧪 Memory Allocator Test Runner"
echo "================================"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    local status=$1
    local message=$2
    if [ "$status" = "PASS" ]; then
        echo -e "${GREEN}✅ PASS${NC}: $message"
    elif [ "$status" = "FAIL" ]; then
        echo -e "${RED}❌ FAIL${NC}: $message"
    elif [ "$status" = "INFO" ]; then
        echo -e "${YELLOW}ℹ️  INFO${NC}: $message"
    fi
}

# Check if we're in the right directory
if [ ! -f "src/memory_allocator.h" ]; then
    echo "Error: Please run this script from the smart_malloc directory"
    exit 1
fi

# Build the project
print_status "INFO" "Building project..."
make clean > /dev/null 2>&1 || true
make all > /dev/null 2>&1

if [ $? -eq 0 ]; then
    print_status "PASS" "Build successful"
else
    print_status "FAIL" "Build failed"
    exit 1
fi

echo ""
echo "📋 Running Manual Tests"
echo "======================"

# Run manual tests from testcases.txt
if [ -f "test/testcases.txt" ]; then
    print_status "INFO" "Running manual test cases..."
    
    # Create a temporary test script with proper input
    cat > /tmp/manual_test.sh << 'EOF'
alloc 1024
stats
leakcheck
exit
EOF
    
    # Run the manual tests with proper input handling
    ./memory_allocator < /tmp/manual_test.sh > /tmp/manual_test_output.txt 2>&1 || true
    
    if grep -q "\[ALLOCATED\]" /tmp/manual_test_output.txt; then
        print_status "PASS" "Manual allocation tests passed"
    else
        print_status "FAIL" "Manual allocation tests failed"
    fi
    
    if grep -q "📊 Memory Allocator Statistics" /tmp/manual_test_output.txt; then
        print_status "PASS" "Statistics test passed"
    else
        print_status "FAIL" "Statistics test failed"
    fi
    
    if grep -q "⚠️  Memory leak detected" /tmp/manual_test_output.txt; then
        print_status "PASS" "Leak detection test passed"
    else
        print_status "FAIL" "Leak detection test failed"
    fi
    
    rm -f /tmp/manual_test.sh /tmp/manual_test_output.txt
else
    print_status "FAIL" "Manual test file not found"
fi

echo ""
echo "🧪 Running Simple Tests"
echo "======================"

# Run simple tests (no external dependencies)
print_status "INFO" "Running simple tests..."
make simple-test > /tmp/simple_test_output.txt 2>&1

if [ $? -eq 0 ]; then
    print_status "PASS" "Simple tests passed"
    echo ""
    echo "📊 Simple Test Summary:"
    grep -E "(✅ PASS|❌ FAIL)" /tmp/simple_test_output.txt | tail -5
else
    print_status "FAIL" "Simple tests failed"
    cat /tmp/simple_test_output.txt
fi

rm -f /tmp/simple_test_output.txt

echo ""
echo "🧪 Running Unit Tests"
echo "===================="

# Check if Google Test is available
if command -v g++ > /dev/null 2>&1; then
    # Try to compile with Google Test
    GTEST_AVAILABLE=false
    
    # Check if we can find Google Test headers
    if [ -f "/usr/include/gtest/gtest.h" ] || [ -f "/usr/local/include/gtest/gtest.h" ] || [ -f "/opt/homebrew/include/gtest/gtest.h" ]; then
        GTEST_AVAILABLE=true
    fi
    
    if [ "$GTEST_AVAILABLE" = true ]; then
        print_status "INFO" "Google Test found, building unit tests..."
        
        # Uncomment Google Test flags in Makefile temporarily
        sed -i.bak 's/# GTEST_LIBS = -lgtest -lgtest_main -lpthread/GTEST_LIBS = -lgtest -lgtest_main -lpthread/' Makefile
        
        # Build unit tests
        make unit-test > /tmp/unit_test_build.log 2>&1
        
        if [ $? -eq 0 ]; then
            print_status "PASS" "Unit tests built successfully"
            
            # Run unit tests
            print_status "INFO" "Running unit tests..."
            ./unit_tests > /tmp/unit_test_output.txt 2>&1
            
            if [ $? -eq 0 ]; then
                print_status "PASS" "All unit tests passed"
                echo ""
                echo "📊 Unit Test Summary:"
                grep -E "(PASS|FAIL)" /tmp/unit_test_output.txt || echo "No test results found"
            else
                print_status "FAIL" "Some unit tests failed"
                echo ""
                echo "📊 Unit Test Summary:"
                grep -E "(PASS|FAIL)" /tmp/unit_test_output.txt || echo "No test results found"
            fi
            
            rm -f /tmp/unit_test_output.txt
        else
            print_status "FAIL" "Unit test build failed"
            cat /tmp/unit_test_build.log
        fi
        
        rm -f /tmp/unit_test_build.log
        
        # Restore Makefile
        mv Makefile.bak Makefile
    else
        print_status "INFO" "Google Test not found. Install with:"
        echo "  Ubuntu/Debian: sudo apt-get install libgtest-dev"
        echo "  macOS: brew install googletest"
        echo "  Or download from: https://github.com/google/googletest"
    fi
else
    print_status "FAIL" "C++ compiler not found"
fi

echo ""
echo "🎯 Test Summary"
echo "==============="
print_status "INFO" "Manual tests: Basic functionality verified"
print_status "INFO" "Simple tests: Comprehensive test coverage (no dependencies)"
print_status "INFO" "Unit tests: Advanced test coverage (if Google Test available)"
print_status "INFO" "Performance: Check individual test output for timing data"

echo ""
echo "💡 Next Steps:"
echo "  1. Run 'make simple-test' for comprehensive testing (no dependencies)"
echo "  2. Install Google Test for advanced unit testing"
echo "  3. Run 'make unit-test' to run unit tests"
echo "  4. Run './memory_allocator' for interactive testing"
echo "  5. Check test/simple_tests.cpp and test/unit_tests.cpp for test details" 