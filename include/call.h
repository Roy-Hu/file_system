#ifndef CALL_H
#define CALL_H

#include <stdbool.h>

#include <comp421/iolib.h>
#include <comp421/filesystem.h>

int yfsOpen(int currInum, char* pName, int *parent_inum);

int yfsChDir(char *pName, int currInum);

int yfsCreate(char* pName, int currInum);

int yfsWrite(int inum, void* buf, int curpos, int size, int reuse);

int yfsRead(int inum, void* buf, int curpos, int size, int reuse);

int yfsMkdir(char* pName, int currInum);

int create(char* pName, int type, int currInum);

int yfsSeek(int inum);

int yfsLink(char* oldname, char* newname, int currInum);

int yfsUnLink(char* pName, int currInum);

int yfsRmDir(char* pName, int currInum);

int yfsStat(char* pName, int currInum, struct Stat *stat);

void yfsSync();

void yfsShutdown();

#endif