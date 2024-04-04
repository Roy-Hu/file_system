#include "yfs.h"
#include "cache.h"
#include <string.h>



/**
 * initialization function for yfs 
 * 1. initilize freeBlocks, freeInodes
 * 2. check and mark occupied blocks that contains
 *    all of the inodes
 * 3. 
 *  
**/
void init() {
    // read the first block to get the header
    struct Block* block = read_block(1);
    struct fs_header* header = (struct fs_header*)malloc(sizeof(struct fs_header));
    memcpy(header, block->datum, sizeof(struct fs_header));
    INODE_NUM = header->num_inodes;
    BLOCK_NUM = header->num_blocks;
    freeBlocks = (bool*)malloc(sizeof(bool)*BLOCK_NUM);
    freeInodes = (bool*)malloc(sizeof(bool)*INODE_NUM);
    freeInodes[0] = 1;
    freeBlocks[0] = 1;
    freeBlocks[1] = 1;
    int i = 0;
    // init inode list as free
    for (int i = 1; i < INODE_NUM + 1; ++i) {
        freeInodes[i] = 0;
    }
<<<<<<< HEAD
    // init block list as free,skip 0 and 1
    for (i = 2; i < BLOCK_NUM; ++i) {
        freeBlocks[i] = 0;
    } 
    // set occupied blocks as 1
    for (i = 2; i < ((INODE_NUM+1)*INODESIZE)/BLOCKSIZE + 1; ++i) {
        freeBlocks[i] = 1;
    }
    // traverse into direct and indirect
=======
>>>>>>> parent of b213021... add some frames
}

int main() {
    return 0;
}
