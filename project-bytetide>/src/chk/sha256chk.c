#include "../include/chk/sha256chk.h"
#include "../include/crypt/sha256.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void computed_chunk_hash(FILE *file, size_t offset, size_t size, char output_hash[SHA256_HEXLEN + 1]){

    char* buffer = (char *)malloc(size);

    if(!buffer){
        perror("failed to allocate buffer");
        return;
    }
    fseek(file, offset, SEEK_SET);
    fread(buffer, 1, size, file);

    struct sha256_compute_data chunk_data = {0};

    uint8_t hashout[SHA256_INT_SZ];

    sha256_compute_data_init(&chunk_data);
    sha256_update(&chunk_data, buffer, size);
    sha256_finalize(&chunk_data, hashout);
    sha256_output_hex(&chunk_data, output_hash);
    output_hash[SHA256_HEXLEN] = '\0';

    free(buffer);
}