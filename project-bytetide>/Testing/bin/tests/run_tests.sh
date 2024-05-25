#!/bin/bash

# Test 1: .bpkg file doesn't exist
echo "Test 1: .bpkg file doesn't exist"
if ./merkle_tree_tester non_existent.bpkg; then
    echo "Test 1 Failed: Unexpected success on non-existent file"
else
    echo "Test 1 Passed"
fi
echo ""

# Test 2: Verifying chunk and hash counts
echo "Test 2: Verifying chunk and hash counts"
./merkle_tree_tester check_counts.bpkg
echo ""

# Test 3: Testing with invalid format of hashes
echo "Test 3: Testing with invalid format of hashes"
./merkle_tree_tester invalid_hashes.bpkg
echo ""

# Test 4: Single element functionality of a merkle tree
echo "Test 4: Single element functionality"
./merkle_tree_tester single_element_test.bpkg
echo ""

# Test 5: Empty Merkle Tree Initialization
echo "Test 5: Empty Merkle Tree Initialization"
./merkle_tree_tester empty_tree.bpkg
echo ""

# Test 6: Odd Chunk Test
echo "Test 6: Odd Chunk Test"
./merkle_tree_tester odd_chunks.bpkg
echo ""

# Test 7: NChunk Correctness
echo "Test 7: NChunk Correctness"
./merkle_tree_tester nchunk_test.bpkg
echo ""

# Test 8: NHash Correctness
echo "Test 8: NHash Correctness"
./merkle_tree_tester nhash_test.bpkg
echo ""

# Test 9: Corrupted header
echo "Test 9: Corrupted header"
./merkle_tree_tester corrupted_header.bpkg
echo ""

# Test 10: Deliberately Missing Chunks
echo "Test 10: Deliberately Missing Chunks"
./merkle_tree_tester missing_chunks.bpkg
echo ""