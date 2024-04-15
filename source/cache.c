#include "cache.h"
// #include "uthash.h"
#include <comp421/yalnix.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

LRUNodeCache *nd_head = NULL;
LRUBlockCache *blk_head = NULL;

// size of node cache
unsigned int n_size;
// size of block cache
unsigned int b_size;

/* write the inode back to disk */
int writeInode(int inum, struct inode* inode) {
    if (inum <= 0 || inum > INODE_NUM) {
        TracePrintf( 1, "[SERVER][ERR] writeInode: Invalid inum %d\n", inum);
        return ERROR;
    }

    int block_num = getInodeBlockNum(inum);
    int offset = (inum % INODE_PER_BLOCK) * INODESIZE;

    Block* blk = read_block(block_num);

    memcpy(blk->datum + offset, inode, INODESIZE);

    return WriteSector(block_num, (void *) blk->datum);
}

/* find the inode given the inum */
struct inode* findInode(int inum) {
    if (inum <= 0 || (INODE_NUM != 0 && inum > INODE_NUM)) {
        TracePrintf( 1, "[SERVER][ERR] findInode: Invalid inum %d\n", inum);
        return NULL;
    }
    
    return lRUGetNode(inum);
}

/* read a block, save it in a struct and return a pointer to the block */
Block* read_block(int bNum) {    
    Block* block = (Block*)malloc(sizeof(Block));

    int res = ReadSector(bNum, (void *) block->datum);
    if (res == ERROR) {
        TracePrintf( 1, "[SERVER][ERR] ReadSector failed\n");
        return NULL;
    }

    return block;
}

/* read a block, save it in a struct and return a pointer to the block */
int write_block(int bNum, void* data) {
    return WriteSector(bNum, data);
}

/* LRU Cache Interface */
void init_node_lru(int nodeSize, int blkSize) {
    n_size = nodeSize;
    b_size = blkSize;
}

struct inode* lRUGetNode(int key) {
    LRUNodeCache *cur = NULL;

    HASH_FIND_INT(nd_head, &key, cur);

    if (cur != NULL) {
        TracePrintf( 1, "[CACHE][LOG] Find indoe %d in cache\n", key);

        HASH_DEL(nd_head, cur);
    } else {
        TracePrintf( 1, "[CACHE][LOG] Can't find indoe %d in cache\n", key);

        if (HASH_COUNT(nd_head) == INODE_CACHESIZE) {
            TracePrintf( 1, "[CACHE][LOG] Cache is full, evict node\n");

            if (nd_head->dirty) lRUWriteNode(nd_head);

            HASH_DEL(nd_head, nd_head);
        }

        int block_num = getInodeBlockNum(key);
        int offset = (key % INODE_PER_BLOCK) * INODESIZE;

        Block* blk = read_block(block_num);
        if (blk == NULL) return NULL;

        struct inode* inode = (struct inode*)malloc(sizeof(struct inode));

        memcpy(inode, blk->datum + offset, INODESIZE);

        cur = (LRUNodeCache *)malloc(sizeof(LRUNodeCache));
        cur->val = inode;
        cur->key = key;

        TracePrintf( 1, "[CACHE][LOG] Add indoe %d in cache\n", key);
    }

    HASH_ADD_INT(nd_head, key, cur);

    return cur->val;
}

struct block* lRUGetBlk(int key) {
    LRUBlockCache *cur = NULL;

    HASH_FIND_INT(blk_head, &key, cur);
    if (cur != NULL) { // found, move it to the head
        TracePrintf( 1, "[CACHE][LOG] Find block %d in cache\n", key);

        HASH_DEL(blk_head, cur);
    } else {
        TracePrintf( 1, "[CACHE][LOG] Can't find block %d in cache\n", key);

        if (HASH_COUNT(blk_head) == BLOCK_CACHESIZE) {
            TracePrintf( 1, "[CACHE][LOG] Cache is full, evict node\n");

            if (blk_head->dirty) lRUWriteBlk(blk_head);

            HASH_DEL(blk_head, blk_head);
        }

        Block* block = (Block*)malloc(sizeof(Block));

        int res = ReadSector(key, (void *) block->datum);
        if (res == ERROR) {
            TracePrintf( 1, "[CACHE][ERR] ReadSector failed\n");
            return NULL;
        }

        cur = (LRUBlockCache *)malloc(sizeof(LRUBlockCache));
        cur->val = block;
        cur->key = key;

        TracePrintf( 1, "[CACHE][LOG] Add block %d in cache\n", key);
    }

    HASH_ADD_INT(blk_head, key, cur);

    return cur->val;
}

void lRUNodePut(int key, struct inode* value) {
    LRUNodeCache *cur = NULL;
    HASH_FIND_INT(nd_head, &key, cur);
    // find the inode
    if (cur != NULL) {
        HASH_DEL(nd_head, cur);

        cur->key = key;
        cur->val = value;
        cur->dirty = true;

        TracePrintf( 1, "[CACHE][LOG] Found the node, replace it with new value!\n");
    } else { // insert new
        TracePrintf( 1, "[CACHE][LOG] Can't find inode %d in cache\n", key);

        if (HASH_COUNT(nd_head) == INODE_CACHESIZE) {
            TracePrintf( 1, "[CACHE][LOG] Cache is full, evict node\n");

            if (nd_head->dirty) lRUWriteNode(nd_head);

            HASH_DEL(nd_head, nd_head);
        }

        cur = (LRUNodeCache *)malloc(sizeof(LRUNodeCache));
        cur->key = key;
        cur->val = value;
        cur->dirty = true;
    }

    HASH_ADD_INT(nd_head, key, cur);

    return;
}

void lRUBlockPut(int key, struct block* value) {
    LRUBlockCache *cur = NULL;
    HASH_FIND_INT(blk_head, &key, cur);
    // find the inode
    if (cur != NULL) {
        HASH_DEL(blk_head, cur);

        cur->key = key;
        cur->val = value;
        cur->dirty = true;

        TracePrintf( 1, "[CACHE][LOG] Found the block, replace it with new value!\n");
    } else { // insert new
        TracePrintf( 1, "[CACHE][LOG] Can't find block %d in cache\n", key);

        if (HASH_COUNT(blk_head) == BLOCK_CACHESIZE) {
            TracePrintf( 1, "[CACHE][LOG] Cache is full, evict node\n");

            if (blk_head->dirty) lRUWriteBlk(blk_head);

            HASH_DEL(blk_head, blk_head);
        }

        cur = (LRUBlockCache *)malloc(sizeof(LRUBlockCache));
        cur->key = key;
        cur->val = value;
        cur->dirty = true;

        TracePrintf( 1, "[CACHE][LOG] Didn't find the block, create a new entry in cache!\n");
    }

    HASH_ADD_INT(blk_head, key, cur);

    return;
}

void lRUWriteNode(LRUNodeCache *cahce) {
    TracePrintf( 1, "[CACHE][LOG] Write inode %d to disk\n", cahce->key);

    int block_num = getInodeBlockNum(cahce->key);
    int offset = (cahce->key % INODE_PER_BLOCK) * INODESIZE;

    Block* blk = read_block(block_num);

    memcpy(blk->datum + offset, cahce->val, INODESIZE);
    
    write_block(block_num, (void *) blk->datum);
}


void lRUWriteBlk(LRUBlockCache *cahce) {
    TracePrintf( 1, "[CACHE][LOG] Write block %d to disk\n", cahce->key);

    WriteSector(cahce->key, (void *) cahce->val->datum);
}