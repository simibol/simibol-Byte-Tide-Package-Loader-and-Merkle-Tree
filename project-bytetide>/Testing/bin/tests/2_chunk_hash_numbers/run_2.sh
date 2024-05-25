#!/bin/bash

# Define the filename
filename="file1.bpkg"

# Check if the .bpkg file exists
if [ ! -f "$filename" ]; then
    echo "Error: File $filename does not exist."
    exit 1
fi

# Initialize counters
hash_count=0
chunk_count=0
max_hashes=255
max_chunks=256

# Flags to know where we are in the file
in_hashes=false
in_chunks=false

# Read the file
while IFS= read -r line
do
    # Detect sections
    if [[ "$line" == "hashes:" ]]; then
        in_hashes=true
        in_chunks=false
        continue
    elif [[ "$line" == "chunks:" ]]; then
        in_hashes=false
        in_chunks=true
        continue
    elif [[ "$line" == *":" && "$line" != "hashes:" && "$line" != "chunks:" ]]; then
        # Assumes any new section heading ends the count
        in_hashes=false
        in_chunks=false
    fi

    # Count within sections
    if $in_hashes && [[ "$line" != "hashes:" ]]; then
        ((hash_count++))
    elif $in_chunks && [[ "$line" != "chunks:" ]]; then
        ((chunk_count++))
    fi

done < "$filename"

# Output results and check for errors
if [ "$hash_count" -gt "$max_hashes" ] || [ "$chunk_count" -gt "$max_chunks" ]; then
    echo "Too many hashes or chunks"
    exit 2
else
    echo "Hashes and Chunks count within limit: $hash_count hashes, $chunk_count chunks"
    exit 0
fi