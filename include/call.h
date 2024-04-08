#ifndef CALL_H
#define CALL_H

#include <stdbool.h>

int yfsOpen(int inode, char* pName, int *parent_inum);

int yfsCreate(int inode, char* pName);

int yfsMkdir(int inode, char* pName);

int create(int inode, char* pName, int type);
// int touch(int inum, char* filename);

#endif