#include "call.h"
#include "helper.h"
#include "inode.h"
#include "disk.h"

#include <comp421/yalnix.h>
#include <comp421/filesystem.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// On success, return the file's inum
int yfsOpen(int inode, char* pName, int *parent_inum) {
    TracePrintf( 1, "[SERVER][LOG] Open file\n");

    if (normPathname(pName) == ERROR) {
        TracePrintf( 1, "[SERVER][ERR] Cannot normalize path name %s\n", pName);
        return ERROR;
    }

    char* lName = getLastName(pName);
    if (lName[0]== '\0') {
        TracePrintf( 1, "[SERVER][ERR] Empty last name!\n");
        return ERROR;
    }

    TracePrintf( 1, "[SERVER][LOG] Get last name %s\n", lName);

    // found the inum of the last dir (before the last slash)
    *parent_inum = findInum(pName, inode);
    if (*parent_inum == ERROR) {
        TracePrintf( 1, "[SERVER][ERR] Fail to find parent dir for %s, non-existing dir!\n", pName);
        return ERROR;
    }

    return retrieveDir(*parent_inum, lName);
}

int yfsCreate(int inode, char* pName) {
    TracePrintf( 1, "[SERVER][LOG] Create file\n");

    return create(inode, pName, INODE_REGULAR);
}

int yfsMkdir(int inode, char* pName) {
    TracePrintf( 1, "[SERVER][LOG] Create Directory\n");

    return create(inode, pName, INODE_DIRECTORY);
}

int create(int inode, char* pName, int type) {
    char* lName = getLastName(pName);
    if (lName[0]== '\0') {
        TracePrintf( 1, "[SERVER][ERR] Empty last name!\n");
        return ERROR;
    }

    int parent_inum;
    // finding the file name
    int file_inum = yfsOpen(inode, pName, &parent_inum);
    if (parent_inum == ERROR) return ERROR;

    // create new file
    if (file_inum == ERROR) {
        file_inum = getFreeInode();
        if (file_inum == 0) {
            TracePrintf( 1, "[SERVER][ERR] No availiable inode!\n");
            return ERROR;
        }

        // create a new entry for the file
        struct inode* inode_entry = findInode(file_inum);

        createInode(inode_entry, file_inum, parent_inum, type);

        struct inode* parent_node = findInode(parent_inum);

        // put the entry in the block of parent dir
        addInodeEntry(parent_node, file_inum, lName);

        TracePrintf( 1, "[SERVER][INF] Successfully create %s with inum %d\n", lName, file_inum);
    }
    // already existed
    // more operations need to be added
    else {
        if (type == INODE_DIRECTORY) {
            TracePrintf( 1, "[SERVER][ERR] Dir %s already exists\n", lName);
            return ERROR;
        }

        struct inode* inode = findInode(file_inum);

        if (inode->type != INODE_REGULAR) {
            TracePrintf( 1, "[SERVER][ERR] %s's inode type is not REGULAR\n", lName);
            return ERROR;
        }

        TracePrintf( 1, "[SERVER][LOG] File %s already exists\n", lName);

        // truncates the size to 0 
        inode->size = 0;
    }
    
    return file_inum;
}

// /* create a file during the */
// int touch(int inum, char* filename) {
//     (void) inum;
//     (void) filename;
//     TracePrintf( 1, "[SERVER][LOG] Touch a new file for %s\n", filename);

//     int new_inum = getFreeInode();
 
//     if (new_inum == 0) {
//         TracePrintf( 1, "[SERVER][ERR] No availiable inode!\n");
//         return ERROR;
//     }
//     // create a new entry for the file
//     struct inode* inode_entry = findInode(new_inum);
//     createInode(inode_entry, INODE_REGULAR, false);

//     // Why reuse++?
//     // inode_entry->reuse++;


//     // create a new dir

//     // struct dir_entry dir;
//     // dir.inum = new_inum;

//     // memset(&entry.name, '\0', DIRNAMELEN);
//     // int filenameLen = strlen(filename);
//     // if(filenameLen > DIRNAMELEN){
// 	// 	 memcpy(&entry.name, filename, DIRNAMELEN);
// 	// }
//     // else {
//     //     memcpy(&entry.name, filename, filenameLen);
//     // }

//     // adding new dir to parent inode


//     return new_inum;
// }