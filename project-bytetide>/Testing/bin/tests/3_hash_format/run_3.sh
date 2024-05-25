#!/bin/bash

# Define the filename
filename="file1.bpkg"

# Check if the .bpkg file exists
if [ ! -f "$filename" ]; then
    echo "Error: File $filename does not exist."
    exit 1
fi

# Flag to track the hashes section
in_hashes=false

# Read the file
while IFS= read -r line
do
    # Detect the hashes section
    if [[ "$line" == "hashes:" ]]; then
        in_hashes=true
        continue
    elif [[ "$line" == *":" && "$line" != "hashes:" ]]; then
        in_hashes=false
    fi

    # Check hash format
    if $in_hashes && [[ "$line" =~ ^[0-9a-f]{64}$ ]]; then
        continue
    else
        if $in_hashes; then
            echo "Invalid hash"
            exit 2
        fi
    fi
done < "$filename"

echo "All hashes are valid."
exit 0