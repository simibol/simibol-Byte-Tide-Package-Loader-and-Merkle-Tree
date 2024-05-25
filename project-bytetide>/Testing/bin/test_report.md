** test cases for pkgmain implemented in the C programming language **

# Author
Simon Bolger - 520444627

# Date
Wednesday 15th of May 2024 8:03 AEST

# AI used (yes/no)
yes. ChatGPT 4

# AI Link
https://chat.openai.com

==========================================================================
####################### Tests ############################

# Test 1 Description - .bpkg file doesn't exist

To verify if the specified .bpkg file exists and the system correctly identifies it's existence, where in these cases the .bpkg file will exist in the same directory as too the data. If this is true, the program will return "File Created." 
The input is a valid .bpkg file that has no data file yet created for it.

# Test 2 Description - Verifying the chunk and hash amount

to verify if the specified .bpkg file includes the correct amount of chunks and hashes in the bpkg file. Noting that there is a maximum of 255 hashes and 256 chunks. 
The method to verifying this is to parse the bpkg file and count the chunks and hashes, and the count should not exceed 255 hashes or 256 chunks. If there are too many hashes or chunks, the program should output "Too many hashes or chunks"

# Test 3 Description - Testing with invalid format of hashes

verifying whether each .bpkg file contains hashes with correct formatting. The hashes need to include 64 hexadecimal characters to be valid, containing numbers from 0->9 and letters from a-f, if there's any other characters this is an invalid hash.
The method to verifying this is to parse the bpkg file and examine the hashes, with the count having to equal exactly 64, with the valid letters. If this is not the case, the program should output, "Invalid hash"

# Test 4 Description - Single element functionality of a merkle tree

Testing if the merkle tree can function with only a single data block. This means the element should be the root. If it isn't the root, we will return "not a valid merkle tree: single node is not the root"

# Test 5 Description - Empty Merkle Tree Initialisation

Ensure that the merkle tree, even if it doesn't have any values, still gets parsed correctly. This is done by initialising the merkle tree and parsing an empty .bpkg file. The system should reject the file, and output a "Not a valid merkle tree: no hash values provided" as there is no use in us having an empty merkle tree.

# Test 6 Description - Odd Chunk Test

Ensure that the merkle tree is still parsed and is valid even if an odd number of chunks were added to the merkle tree. This is done by creating a a .bpkg with an odd number of chunks. This is successfully passed if a placeholder node is added to ensure the tree is balanced

# Test 7 Description - NChunk Correctness

Ensuring that the "nchunks" value specifies exactly the amount of chunks that are present in the .bpkg file. If this isn't the case, the program should return "unable to construct file", else the the file should open normally "file opened"

# Test 8 Description - NHash Correctness

Ensuring that the "nhashes" value specifies exactly the amount of nhashes that are present in the .bpkg file. If this isn't the case, the program should return "unable to construct file", else the the file should open normally "file opened"

# Test 9 Description - Corrupted header

A corrupt header would be a file that has multiple incorrect implementations of the .bpkg file, and our program needs to ensure it outputs the correct error for each line of the header, specifying exactly what line it errors at.

# Test 10 Description - Deliberately Missing Chunks

Ensuring that when the program is presented a file with deliberately missing chunks or whitespace gaps between chunks, the program will skip over the whitespace, printing "whitespace detected" and not process the empty gap as a chunk, allowing the program to flow properly. 

# AI prompt 1:

first_test.bpkg (see 1_file_exists) this is the format of the .bpkg file, now create a test case for it, referring to the assignment description provided (A3 Assignment Specification) (file linked). Then write test case code for all the test case descriptions provided (test descriptions are pasted into it)

# AI generated 1:

see run_1.sh

see run_2.sh

see run_3.sh

see run_4.sh

see run_5.sh

see run_6.sh

see run_7.sh

see run_8.sh

see run_9.sh

see run_10.sh

# AI prompt 2:

can you write the example .bpkg files for single_element_test.bpkg, empty_tree.bpkg and odd_chunks.bpkg?

# AI generated 2:

see corresponding files in 4, 5, and 6 tests

Each .bpkg file is tailored to trigger specific behaviors in your Merkle tree handling code:

 - single_element_test.bpkg tests the scenario where a single chunk and hash must form the entire tree.
 - empty_tree.bpkg checks how the system handles an empty input, which should not be a valid case.
 - odd_chunks.bpkg tests the Merkle tree's handling of an odd number of chunks, where it should possibly add a    placeholder to maintain balance.

# AI prompt 3:

how do you run all these tests?

# AI generated 3:

See "instructions to run testing script" below


# Instructions to run testing script

Compile all testing code beforehand

see run_tests.sh

chmod +x run_tests.sh

./run_tests.sh


