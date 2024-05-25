#!/bin/bash

# Define the filename
filename="first_test.bpkg"

# Check if the .bpkg file exists in the current directory
if [ -f "$filename" ]; then
    # Assuming a program executable 'pkgchk' that checks and potentially creates the file
    output=$(./pkgmain $filename)  # Replace 'pkgchk' with your actual program's name

    # Check the output of the command
    if [ "$output" == "File Created." ]; then
        echo "Test Passed: $filename identified correctly and file was created."
    else
        echo "Test Failed: File exists but the output was not as expected."
    fi
else
    # File does not exist, simulate the scenario where the file is created
    touch $filename
    echo "Simulating file creation for testing..."
    output=$(./pkgmain $filename)  # Ensure this command tries to create the file or handle it

    if [ "$output" == "File Created." ]; then
        echo "Test Passed: $filename was not present and was created successfully."
    else
        echo "Test Failed: $filename was not created as expected."
    fi
fi