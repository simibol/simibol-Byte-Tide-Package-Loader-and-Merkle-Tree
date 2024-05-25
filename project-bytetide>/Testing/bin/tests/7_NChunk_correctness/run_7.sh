#!/bin/bash

filename="example.bpkg"
nchunks_line=$(grep -m 1 "nchunks:" $filename)
nhashes_line=$(grep -m 1 "nhashes:" $filename)

nchunks=${nchunks_line#*:}
nhashes=${nhashes_line#*:}

actual_chunks=$(grep -c "^[0-9a-f]{64},[0-9]*,[0-9]*$" $filename)
actual_hashes=$(grep -c "^[0-9a-f]{64}$" $filename)

if [ "$actual_chunks" -ne "$nchunks" ]; then
    echo "unable to construct file"
    exit 1
fi

if [ "$actual_hashes" -ne "$nhashes" ]; then
    echo "unable to construct file"
    exit 1
fi

echo "file opened"
exit 0