#ifndef MERKLE_TREE_H
#define MERKLE_TREE_H

#include <stddef.h>
#include <stdint.h>
#include "chk/pkgchk.h"
#include "../include/crypt/sha256.h"

#define SHA256_HEXLEN (64)

struct merkle_tree_node {
    void* key;
    void* value;
    struct merkle_tree_node* left;
    struct merkle_tree_node* right;
    int is_leaf;
    int is_complete;
    char expected_hash[SHA256_HEXLEN + 1];
    char computed_hash[SHA256_HEXLEN + 1];
};


struct merkle_tree {
    struct merkle_tree_node* root;
    size_t n_nodes;
};

struct bpkg_query; 

struct chunk;

struct merkle_tree_node* create_merkle_tree_node(const char* expected_hash);

struct merkle_tree* create_merkle_tree_from_hashes(const char** expected_hashes, size_t n_blocks);

void destroy_merkle_tree(struct merkle_tree_node* node);

void compute_merkle_node_hash(struct merkle_tree_node* node);

struct merkle_tree_node* dfs_find_node_by_hash(struct merkle_tree_node* node, const char* hash);

struct merkle_tree* create_merkle_tree(const char** data_blocks, size_t n_blocks, struct bpkg_query bpkg);

void compute_merkle_tree_hashes(struct merkle_tree_node* node);

void update_node_completeness(struct merkle_tree_node* node);

void compute_and_update_node(struct merkle_tree_node* root, const char* filename, struct chunk* chunks, size_t nchunks);

struct merkle_tree_node* create_nodes_from_hashes(const char** expected_hashes, int start, int end);

void hash_to_hex(const uint8_t* hash, char* hex_buffer);

void collect_min_hashes(struct merkle_tree_node* node, struct bpkg_query* qry);

void in_order_nodes_traversal(struct merkle_tree_node* node, struct merkle_tree_node** nodes, size_t* index);

void destroy_tree(struct merkle_tree* node);

#endif