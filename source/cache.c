#include "cache.h"

#include <comp421/filesystem.h>
#include <comp421/yalnix.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

    struct inode* test = findInode(inum);
    TracePrintf( 1, "[SERVER][LOG] Inode %d, type %d, size %d\n", inum, inode->type, inode->size);
    TracePrintf( 1, "[SERVER][LOG] Test Inode %d, type %d, size %d\n", inum, test->type, test->size);
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
