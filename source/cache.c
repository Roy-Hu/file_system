#include "cache.h"
/* LRU cache for inode and datablock */

/* read a block, save it in a struct and return a pointer to the block*/
struct Block* read_block(int BNum) {
    struct Block* block = (struct Block*)malloc(sizeof(struct Block));
    ReadSector(BNum, (void *) block->datum);
    return block;
}

/* 
 * find the inode based on given Inum
 * returned a copy of that inode (dynamic allocated)
 *
 */
struct inode* findInode(int Inum) {
    int block_num = Inum / INODE_PER_BLOCK + 1;
    struct Block* blk = read_block(block_num);
    struct inode* temp = (struct inode*)malloc(sizeof(struct inode));
    int offset = (Inum % INODE_PER_BLOCK)*INODESIZE;
    memcpy(temp, blk->datum +offset, INODESIZE);
    free(blk);
    return temp;
}