#ifndef CACHE_H
#define CACHE_H

#include "disk.h"
#include "uthash.h"

typedef struct {
    int key;      // inum
    void* val;
    UT_hash_handle hh;
} LRUNodeCache;

typedef struct {
    int key;    // block num
    void* val;
    UT_hash_handle hh;
} LRUBlockCache;

// geting a block or node
// return pointer to the obj if found, otherwise NULL
struct inode* lRUGetNode(int key);
struct block* lRUGetBlk(int key);

void init_node_lru(int nodeSize, int blkSize);


struct inode* findInode(int inum);

Block* read_block(int bNum);

void writeInode(int inum, struct inode* inode);

void setInodeFree(int inum);

#endif