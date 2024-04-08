#include "disk.h"

#include <comp421/yalnix.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
    
    TracePrintf( 1, "[SERVER][ERR] No free block available\n");
    return ERROR;
}

void writeInode(int Inum, struct inode* inode) {
    if (Inum <= 0 || Inum > INODE_NUM) {
        TracePrintf( 1, "[SERVER][ERR] Invalid Inum %d\n", Inum);
        return;
    }

    int block_num = getInodeBlockNum(Inum);
    int offset = (Inum % INODE_PER_BLOCK) * INODESIZE;

    Block* blk = read_block(block_num);

    memcpy(blk->datum + offset, inode, INODESIZE);

    WriteSector(block_num, (void *) blk->datum);
};

struct inode* findInode(int Inum) {
    if (Inum <= 0 || Inum > INODE_NUM) {
        TracePrintf( 1, "[SERVER][ERR] Invalid Inum %d\n", Inum);
        return NULL;
    }
    
    int block_num = getInodeBlockNum(Inum);
    int offset = (Inum % INODE_PER_BLOCK) * INODESIZE;

    Block* blk = read_block(block_num);
    struct inode* inode = (struct inode*)malloc(sizeof(struct inode));
    
    memcpy(inode, blk->datum + offset, INODESIZE);
    free(blk);

    return inode;
}

/* read a block, save it in a struct and return a pointer to the block*/
Block* read_block(int BNum) {
    Block* block = (Block*)malloc(sizeof(Block));

    int res = ReadSector(BNum, (void *) block->datum);
    if (res == ERROR) {
        TracePrintf( 1, "[SERVER][ERR] ReadSector failed\n");
        return NULL;
    }

    return block;
}
