#ifndef INODE_H
#define INODE_H

#include <comp421/filesystem.h>

#define END_POS -1

#define INVALID_INUM 0

#define FILEWRITE 0
#define FILEREAD 1
#define DIRUPDATE 2


int getFreeInode();

void createInode(int inum, int parent_inum, int type);

void addInodeEntry(int parent_inum, int file_inum, char* name);

int normPathname(char* pathname);

int findInum(char* pathname, int curr_inum);

int retrieve(int inum, char* dirname, int type);

void setdirName(struct dir_entry* entry, char* filename);

int inodeReadWrite(int inum, void* buf, int curpos, int size, int type);

#endif