#ifndef PKGCHK_H
#define PKGCHK_H

#include <stddef.h>
#include <stdint.h>
#include "../../include/tree/merkletree.h"

#define SHA256_HEXLEN 64
/**
 * Query object, allows you to assign
 * hash strings to it.
 * Typically: malloc N number of strings for hashes
 *    after malloc the space for each string
 *    Make sure you deallocate in the destroy function
 */
struct bpkg_query{
	char** hashes;
	size_t len;
};

struct chunk {
		char hash[SHA256_HEXLEN + 1]; //hash of a chunk
		uint32_t offset; //file's internal offset
		uint32_t size; //size of the chunk
		
	}; //chunks dynamic array

//TODO: Provide a definition
struct bpkg_obj{

	char ident[1025]; //identifier string
	char filename[257]; //filename string
	uint32_t size; //size of file
	uint32_t nhashes; //number of hashes that are pre-computed
	char** hashes; //Array of strings for non-leaf hashes
	uint32_t nchunks; //Number of data chunks
	struct chunk *chunks;
	struct merkle_tree* merkle_root;


};

struct merkle_tree_node;
/**
 * Loads the package for when a value path is given
 */
struct bpkg_obj* bpkg_load(const char* path);

/**
 * Checks to see if the referenced filename in the bpkg file
 * exists or not.
 * @param bpkg, constructed bpkg object
 * @return query_result, a single string should be
 *      printable in hashes with len sized to 1.
 * 		If the file exists, hashes[0] should contain "File Exists"
 *		If the file does not exist, hashes[0] should contain "File Created"
 */
struct bpkg_query bpkg_file_check(struct bpkg_obj* bpkg);

/**
 * Retrieves a list of all hashes within the package/tree
 * @param bpkg, constructed bpkg object
 * @return query_result, This structure will contain a list of hashes
 * 		and the number of hashes that have been retrieved
 */
struct bpkg_query bpkg_get_all_hashes(struct bpkg_obj* bpkg);

/**
 * Retrieves all completed chunks of a package object
 * @param bpkg, constructed bpkg object
 * @return query_result, This structure will contain a list of hashes
 * 		and the number of hashes that have been retrieved
 */
struct bpkg_query bpkg_get_completed_chunks(struct bpkg_obj* bpkg);


/**
 * Gets the mininum of hashes to represented the current completion state
 * Example: If chunks representing start to mid have been completed but
 * 	mid to end have not been, then we will have (N_CHUNKS/2) + 1 hashes
 * 	outputted
 *
 * @param bpkg, constructed bpkg object
 * @return query_result, This structure will contain a list of hashes
 * 		and the number of hashes that have been retrieved
 */
struct bpkg_query bpkg_get_min_completed_hashes(struct bpkg_obj* bpkg); 


/**
 * Retrieves all chunk hashes given a certain an ancestor hash (or itself)
 * Example: If the root hash was given, all chunk hashes will be outputted
 * 	If the root's left child hash was given, all chunks corresponding to
 * 	the first half of the file will be outputted
 * 	If the root's right child hash was given, all chunks corresponding to
 * 	the second half of the file will be outputted
 * @param bpkg, constructed bpkg object
 * @return query_result, This structure will contain a list of hashes
 * 		and the number of hashes that have been retrieved
 */
struct bpkg_query bpkg_get_all_chunk_hashes_from_hash(struct bpkg_obj* bpkg, char* hash);


/**
 * Deallocates the query result after it has been constructed from
 * the relevant queries above.
 */
void bpkg_query_destroy(struct bpkg_query* qry);

/**
 * Deallocates memory at the end of the program,
 * make sure it has been completely deallocated
 */
void bpkg_obj_destroy(struct bpkg_obj* obj);

void get_child_chunks(struct merkle_tree* tree, struct merkle_tree_node* node, struct bpkg_query* qry);

void bpkg_query_destroy2(struct bpkg_query* qry);


#endif