#ifndef SHA256CHK_H
#define SHA256CHK_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#define SHA256_BFLEN 1024
#define SHA256_HEXLEN 64

void computed_chunk_hash(FILE *file, size_t offset, size_t size, char output_hash[SHA256_HEXLEN + 1]);

#endif 