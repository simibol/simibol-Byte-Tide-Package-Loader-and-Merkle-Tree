#!/bin/bash

# Define the filename
filename="file1.bpkg"

# Check if the .bpkg file exists
if [ ! -f "$filename" ]; then
    echo "Error: File $filename does not exist."
    exit 1
fi

# Define expected patterns
ident_pattern="^[0-9a-f]{128}$"
filename_pattern="^filename:[^ ]+$"
size_pattern="^size:[0-9]+$"
nhashes_pattern="^nhashes:[0-9]+$"
hashes_pattern="^hashes:$"
hash_pattern="^[0-9a-f]{64}$"

# Variable to detect if we are in the hashes section
in_hashes=false
line_number=0

# Read the file line by line
while IFS= read -r line
do
    line_number=$((line_number + 1))

    # Check header patterns
    if [[ "$line" =~ $ident_pattern ]]; then
        continue
    elif [[ "$line" =~ $filename_pattern ]]; then
        continue
    elif [[ "$line" =~ $size_pattern ]]; then
        continue
    elif [[ "$line" =~ $nhashes_pattern ]]; then
        continue
    elif [[ "$line" == "hashes:" ]]; then
        in_hashes=true
        continue
    elif $in_hashes && [[ "$line" =~ $hash_pattern ]]; then
        continue
    elif [[ "$line" == "" ]]; then  # Exit if empty line indicating end of the header
        break
    else
        echo "Corruption detected at line $line_number: $line"
        exit 2
    fi
done < "$filename"

echo "Header is valid."
exit 0