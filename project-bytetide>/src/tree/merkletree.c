#include "../../include/tree/merkletree.h"
#include "../../include/crypt/sha256.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "../../include/chk/pkgchk.h"

//Your code here!
/**
 * Converts SHA256 hash to hexadecmial string
 * @param hash, binary hash value
 * @param hex_buffer, buffer that stores the hexadecmial string
 */

void hash_to_hex(const uint8_t* hash, char* hex_buffer){
    
    assert(hex_buffer != NULL);
    //hex digits
    static const char hex_digits[] = "0123456789abcdef" ;
    //converts each byte of the hash -> 2 hexadecimal characters
    for(int i = 0; i < 32; i++){
        hex_buffer[i * 2] = hex_digits[(hash[i] >> 4) &0xF];
        hex_buffer[i * 2 + 1] = hex_digits[hash[i] & 0xF];

    }
    hex_buffer[SHA256_HEXLEN] = '\0'; //null terminator for the string
}

/**
 * Computes the hash for a merkle tree node
 * @param node, the merkle tree node to compute the hash for
 */

void compute_merkle_node_hash(struct merkle_tree_node* node ){

    if(node->is_leaf){ //hashing data that is stored in the 'value' attribute
        
        uint8_t hash_output[32]; //matching the 32 byte output of SHA256
        sha256(node->value, strlen((char*)node->value), hash_output);
        hash_to_hex(hash_output, node->computed_hash);
        

    } else { //combining and hashing the child nodes hashes
        if(node->left && node->right){
            
            char combined_hashes[2 * SHA256_HEXLEN + 1] = {0};
            snprintf(combined_hashes, sizeof(combined_hashes), "%s%s", //combinin hashes
                node->left->computed_hash, node->right->computed_hash);
            //computing the combined hashes
            uint8_t hash_output[32];
            sha256(combined_hashes, strlen(combined_hashes), hash_output);
            hash_to_hex(hash_output, node->computed_hash);
            
        }
    }

}
/**
 * Creates merkletree nodes from the data blocks
 * @param data_blocks, the data blocks to create the nodes from
 * @param start, the start index of the data blocks
 * @param end, the end index of the data blocks
 * @return node, the root of the created merkle tree nodes
 */

struct merkle_tree_node* create_nodes(const char** data_blocks, int start, int end, struct bpkg_query qry, int index){

    if(start > end){
        return NULL;
    }
    struct merkle_tree_node* node = malloc(sizeof(struct merkle_tree_node));

    if(!node){
        perror("memory allocation failed for merkle tree node");
        return NULL;
    }
    memset(node, 0, sizeof(struct merkle_tree_node));

    if(2*index+1>=qry.len){ //leaf node
        node->is_leaf = 1;
        node->value = strdup(data_blocks[start]);//copy the data blocks

        strncpy(node->expected_hash, qry.hashes[index], 64);
        node->expected_hash[strlen(qry.hashes[index])] = '\0';
        if(!node->value){
            perror("Memory allocation failed for node value\n");
            free(node);
            return NULL;
        }

    } else { //internal nodes
        node->is_leaf = 0;
        int middle = (start+end) / 2;
        strncpy(node->expected_hash, qry.hashes[index],64);
        node->expected_hash[strlen(qry.hashes[index])] = '\0';
        node->left = create_nodes(data_blocks, start, middle, qry, 2*index+1); //create left subtree
        node->right = create_nodes(data_blocks, middle + 1, end, qry, 2*index+2);//creatr right subtree
        if(!node->left || !node->right){

            destroy_merkle_tree(node->left);
            destroy_merkle_tree(node->right);
            free(node);
            return NULL;
        }

    }
    compute_merkle_node_hash(node); //compute hash for the node
    return node;

}
/**
 * Destroys the merkle tree and frees the memory
 * @param node, the root of ther merkle tree to destroy
 *
 */

void destroy_merkle_tree(struct merkle_tree_node* node){
    if(node){
        free(node->value);
        destroy_merkle_tree(node->right);
        destroy_merkle_tree(node->left);
        free(node);
    }
}
/**
 * Destroys the nodes of the merkletree
 * @param node, the nodes in the merkletree to destroy
 *
 */
void destroy_tree(struct merkle_tree* node) {
    if(node){
        destroy_merkle_tree(node->root);
        free(node);
    }
}
/**
 * Create a merkle tree from data blocks
 * @param data_blocks, data blocks to create the merkle tree from
 * @param n_blocks, the number of data blocks
 * @param qry, reference to the storage of the hashes
 * @return merkle_tree, the created merkle tree
 */

struct merkle_tree* create_merkle_tree(const char** data_blocks, size_t n_blocks, struct bpkg_query qry){

    struct merkle_tree* merkle_tree = malloc(sizeof(struct merkle_tree));

    if(!merkle_tree){
        perror("failed to allocate memory for merkle tree");
        return NULL;
        
    }
    merkle_tree->root = create_nodes(data_blocks, 0, n_blocks -1, qry, 0);
    if(!merkle_tree->root){
        free(merkle_tree);
        return NULL;
    }

    return merkle_tree;
    

}

/**
 * Finds a node by its hash in the merkletree by using a dfs function
 * @param node, nodes in the merkletree to search
 * @param hash, the hash value to search for
 * @return found_node, the node with the matching hash, or null if not found
 */

struct merkle_tree_node* dfs_find_node_by_hash(struct merkle_tree_node* node, const char* hash){

    if(node == NULL){
        return NULL;
    }

    if(strncmp(node->expected_hash, hash, 64) == 0){
        return node;
    }
    struct merkle_tree_node* found_node = dfs_find_node_by_hash(node->left, hash);

    if(found_node!= NULL){
        return found_node;

    }
    found_node = dfs_find_node_by_hash(node->right, hash);
    if(found_node != NULL){
        return found_node;
    }
    return found_node;

}

/**
 * Collects the minimum set of hashes needed to represent the current completion state
 * @param node, current node being looked at in the merkletree
 * @param qry, the qry to store the collected hashes
 */

void collect_min_hashes(struct merkle_tree_node* node, struct bpkg_query* qry){
    if(node == NULL){
        return;
    }
    if(node->is_leaf || !node->is_complete) {
        char** temp = realloc(qry->hashes, (qry->len + 1) * sizeof(char*));
        if(!temp){
            perror("Failed to allocate memory for min hashes");
            qry->len = 0;
            return;
        }
        qry->hashes = temp;

        qry->hashes[qry->len] = strdup(node->computed_hash);
        if(qry->hashes[qry->len] == NULL){
            perror("failed to copy min hash");
            return;
        }
        
        qry->len++;
        return;

    } 
    if(node->left){
        collect_min_hashes(node->left, qry);
        
    }
    if(node->right){
        collect_min_hashes(node->right, qry);
    }
}


/**
 * recursion to compute the entire tree worth of hashes
 * @param node, the root of the merkletree
 */
void compute_merkle_tree_hashes(struct merkle_tree_node* node){

    if(node == NULL) return;
    if(node->left) compute_merkle_tree_hashes(node->left);
    if(node->right) compute_merkle_tree_hashes(node->right);
    compute_merkle_node_hash(node);

}

/**
 * updates the completion status of the merkletree node
 * @param node, the merkletree node to update
 */

void update_node_completeness(struct merkle_tree_node* node){

    if(node->is_leaf){
        node->is_complete = (strcmp(node->expected_hash, node->computed_hash) == 0);
    } else {
        if(node->left && node->right){
            update_node_completeness(node->left);
            update_node_completeness(node->right);
            node->is_complete = node->left->is_complete && node->right->is_complete;
        }
        else{
            node->is_complete = 0;
        }
    }
}

