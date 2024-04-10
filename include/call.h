#ifndef CALL_H
#define CALL_H

#include <stdbool.h>
#include <comp421/filesystem.h>

int yfsOpen(int inode, char* pName, int *parent_inum);

int yfsCreate(char* pName, int currInum);

int yfsWrite(int inum, void* buf, int curpos, int size);

int yfsRead(int inum, void* buf, int curpos, int size);

int yfsMkdir(char* pName, int currInum);

int create(char* pName, int type, int currInum);

int yfsSeek(int inum);

int yfsLink(char* oldname, char* newname);

int yfsUnLink(char* pName);

#endif