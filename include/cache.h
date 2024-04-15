#ifndef CACHE_H
#define CACHE_H

#include "disk.h"
#include "uthash.h"
#include <stdbool.h>
typedef struct {
    int key;      // inum
    struct inode* val;
    UT_hash_handle hh;
    bool dirty;
} LRUNodeCache;

typedef struct {
    int key;    // block num
    struct block* val;
    UT_hash_handle hh;
    bool dirty;
} LRUBlockCache;

// geting a block or node
// return pointer to the obj if found, otherwise NULL
struct inode* lRUGetNode(int key);

struct block* lRUGetBlk(int key);

// put inode pointer by inum to cache
// if not exist, add the inode to cache
void lRUNodePut(int key, struct inode* value);

void lRUBlockPut(int key, struct block* value);

void init_node_lru(int nodeSize, int blkSize);

struct inode* findInode(int inum);

Block* read_block(int bNum);

int write_block(int bNum, void* data);

int writeInode(int inum, struct inode* inode);

void lRUWriteNode(LRUNodeCache *cahce);

void lRUWriteBlk(LRUBlockCache *cahce);


#endif