#!/bin/bash

# Define the filename
filename="file1.bpkg"

# Check if the .bpkg file exists
if [ ! -f "$filename" ]; then
    echo "Error: File $filename does not exist."
    exit 1
fi

# Variable to detect if we are in the chunks section
in_chunks=false
previous_line_empty=true

# Read the file line by line
while IFS= read -r line
do
    # Check if we are entering or exiting the chunks section
    if [[ "$line" == "chunks:" ]]; then
        in_chunks=true
        continue
    elif [[ "$line" == *":" && "$line" != "chunks:" ]]; then
        in_chunks=false
    fi

    # Perform checks if we are in the chunks section
    if $in_chunks; then
        # Check for empty lines or whitespace
        if [[ -z "$line" ]]; then
            if ! $previous_line_empty; then
                echo "Whitespace detected"
            fi
            previous_line_empty=true
            continue
        else
            previous_line_empty=false
        fi

        # Validate the chunk line format
        if [[ ! "$line" =~ ^[0-9a-f]{64},[0-9]+,[0-9]+$ ]]; then
            echo "Incorrect chunk format detected"
            exit 2
        fi
    fi
done < "$filename"

echo "Test completed successfully."
exit 0