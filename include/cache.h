#ifndef CACHE_H
#define CACHE_H

#include "disk.h"

struct inode* findInode(int inum);

Block* read_block(int bNum);

void writeInode(int inum, struct inode* inode);

void setInodeFree(int inum);

#endif