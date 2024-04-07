#include "disk.h"

#include <comp421/yalnix.h>
#include <stdlib.h>
#include <string.h>

/* 
 * find the inode based on given Inum
 * returned a copy of that inode (dynamic allocated)
 *
 */
int getInodeBlockNum(int Inum) {
    return Inum / INODE_PER_BLOCK + 1;
}

int getFreeBlock() {
    int i;
    for (i = 0; i < BLOCK_NUM; ++i) {
        if (freeBlocks[i] == 0) {
            freeBlocks[i] = 1;
            return i;
        }
    }
    
    TracePrintf(1, "[ERROR] No free block available\n");
    return ERROR;
}

struct inode* findInode(int Inum) {
    if (Inum <= 0 || Inum > INODE_NUM) {
        TracePrintf(1, "[ERROR] Invalid Inum\n");
        return NULL;
    }
    
    int block_num = getInodeBlockNum(Inum);
    int offset = (Inum % INODE_PER_BLOCK) * INODESIZE;

    struct Block* blk = read_block(block_num);
    struct inode* inode = (struct inode*)malloc(sizeof(struct inode));
    
    memcpy(inode, blk->datum + offset, INODESIZE);
    free(blk);

    return inode;
}

/* read a block, save it in a struct and return a pointer to the block*/
struct Block* read_block(int BNum) {
    struct Block* block = (struct Block*)malloc(sizeof(struct Block));

    int res = ReadSector(BNum, (void *) block->datum);
    if (res == ERROR) {
        TracePrintf(1, "[ERROR] ReadSector failed\n");
        return NULL;
    }

    return block;
}
