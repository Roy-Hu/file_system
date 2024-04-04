#include "cache.h"
/* LRU cache for inode and datablock */

/* read a block, save it in a struct and return a pointer to the block*/
struct Block* read_block(int BNum) {
    struct Block* block = (struct Block*)malloc(sizeof(struct Block));
    int res = ReadSector(BNUm, (void *) block->datum);
    return block;
}