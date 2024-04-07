#ifndef DISK_H
#define DISK_H

#include <stdbool.h>
#include <comp421/filesystem.h>

#define INODE_PER_BLOCK 8

/**
 * structure to keep data of blocks
 * 
*/
typedef struct Block {
    char datum[SECTORSIZE];
}Block;


int INODE_NUM;

int BLOCK_NUM;

/* 
 * contains free blocks represented by booleans
 * 0: free
 * 1: occupied
 */
bool* freeBlocks;

bool* freeInodes;

int getInodeBlockNum(int Inum);

struct inode* findInode(int Inum);

int getFreeBlock();

struct Block* read_block(int BNum);

#endif