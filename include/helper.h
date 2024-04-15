#ifndef HELPER_H
#define HELPER_H

#include <comp421/filesystem.h>
#include <stdbool.h>

int normPathname(char* pathname);

char* getLastName(const char* path);

/* set the newly created directory/file name */
void setdirName(struct dir_entry* entry, char* filename);

bool cmpDirName(struct dir_entry* entry, char* filename);

#endif