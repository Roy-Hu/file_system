#include "disk.h"
#include "log.h"

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

/* get a free block from the freeblock lists  */
int getFreeBlock() {
    int i;
    for (i = 0; i < BLOCK_NUM; ++i) {
        if (freeBlocks[i] == 0) {
            freeBlocks[i] = 1;
            return i;
        }
    }
    
    printf( "[SERVER][ERR] No free block available\n");
    return ERROR;
}

/* 
 * Find free inodes in the inode list
 * Return inum if found
 * Return ERROR if not found
 */
int getFreeInode() {
    int i;
    for (i = 0; i < INODE_NUM; ++i) {
        if (freeInodes[i] == 0) {
            freeInodes[i] = 1;
            return i;
        }
    }

    printf( "[SERVER][ERR] No free inode available\n");

    return ERROR;
}
