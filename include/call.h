#ifndef CALL_H
#define CALL_H

#include <stdbool.h>
#include <comp421/filesystem.h>

// char* findNextWritingPos(int curpos, struct inode* node);

int yfsOpen(int inode, char* pName, int *parent_inum);

int yfsCreate(char* pName);

int yfsWrite(int inum, void* buf, int curpos, int size);

int yfsRead(int inum, void* buf, int curpos, int size);

int yfsMkdir(char* pName);

int create(char* pName, int type);

// int touch(int inum, char* filename);

#endif