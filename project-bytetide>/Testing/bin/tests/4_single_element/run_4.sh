#!/bin/bash

# Single Element Test
output=$(./pkgmain single_element_test.bpkg)
expected_output="not a valid merkle tree: single node is not the root"

if [[ "$output" == "$expected_output" ]]; then
    echo "Test 4 Passed: Single element test is successful."
else
    echo "Test 4 Failed: Incorrect output - $output"
fi