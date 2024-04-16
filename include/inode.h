#ifndef INODE_H
#define INODE_H

#include <comp421/filesystem.h>

#define END_POS -1

#define INVALID_INUM 0

#define FILEWRITE 0
#define FILEREAD 1
#define DIRUPDATE 2

int inodeCreate(int inum, int parent_inum, int type);

void inodeAddEntry(int parent_inum, int file_inum, char *name);

int inumFind(char *pathname, int curr_inum);

int inumRetrieve(int inum, char *dirname, int type, int open_flag);

int inodeReadWrite(int inum, void *buf, int curpos, int size, int type);

int inodeDelete(int inum);

int inodeDelEntry(int parentInum, int fileInum, char* eName);

void printdirentry(int inum);

void printInode(int inum);

int incrementNlink(int inum);

int decrementNlink(int inum);

#endif