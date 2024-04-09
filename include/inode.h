#ifndef INODE_H
#define INODE_H

#include <comp421/filesystem.h>

#define END_POS -1

#define INVALID_INUM 0

#define FILEWRITE 0
#define FILEREAD 1
#define DIRUPDATE 2

void inodeCreate(int inum, int parent_inum, int type);

void inodeAddEntry(int parent_inum, int file_inum, char *name);

int inumFind(char *pathname, int curr_inum);

int inumRetrieve(int inum, char *dirname, int type);

int inodeReadWrite(int inum, void *buf, int curpos, int size, int type);

#endif