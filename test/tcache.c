#include <stdio.h>
#include "../include/uthash.h"

typedef struct block {
    char datum[5];
} Block;

typedef struct {
    int key;    // block num
    struct block* val;
    UT_hash_handle hh;
} LRUBlockCache;

LRUBlockCache *blk_head = NULL;
int b_size;
void init_node_lru(int blkSize);

struct block* lRUGetBlk(int key);

void lRUBlockPut(int key, struct block* value);
void init_node_lru(int blkSize);

void lRUBlockPut(int key, struct block* value) {
    LRUBlockCache *cur = NULL, *next = NULL;
    HASH_FIND_INT(blk_head, &key, cur);
    // find the inode
    if (cur != NULL) {
        HASH_DEL(blk_head, cur);
        cur->key = key;
        cur->val = value;
        HASH_ADD_INT(blk_head, key, cur);
        TracePrintf( 1, "[CACHE][LOG] Found the block, replace it with new value!\n");

    } else { // insert new
        int cnt = HASH_COUNT(blk_head);
        if (cnt == b_size) { // evict a block
            HASH_ITER(hh, blk_head, cur, next) {
                HASH_DEL(blk_head, cur);
                free(cur);
                break;
            }
        TracePrintf( 1, "[CACHE][LOG] Full, evicting a blk\n");

        }
        LRUBlockCache *new_node = (LRUBlockCache *)malloc(sizeof(LRUBlockCache));
        new_node->key = key;
        new_node->val = value;
        HASH_ADD_INT(blk_head, key, new_node);
        TracePrintf( 1, "[CACHE][LOG] Didn't find the block, create a new entry in cache!\n");

    }
    return;
}


struct block* lRUGetBlk(int key) {
    LRUBlockCache *cur = NULL;
    HASH_FIND_INT(blk_head, &key, cur);
    if (cur != NULL) { // found, move it to the head
        TracePrintf( 1, "[CACHE][LOG] Found block, put it to the head...\n");
        HASH_DEL(blk_head, cur);
        HASH_ADD_INT(blk_head, key, cur);
        return cur->val;
    }
    // not found
    TracePrintf( 1, "[CACHE][LOG] Can't find block!\n");
    return NULL;
}

void init_node_lru(int blkSize) {
    b_size = blkSize;
}

int
main()
{
    TracePrintf( 1, "Cache test in progress\n");
    Block* blk = (Block*)malloc(sizeof(Block));
    Block* blk2 = (Block*)malloc(sizeof(Block));
    init_node_lru(1);
    lRUBlockPut(1, blk);
    lRUGetBlk(1);
    lRUBlockPut(2, blk2);


    
    return (0);
}
