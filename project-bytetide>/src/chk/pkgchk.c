#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "chk/pkgchk.h"
#include "../include/tree/merkletree.h"
#include "crypt/sha256.h"
#include "../include/chk/sha256chk.h"


#define HEAD_BUF_LEN 10

// PART 1


/**
 * Loads the package for when a value path is given
 */
struct bpkg_obj* bpkg_load(const char* path) {
    


    //opening bpkg file

    FILE* file = fopen(path, "rb");

    //checking if the file exists

    if (!file){
        perror("Failed to open file");
        return NULL;
    }

    //allocating memory for the each of the contents of the .bpkg file
    struct bpkg_obj* obj = malloc(sizeof(struct bpkg_obj) );

    //checking if the memory allocation for the contents is successful

    if(!obj){
        fclose(file);
        perror("Failed to allocate bpkg_obj");
        return NULL;
    }

    
    memset(obj, 0, sizeof(struct bpkg_obj));

    //reading 'ident' from the file

    if(fscanf(file, "ident:%1024s\n", obj->ident) != 1){
        fprintf(stderr, "Failed to read 'ident' from file.\n");
        fclose(file);
        bpkg_obj_destroy(obj);
        return NULL;
    }
    //reading 'filename' from the file

    if(fscanf(file, "filename:%256s\n", obj->filename) != 1){
        fprintf(stderr, "Failed to read 'filename' from file.\n");

        fclose(file);
        bpkg_obj_destroy(obj);
        return NULL;
    }
    //reading 'size' from the file
    if(fscanf(file, "size:%u\n", &obj->size) != 1){
        fprintf(stderr, "Failed to read 'size' from file.\n");
        fclose(file);
        bpkg_obj_destroy(obj);
        return NULL;
    }
    
    //reading 'nhashes' from the file
    if(fscanf(file, "nhashes:%u\n", &obj->nhashes) != 1){
        fprintf(stderr, "Failed to read 'nhashes' from file.\n");
        fclose(file);
        bpkg_obj_destroy(obj);
        return NULL;
    }

    char hashes_head[HEAD_BUF_LEN]; //buffer to read section header

    //seeing if it actually found the correct hashes
    if(fgets(hashes_head, sizeof(hashes_head), file) == NULL || strncmp(hashes_head, "hashes:\n", 8) != 0){
        fprintf(stderr, "Expected 'hashes: but found '%s\n", hashes_head);
        fclose(file);
        bpkg_obj_destroy(obj);
        return NULL;
    }


    //allocating memory for hashes
    obj->hashes = malloc(obj->nhashes * sizeof(char*));
    //checking to see if the hashes exist
    if (obj->hashes == NULL){

        perror("Failed to allocate memory for hashes");
        
        fclose(file);
        free(obj->hashes); //SHOULD IT BE REMOVED?
        bpkg_obj_destroy(obj);
        return NULL;
    }
    //reading the hashes from the file
    for (uint32_t i = 0; i < obj->nhashes; i++){
        obj->hashes[i] = malloc(SHA256_HEXLEN + 1);

        if(!obj->hashes[i]){
            
            perror("Failed to allocate memory for a hash");
            for(uint32_t j = 0; j < i; j++){
                free(obj->hashes[j]);
            }
            free(obj->hashes);
            fclose(file);
            bpkg_obj_destroy(obj);
            return NULL;

        }
        

        if(fscanf(file, "%64s\n", obj->hashes[i]) != 1){
            perror("failed to read hashes");
            while(i >= 0){
                free(obj->hashes[i--]);
            }
            free(obj->hashes);
            fclose(file);
            bpkg_obj_destroy(obj);
            return NULL;
        }
    //reading 'nchunks' from the file    
    }
    if(fscanf(file, "nchunks:%u\n", &obj->nchunks) != 1){
        fprintf(stderr, "Failed to read 'nchunks' from file.\n");
        fclose(file);
        bpkg_obj_destroy(obj);
        return NULL;
    }
    char chunks_head[HEAD_BUF_LEN]; //buffer to read section header

    //seeing if it actually found the correct chunks
    if(fgets(chunks_head, sizeof(chunks_head), file) == NULL || strncmp(chunks_head, "chunks:\n", 8) != 0){
        fprintf(stderr, "Expected 'chunks:' but found '%s\n", chunks_head);
        fclose(file);
        bpkg_obj_destroy(obj);
        return NULL;
    }
    //allocating memory for the chunks
    obj->chunks = malloc(obj->nchunks * sizeof(struct chunk));
    //seeing if the chunks exist
    if(!obj->chunks){
        perror("Failed to allocate memory for chunks");
        free(obj->hashes);
        free(obj);
        fclose(file);
        bpkg_obj_destroy(obj);
        return NULL;
    }


    //reading the chunks from the file
    for (uint32_t i = 0; i < obj->nchunks; i++){
        if(fscanf(file, "%64s,%u,%u\n", obj->chunks[i].hash, &obj->chunks[i].offset, &obj->chunks[i].size) != 3){
            perror("failed to read chunk data");
            for(uint32_t j = 0; j <= i ; j++){
                free(obj->hashes[j]);
            }
            free(obj->chunks);
            free(obj->hashes);
            bpkg_obj_destroy(obj);
            fclose(file);
            return NULL;
        }
    }
    fclose(file);
    return obj;
}


/**
 * Checks to see if the referenced filename in the bpkg file
 * exists or not.
 * @param bpkg, constructed bpkg object
 * @return result, a single string should be
 *      printable in hashes with len sized to 1.
 * 		If the file exists, hashes[0] should contain "File Exists"
 *		If the file does not exist, hashes[0] should contain "File Created"
 */
struct bpkg_query bpkg_file_check(struct bpkg_obj* bpkg){ //todo later


    struct bpkg_query result = {0};
    FILE *file;
    

    result.hashes = malloc(sizeof(char*));
    result.len = 1;
    //seeing if memory is allocated properly
    if(!result.hashes){
        perror("Failed to allocate memory for result hashes");
        exit(EXIT_FAILURE);
    }
    file = fopen(bpkg->filename, "r");

    //checking if the file is found or not
    if (file){
        fclose(file);
        result.hashes[0] = strdup("File Exists");

    } else {
        result.hashes[0] = strdup("File Created");
    }

    return result;


}


/**
 * Retrieves a list of all hashes within the package/tree
 * @param bpkg, constructed bpkg object
 * @return qry, This structure will contain a list of hashes
 * 		and the number of hashes that have been retrieved
 */
struct bpkg_query bpkg_get_all_hashes(struct bpkg_obj* bpkg) {

    struct bpkg_query qry = {0};
    //getting all the hashes by getting the hash hashes and the chunk hashes
    size_t total_hashes = bpkg->nhashes + bpkg->nchunks;

    //allocating memory for the total hashes
    qry.hashes = malloc(total_hashes * sizeof(char*)); //memory for hash
    
    if(!qry.hashes){
        perror("Failed to allocate memory for all hashes");
        free(qry.hashes);
        qry.hashes = NULL;
        return qry;
    }
    for(size_t i = 0; i < bpkg->nhashes; i++){

        qry.hashes[i] = strdup(bpkg->hashes[i]); //hash copies
        qry.hashes[i][64] ='\0';

        if(!qry.hashes[i]){
            perror("Failed to copy hash");

            for(size_t j = 0; j < i; j++){
                free(qry.hashes[j]);
            }
            free(qry.hashes);
            qry.hashes = NULL;
            qry.len = 0;
            return qry;
        }
    }
    for(size_t i = 0; i < bpkg->nchunks; i++){

        qry.hashes[bpkg->nhashes + i] = strdup(bpkg->chunks[i].hash); //hash copies
        qry.hashes[bpkg->nhashes + i][64] ='\0';

        if(!qry.hashes[bpkg->nhashes + i]){
            perror("Failed to copy chunk hash");

            for(size_t j = 0; j < bpkg->nhashes; j++){
                free(qry.hashes[j]);
            }
            free(qry.hashes);
            qry.hashes = NULL;
            qry.len = 0;
            return qry;
        }
    }
    qry.len = total_hashes;
    return qry;
    
}



/**
 * Retrieves all completed chunks of a package object
 * @param bpkg, constructed bpkg object
 * @return qry, This structure will contain a list of hashes
 * 		and the number of hashes that have been retrieved
 */
struct bpkg_query bpkg_get_completed_chunks(struct bpkg_obj* bpkg) { 
    struct bpkg_query qry = { 0 };

    
    
    if(!bpkg || !bpkg->chunks || bpkg->nchunks == 0){
        fprintf(stderr, "Invalid bpkg or empty chunks\n");
        return qry;
    }
    FILE* data_file = fopen(bpkg->filename, "rb");
    if(!data_file){
        perror("Failed to open data file");
        return qry;
    }

    //allocate memory for the hashes in the query
    qry.hashes = malloc(bpkg->nchunks * sizeof(char *));
    if(!qry.hashes){
        perror("Failed to allocate memory for completed chunks");
        fclose(data_file);
        return qry;
    }


    size_t count = 0;
    //iterate through and duplicate the hashes

    for(size_t i = 0; i < bpkg->nchunks; i++){

        char computed_hash[SHA256_HEXLEN + 1];
        computed_chunk_hash(data_file, bpkg->chunks[i].offset, bpkg->chunks[i].size, computed_hash);

        if(strcmp(computed_hash, bpkg->chunks[i].hash) == 0){
            qry.hashes[count] = strdup(bpkg->chunks[i].hash);
            if(!qry.hashes[count]){
                perror("Failed to copy chunk data");
                for(size_t j = 0; j < count; j++){
                    free(qry.hashes[i]);
                }
                free(qry.hashes);
                qry.hashes = NULL;
                qry.len = 0;
                fclose(data_file);
                return qry;
            }
            count++;
        }

    }
    //setting the length of the query result for memory purposes
    qry.len = count;
    fclose(data_file);
    return qry;
}




/**
 * Gets the mininum of hashes to represented the current completion state
 * Example: If chunks representing start to mid have been completed but
 * 	mid to end have not been, then we will have (N_CHUNKS/2) + 1 hashes
 * 	outputted
 *
 * @param bpkg, constructed bpkg object
 * @return qry, This structure will contain a list of hashes
 * 		and the number of hashes that have been retrieved
 */
struct bpkg_query bpkg_get_min_completed_hashes(struct bpkg_obj* bpkg) {

    struct bpkg_query qry = { 0 };
    //checking if the bpkg object and the chunks are valid
    if(!bpkg || !bpkg->chunks || bpkg->nchunks == 0){
        fprintf(stderr, "Invalid bpkg or empty chunks\n");
        return qry;
    }
    //allocate memory for the data blocks to hold memory for the chunk hashes
    FILE* data_file = fopen(bpkg->filename, "rb");
    if(!data_file){
        perror("Failed to open data file");
        return qry;
    }
    char** computed_hashes = malloc(bpkg->nchunks * sizeof(char *));
    if(!computed_hashes){
        perror("Failed to allocate memory for computed hashes");
        fclose(data_file);
        return qry;
    }
    for(size_t i = 0; i < bpkg->nchunks; i++){
        computed_hashes[i] = malloc((SHA256_HEXLEN + 1) * sizeof(char));
        if(!computed_hashes[i]){
            perror("failed to allocate memory for computed hashes");
            for(size_t j = 0; j < i; i++){
                free(computed_hashes[j]);
            }
            free(computed_hashes);
            fclose(data_file);
            return qry;
        }
        computed_chunk_hash(data_file, bpkg->chunks[i].offset, bpkg->chunks[i].size, computed_hashes[i]);
    }
    fclose(data_file);
    // using the hashes from my bpkg_get_all_hashes function to compute the merkletree
    struct bpkg_query temp = bpkg_get_all_hashes(bpkg);
    struct merkle_tree* tree = create_merkle_tree((const char**)computed_hashes, bpkg->nchunks, temp);
    bpkg_query_destroy(&temp);

    if(tree == NULL){
        perror("Failed to create merkle tree from computed hashes");
        for(size_t i = 0; i < bpkg->nchunks; i++){
            free(computed_hashes[i] );
        }
        free(computed_hashes);
        bpkg_query_destroy(&temp);
        return qry;
    }

    compute_merkle_tree_hashes(tree->root);
    update_node_completeness(tree->root);
    collect_min_hashes(tree->root, &qry);
    destroy_merkle_tree(tree->root);
    free(tree);

    for(size_t i = 0; i < bpkg->nchunks; i++){
        free(computed_hashes[i]);
    }
    free(computed_hashes);
    return qry;

}



/**
 * Retrieves all chunk hashes given a certain an ancestor hash (or itself)
 * Example: If the root hash was given, all chunk hashes will be outputted
 * 	If the root's left child hash was given, all chunks corresponding to
 * 	the first half of the file will be outputted
 * 	If the root's right child hash was given, all chunks corresponding to
 * 	the second half of the file will be outputted
 * @param bpkg, constructed bpkg object
 * @return qry, This structure will contain a list of hashes
 * 		and the number of hashes that have been retrieved
 */
struct bpkg_query bpkg_get_all_chunk_hashes_from_hash(struct bpkg_obj* bpkg, char* hash) {

    struct bpkg_query* qry = malloc(sizeof(struct bpkg_query));
    qry -> len = 0;
    qry -> hashes =  malloc((bpkg->nchunks)*sizeof(char*));

    //checking if bpkg, its chunks or the hashes are invalid
    if(bpkg == NULL || bpkg->chunks == NULL || bpkg->nchunks == 0 || hash == NULL){
        fprintf(stderr, "invalid input parameters");
        return *qry;
    }
    //opening the data file
    FILE* data_file = fopen(bpkg->filename, "rb");
    if(!data_file){
        perror("Failed to open data file");
        return *qry;
    }
    //allocating memory for the computed hashes
    char** computed_hashes = malloc(bpkg->nchunks * sizeof(char*));
    if(!computed_hashes){
        perror("Failed to allocate memory for computed hashes");
        fclose(data_file);
        return *qry;
    }
    //allocating memoy for each computed hash
    for(size_t i = 0; i < bpkg->nchunks; i++){
        computed_hashes[i] = malloc((SHA256_HEXLEN + 1) * sizeof(char*));
        if(!computed_hashes[i]){
            perror("Failed to allocate memory for computed hash");
            for(size_t j = 0; j < i; j++){
                free(computed_hashes[j]);
            }
            free(computed_hashes);
            fclose(data_file);
            return *qry;
        }
    }
    //compute the hash for each chunk
    for(size_t i = 0; i < bpkg->nchunks; i++){
        computed_chunk_hash(data_file, bpkg->chunks[i].offset, bpkg->chunks[i].size, computed_hashes[i]);
    }
    fclose(data_file);
    //creating a merkle tree from the computed hashes
    struct bpkg_query all_hashes = bpkg_get_all_hashes(bpkg);
    struct merkle_tree* tree = create_merkle_tree((const char**)computed_hashes, bpkg->nchunks, all_hashes);
    bpkg_query_destroy(&all_hashes);

    if(tree == NULL){
        perror("Failed to create merkletree from computed hashes");
        for(size_t i = 0; i < bpkg->nchunks; i++){
            free(computed_hashes[i]);
        }
        free(computed_hashes);
     
        return *qry;
    }


    //finding the target node by hash
    struct merkle_tree_node* target_node = dfs_find_node_by_hash(tree->root, hash);
    


    if(target_node == NULL){
       
        destroy_merkle_tree(tree->root);
        free(tree);
        for(size_t i = 0; i < bpkg->nchunks; i++){
            free(computed_hashes[i]);
        }
        free(computed_hashes);
        return *qry;
    }
    get_child_chunks(tree, target_node, qry);
    destroy_tree(tree);
    for(size_t i = 0; i < bpkg->nchunks; i++){
        free(computed_hashes[i]);
    }
    free(computed_hashes);
    struct bpkg_query temp = *qry;
    free(qry);
    return temp;

}

/**
 * my recursive function to collect the child chunk hashes from the children of the target node that
 * has been selected by the hashes_of function
 * @param tree, pointer to the merkletree
 * @param node, pointer to merkletree's nodes
 * @param qry, poitner to the query struct to access the hashes
 */
void get_child_chunks(struct merkle_tree* tree, struct merkle_tree_node* node, struct bpkg_query* qry) {
    if (node -> is_leaf) {

        char* to_add = malloc(strlen(node -> expected_hash)+1);
        if(to_add){
            strncpy(to_add, node -> expected_hash, 64);
            qry->hashes[qry->len] = to_add;
            qry->len++;
        }

    } else {
        
        get_child_chunks(tree, node -> left, qry);
        get_child_chunks(tree, node -> right, qry);
    }
}
/**
 * Deallocates the query result after it has been constructed from
 * the relevant queries above.
 */
void bpkg_query_destroy(struct bpkg_query* qry) {
    if(qry != NULL){
        for (size_t i = 0; i < qry->len; i++){
            free(qry->hashes[i]);
        }
        free(qry->hashes);
    }
}
void bpkg_query_destroy2(struct bpkg_query* qry) {
    if(qry != NULL){
        for (size_t i = 0; i < qry->len; i++){
            free(qry->hashes[i]);
        }
        free(qry->hashes);
        free(qry);
    }
}

/**
 * Deallocates memory at the end of the program,
 * make sure it has been completely deallocated
 */
void bpkg_obj_destroy(struct bpkg_obj* obj) {
    //TODO: Deallocate here!
    if(obj != NULL){
        for(uint32_t i = 0; i < obj->nhashes; i++){
            free(obj->hashes[i]);

        }
        free(obj->hashes);
    }
    if(obj->chunks != NULL){

        free(obj->chunks);
        
    }
    if(obj->merkle_root){
        destroy_tree(obj->merkle_root);
        obj->merkle_root = NULL;
    }
    free(obj);

}