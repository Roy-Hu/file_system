#include "yfs.h"
#include "cache.h"



/**
 * initialization function for yfs 
 * 1. initilize freeBlocks, freeInodes
 * 2. check and mark occupied blocks that contains
 *    all of the inodes
 * 3. 
 *  
**/
void init() {
    struct Block* block = (struct Block*)malloc(sizeof(struct Block));
    int read_header = ReadSector(1, (void *) block->datum);
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
}
