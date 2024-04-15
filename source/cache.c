#include "cache.h"
// #include "uthash.h"
#include <comp421/filesystem.h>
#include <comp421/yalnix.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

LRUNodeCache *nd_head = NULL;
LRUBlockCache *blk_head = NULL;

// size of node cache
int n_size;
// size of block cache
int b_size;

/* write the inode back to disk */
void writeInode(int inum, struct inode* inode) {
    if (inum <= 0 || inum > INODE_NUM) {
        TracePrintf( 1, "[SERVER][ERR] writeInode: Invalid inum %d\n", inum);
        return;
    }

    int block_num = getInodeBlockNum(inum);
    int offset = (inum % INODE_PER_BLOCK) * INODESIZE;

    Block* blk = read_block(block_num);

    memcpy(blk->datum + offset, inode, INODESIZE);

    WriteSector(block_num, (void *) blk->datum);
};

/* find the inode given the inum */
struct inode* findInode(int inum) {
    if (inum <= 0 || inum > INODE_NUM) {
        TracePrintf( 1, "[SERVER][ERR] findInode: Invalid inum %d\n", inum);
        return NULL;
    }
    
    int block_num = getInodeBlockNum(inum);
    int offset = (inum % INODE_PER_BLOCK) * INODESIZE;

    Block* blk = read_block(block_num);
    struct inode* inode = (struct inode*)malloc(sizeof(struct inode));
    
    memcpy(inode, blk->datum + offset, INODESIZE);
    free(blk);

    return inode;
}

void setInodeFree(int inum) {
    if (inum <= 0 || inum > INODE_NUM) {
        TracePrintf( 1, "[SERVER][ERR] findInode: Invalid inum %d\n", inum);
        return;
    }
    
    int block_num = getInodeBlockNum(inum);
    int offset = (inum % INODE_PER_BLOCK) * INODESIZE;

    Block* blk = read_block(block_num);
    struct inode* inode = (struct inode*)malloc(sizeof(struct inode));
    
    memcpy(inode, blk->datum + offset, INODESIZE);
    inode->type = INODE_FREE;
    // copy back
    memcpy(blk->datum + offset, inode, INODESIZE);
    free(blk);

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

/* LRU Cache Interface */
void init_node_lru(int nodeSize, int blkSize) {
    n_size = nodeSize;
    b_size = blkSize;
}

struct inode* lRUGetNode(int key) {
    LRUNodeCache *cur = NULL;
    HASH_FIND_INT(nd_head, &key, cur);
    if (cur != NULL) {
        HASH_DEL(nd_head, cur);
        HASH_ADD_INT(nd_head, key, cur);
        return cur->val;
    }
    // not found
    return NULL;
}

struct block* lRUGetBlk(int key) {
    LRUBlockCache *cur = NULL;
    HASH_FIND_INT(blk_head, &key, cur);
    if (cur != NULL) {
        HASH_DEL(blk_head, cur);
        HASH_ADD_INT(blk_head, key, cur);
        return cur->val;
    }
    // not found
    return NULL;
}

    void lRUNodePut(int key, struct inode* value)
{
    LRUNodeCache *cur = NULL, *next = NULL;
    HASH_FIND_INT(nd_head, &key, cur);
    // find the inode
    if (cur != NULL) {
        HASH_DEL(nd_head, cur);
        cur->key = key;
        cur->val = value;
        HASH_ADD_INT(nd_head, key, cur);
    } else { // insert new
        int cnt = HASH_COUNT(nd_head);
        if (cnt == n_size) {
            HASH_ITER(hh, nd_head, cur, next) {
                HASH_DEL(nd_head, cur);
                free(cur);
                break;
            }
        }
        LRUNodeCache *new_node = (LRUNodeCache *)malloc(sizeof(LRUNodeCache));
        new_node->key = key;
        new_node->val = value;
        HASH_ADD_INT(nd_head, key, new_node);
    }
    return;
}

void lRUBlockPut(int key, struct block* value)
{
    LRUBlockCache *cur = NULL, *next = NULL;
    HASH_FIND_INT(blk_head, &key, cur);
    // find the inode
    if (cur != NULL) {
        HASH_DEL(blk_head, cur);
        cur->key = key;
        cur->val = value;
        HASH_ADD_INT(blk_head, key, cur);
    } else { // insert new
        int cnt = HASH_COUNT(blk_head);
        if (cnt == n_size) { // evict a block
            HASH_ITER(hh, blk_head, cur, next) {
                HASH_DEL(blk_head, cur);
                free(cur);
                break;
            }
        }
        LRUBlockCache *new_node = (LRUBlockCache *)malloc(sizeof(LRUBlockCache));
        new_node->key = key;
        new_node->val = value;
        HASH_ADD_INT(blk_head, key, new_node);
    }
    return;
}
