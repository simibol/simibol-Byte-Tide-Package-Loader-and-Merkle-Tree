#!/bin/bash

# Odd Chunk Test
output=$(./pkgmain odd_chunks.bpkg)
expected_output="placeholder node added"

if [[ "$output" == *"$expected_output"* ]]; then
    echo "Test 6 Passed: Odd chunk test is successful."
else
    echo "Test 6 Failed: Incorrect output - $output"
fi