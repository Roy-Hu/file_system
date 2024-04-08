#include "call.h"
#include "helper.h"
#include "inode.h"
#include "disk.h"

#include <comp421/yalnix.h>
#include <comp421/filesystem.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


int create_file(int inode, char* pName) {
    TracePrintf( 1, "[SERVER][LOG] Create file\n");
    if (checkNnormalizePathname(pName) == ERROR) {
        TracePrintf( 1, "[SERVER][ERR] Cannot normalize filename %s\n", pName);
    }

    // found the inum of the last dir (before the last slash)
    int inum = findInum(pName, inode);
    if (inum == ERROR) {
        TracePrintf( 1, "[SERVER][ERR] Fail to find parent dir for %s, non-existing dir!\n", pName);
        return ERROR;
    }

    // TO-DO:
    // 1. check the file name is created or not
    // 2. if not:
    //      find a free position in the block and allocated a new inode for the file
    //      create a new dir_entry for that filename associate with that inum just
    //      created and put the entry in the block of parent dir
    // 3. if created:
    char* filename = getLastFilename(pName);
    // reply with ERROR?
    if (filename[0 ]== '\0') {
        TracePrintf( 1, "[SERVER][ERR] Empty file name!\n");
        return ERROR;
    }

    TracePrintf( 1, "[SERVER][LOG] Get file name %s\n", filename);

    int file_inum = retrieveDir(inum, filename);
    // create new file
    if (file_inum == ERROR) {
        file_inum = touch(file_inum, filename);
        if (file_inum == ERROR) {
            TracePrintf( 1, "[SERVER][ERR] Fail creating file\n");
            // reply error
            return ERROR;
        }

        struct inode* parent_node = findInode(inum);

        // put the entry in the block of parent dir
        addInodeEntry(parent_node, file_inum, filename);

        TracePrintf( 1, "[SERVER][INF] Successfully created file %s with inum %d\n", filename, file_inum);
    }
    // already existed
    // more operations need to be added
    else {
        struct inode* inode = findInode(file_inum);

        if (inode->type != INODE_REGULAR) {
            TracePrintf( 1, "[SERVER][ERR] %s's inode type is not REGULAR\n", filename);
            return ERROR;
        }

        TracePrintf( 1, "[SERVER][LOG] File %s already exists\n", filename);

        // truncates the size to 0 
        inode->size = 0;
    }
    
    return file_inum;
}

/* create a file during the */
int touch(int inum, char* filename) {
    (void) inum;
    (void) filename;
    TracePrintf( 1, "[SERVER][LOG] Touch a new file for %s\n", filename);
    int new_inum = 0;
    // allocate a new inode number
    int i = 1;
    for (; i < INODE_NUM + 1; ++i) {
        if (freeInodes[i] == 0) {
            new_inum = i;
            freeInodes[i] = 1;
            break;
        }
    }
    if (new_inum == 0) {
        TracePrintf( 1, "[SERVER][ERR] No availiable inode!\n");
        return ERROR;
    }
    // create a new entry for the file
    struct inode* inode_entry = findInode(new_inum);
    createInode(inode_entry, INODE_REGULAR);

    // Why reuse++?
    // inode_entry->reuse++;


    // create a new dir

    // struct dir_entry dir;
    // dir.inum = new_inum;

    // memset(&entry.name, '\0', DIRNAMELEN);
    // int filenameLen = strlen(filename);
    // if(filenameLen > DIRNAMELEN){
	// 	 memcpy(&entry.name, filename, DIRNAMELEN);
	// }
    // else {
    //     memcpy(&entry.name, filename, filenameLen);
    // }

    // adding new dir to parent inode


    return new_inum;
}