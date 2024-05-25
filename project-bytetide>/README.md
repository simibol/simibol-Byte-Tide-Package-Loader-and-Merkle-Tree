Here is my project-bytetide readme file, this contains the information about the organisation of my software.

src/chk/pkgchk.c is the file that handles all the loading of the .bpkg package file from the given path. It contains a file check, that checks the filename and sees if the .bpkg file actually exists, the get all hashes function which retrieves the hashes from both the chunk and hash hashes inside the .bpkg files. This file also acts for the retrieval of all the completed chunks, getting the minimum completed chunks, getting all the hashes from the hashing function (hashing functionality located at src/chk/sha256chk.c). This file also has functions to deallocate the queries and the memory for efficient memory use.

the src/tree/merkletree.c file includes all the merkletree functionalities.

the src/crypt/sha256chk.c file computes the hashes that the .data file gives to it, helping compute the hash of the files chunks

Testing is located in my Testing file, and that can be accessed when opening the folder.

all the header files are located in the /include folder, which contains all the header files to the files described above

A further detailed functionality of each of the functions is detailed in a comment above the start of each function, detailing if I've used a
recursive function for things such like my merkle tree and for my other functions. Functionalities 