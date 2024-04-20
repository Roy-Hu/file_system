#include "cache.h"
#include "log.h"

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

/* write the inode */
int writeInode(int inum, struct inode* inode) {
    if (inum <= 0 || inum > INODE_NUM) {
        TracePrintf( ERR, "[SERVER][ERR] writeInode: Invalid inum %d\n", inum);
        return ERROR;
    }

    lRUNodePut(inum, inode);

    return 0;
}

/* find the inode given the inum */
struct inode* findInode(int inum) {
    if (inum <= 0 || (INODE_NUM != 0 && inum > INODE_NUM)) {
        TracePrintf( ERR, "[SERVER][ERR] findInode: Invalid inum %d\n", inum);
        return NULL;
    }
    
    return lRUGetNode(inum);
}

/* read a block, save it in a struct and return a pointer to the block */
Block* read_block(int bNum) {    
    return lRUGetBlk(bNum);
}

/* read a block, save it in a struct and return a pointer to the block */
int write_block(int bNum, void* data) {
    lRUBlockPut(bNum, data);

    return 0;
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
        TracePrintf( TRC, "[SERVER][CACHE][TRC] Find indoe %d in cache\n", key);

        HASH_DEL(nd_head, cur);
    } else {
        TracePrintf( TRC, "[SERVER][CACHE][TRC] Can't find indoe %d in cache\n", key);

        if (HASH_COUNT(nd_head) == INODE_CACHESIZE) {
            TracePrintf( TRC, "[SERVER][CACHE][TRC] Cache is full, evict node\n");

            if (nd_head->dirty) {
                int block_num = getInodeBlockNum(nd_head->key);
                int offset = (nd_head->key % INODE_PER_BLOCK) * INODESIZE;

                Block* blk = read_block(block_num);

                memcpy(blk->datum + offset, nd_head->val, INODESIZE);
                
                write_block(block_num, (void *) blk->datum);
            }
            
            HASH_DEL(nd_head, nd_head);
        }

        int block_num = getInodeBlockNum(key);
        int offset = (key % INODE_PER_BLOCK) * INODESIZE;

        Block* blk = read_block(block_num);
        if (blk == NULL) {
            TracePrintf( ERR, "[SERVER][CACHE][ERR] Read block %d failed\n", block_num);
            return NULL;
        }

        struct inode* inode = (struct inode*)malloc(sizeof(struct inode));

        memcpy(inode, blk->datum + offset, INODESIZE);

        cur = (LRUNodeCache *)malloc(sizeof(LRUNodeCache));
        cur->val = inode;
        cur->key = key;
        cur->dirty = false;

        TracePrintf( TRC, "[SERVER][CACHE][TRC] Add indoe %d in cache\n", key);
    }

    HASH_ADD_INT(nd_head, key, cur);

    return cur->val;
}

struct block* lRUGetBlk(int key) {
    LRUBlockCache *cur = NULL;

    HASH_FIND_INT(blk_head, &key, cur);
    if (cur != NULL) { // found, move it to the head
        TracePrintf( TRC, "[SERVER][CACHE][TRC] Find block %d in cache\n", key);

        HASH_DEL(blk_head, cur);
    } else {
        TracePrintf( TRC, "[SERVER][CACHE][TRC] Can't find block %d in cache\n", key);

        if (HASH_COUNT(blk_head) == BLOCK_CACHESIZE) {
            TracePrintf( TRC, "[SERVER][CACHE][TRC] Cache is full, evict node\n");

            if (blk_head->dirty) WriteSector(blk_head->key, (void *) blk_head->val->datum);

            HASH_DEL(blk_head, blk_head);
        }

        Block* block = (Block*)malloc(sizeof(Block));

        int res = ReadSector(key, (void *) block->datum);
        if (res == ERROR) {
            TracePrintf( ERR, "[SERVER][CACHE][ERR] ReadSector failed\n");
            return NULL;
        }

        cur = (LRUBlockCache *)malloc(sizeof(LRUBlockCache));
        cur->val = block;
        cur->key = key;
        cur->dirty = false;

        TracePrintf( TRC, "[SERVER][CACHE][TRC] Add block %d in cache\n", key);
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

        TracePrintf( TRC, "[SERVER][CACHE][TRC] Found the node, replace it with new value!\n");
    } else { // insert new
        TracePrintf( TRC, "[SERVER][CACHE][TRC] Can't find inode %d in cache\n", key);

        if (HASH_COUNT(nd_head) == INODE_CACHESIZE) {
            TracePrintf( TRC, "[SERVER][CACHE][TRC] Cache is full, evict node\n");

            if (nd_head->dirty) {
                int block_num = getInodeBlockNum(nd_head->key);
                int offset = (nd_head->key % INODE_PER_BLOCK) * INODESIZE;

                Block* blk = read_block(block_num);

                memcpy(blk->datum + offset, nd_head->val, INODESIZE);
                
                write_block(block_num, (void *) blk->datum);
            }

            HASH_DEL(nd_head, nd_head);
        }

        cur = (LRUNodeCache *)malloc(sizeof(LRUNodeCache));
    }

    cur->key = key;
    cur->val = value;
    cur->dirty = true;

    HASH_ADD_INT(nd_head, key, cur);

    return;
}

void lRUBlockPut(int key, struct block* value) {
    LRUBlockCache *cur = NULL;
    HASH_FIND_INT(blk_head, &key, cur);

    // find the inode
    if (cur != NULL) {
        HASH_DEL(blk_head, cur);

        TracePrintf( TRC, "[SERVER][CACHE][TRC] Found the block, replace it with new value!\n");
    } else { // insert new
        TracePrintf( TRC, "[SERVER][CACHE][TRC] Can't find block %d in cache\n", key);

        if (HASH_COUNT(blk_head) == BLOCK_CACHESIZE) {
            TracePrintf( TRC, "[SERVER][CACHE][TRC] Cache is full, evict node\n");

            if (blk_head->dirty) WriteSector(blk_head->key, (void *) blk_head->val->datum);

            HASH_DEL(blk_head, blk_head);
        }

        cur = (LRUBlockCache *)malloc(sizeof(LRUBlockCache));
    }

    cur->key = key;
    cur->val = value;
    cur->dirty = true;

    HASH_ADD_INT(blk_head, key, cur);

    return;
}

void lRUWriteDirty() {
    LRUNodeCache *curNode, *tmpNode;
    LRUBlockCache *curBlk, *tmpBlk;

    HASH_ITER(hh, nd_head, curNode, tmpNode) {
        if (curNode->dirty) {
            curNode->dirty = false;

            int block_num = getInodeBlockNum(curNode->key);
            int offset = (curNode->key % INODE_PER_BLOCK) * INODESIZE;

            Block* blk = read_block(block_num);

            memcpy(blk->datum + offset, curNode->val, INODESIZE);
            
            write_block(block_num, (void *) blk->datum);
        }
    }

    HASH_ITER(hh, blk_head, curBlk, tmpBlk) {
        if (curBlk->dirty) {
            curBlk->dirty = false;
            WriteSector(curBlk->key, (void *) curBlk->val->datum);
        }
    }
}