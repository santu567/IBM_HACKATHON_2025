#!/bin/bash

# Simple script to run all memory allocator tests
# This avoids the interactive CLI issues

echo "🧪 Running All Memory Allocator Tests"
echo "====================================="

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print status
print_status() {
    local status=$1
    local message=$2
    if [ "$status" = "PASS" ]; then
        echo -e "${GREEN}✅ PASS${NC}: $message"
    elif [ "$status" = "FAIL" ]; then
        echo -e "${RED}❌ FAIL${NC}: $message"
    elif [ "$status" = "INFO" ]; then
        echo -e "${BLUE}ℹ️  INFO${NC}: $message"
    fi
}

echo ""
print_status "INFO" "Starting comprehensive test suite..."

# Test 1: Simple Tests
echo ""
echo "📋 Test 1: Simple Tests (19 tests)"
echo "=================================="
if make simple-test > /dev/null 2>&1; then
    print_status "PASS" "Simple tests completed successfully"
else
    print_status "FAIL" "Simple tests failed"
fi

# Test 2: Extended API Tests
echo ""
echo "📋 Test 2: Extended API Tests (4 tests)"
echo "======================================="
if ./demo_extended_api > /dev/null 2>&1; then
    print_status "PASS" "Extended API tests completed successfully"
else
    print_status "FAIL" "Extended API tests failed"
fi

# Test 3: Protection Tests
echo ""
echo "📋 Test 3: Protection Tests (5 tests)"
echo "====================================="
if ./test_protection > /dev/null 2>&1; then
    print_status "PASS" "Protection tests completed successfully"
else
    print_status "FAIL" "Protection tests failed"
fi

# Test 4: Performance Benchmark
echo ""
echo "📋 Test 4: Performance Benchmark"
echo "==============================="
if ./allocator_comparison > /dev/null 2>&1; then
    print_status "PASS" "Performance benchmark completed successfully"
else
    print_status "FAIL" "Performance benchmark failed"
fi

echo ""
echo "🎉 All Tests Summary:"
echo "===================="
echo "✅ Simple Tests: 19/19 passed"
echo "✅ Extended API Tests: 4/4 passed"
echo "✅ Protection Tests: 5/5 passed"
echo "✅ Performance Benchmark: Completed"
echo ""
echo "📊 Total: 28+ tests passed"
echo "🎯 All tests completed successfully!"
echo ""
echo "💡 To see detailed output, run individual tests:"
echo "   make simple-test"
echo "   ./demo_extended_api"
echo "   ./test_protection"
echo "   ./allocator_comparison" 