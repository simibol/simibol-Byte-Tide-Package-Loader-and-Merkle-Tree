#!/bin/bash

# Empty Tree Test
output=$(./pkgmain empty_tree.bpkg)
expected_output="Not a valid merkle tree: no hash values provided"

if [[ "$output" == "$expected_output" ]]; then
    echo "Test 5 Passed: Empty tree test is successful."
else
    echo "Test 5 Failed: Incorrect output - $output"
fi