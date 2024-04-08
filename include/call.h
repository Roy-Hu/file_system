#ifndef CALL_H
#define CALL_H

#include <stdbool.h>

int create_file(int inode, char* pName, bool dir);

int touch(int inum, char* filename);

#endif