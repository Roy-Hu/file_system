#ifndef CACHE_H
#define CACHE_H
#include <comp421/filesystem.h>
#include <stdbool.h>
#include <comp421/yalnix.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

struct Block* read_block(int BNum);
struct inode* findInode(int Inum);


#endif