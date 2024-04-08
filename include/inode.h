#ifndef INODE_H
#define INODE_H

#include <comp421/filesystem.h>

int getFreeInode();

void createInode(struct inode* inode, int inum, int parent_inum, int type);

void addInodeEntry(struct inode* inode, int Inum, char* name);

int normPathname(char* pathname);

int findInum(char* pathname, int curr_inum);

int retrieveDir(int inum, char* dirname);

void setdirName(struct dir_entry* entry, char* filename);

#endif