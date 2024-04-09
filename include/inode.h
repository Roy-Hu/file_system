#ifndef INODE_H
#define INODE_H

#include <comp421/filesystem.h>

int getFreeInode();

void createInode(int inum, int parent_inum, int type);

void addInodeEntry(int parent_inum, int file_inum, char* name);

int normPathname(char* pathname);

int findInum(char* pathname, int curr_inum);

int retrieveDir(int inum, char* dirname);

void setdirName(struct dir_entry* entry, char* filename);

#endif