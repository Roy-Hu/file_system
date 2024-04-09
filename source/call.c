#include "call.h"
#include "inode.h"
#include "cache.h"
#include "helper.h"

#include <comp421/yalnix.h>


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// On success, return the file's inum
int yfsOpen(int inode, char* pName, int *parent_inum) {
    TracePrintf( 1, "[SERVER][LOG] Open file %s\n", pName);

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

    return retrieve(*parent_inum, lName, INODE_REGULAR);
}

int yfsCreate(char* pName) {
    TracePrintf( 1, "[SERVER][LOG] Create file %s\n", pName);

    return create(pName, INODE_REGULAR);
}

/* return the bytes written to the file*/
int yfsWrite(int inum, void* buf, int curpos, int size) {
    TracePrintf( 1, "[SERVER][LOG] Write %d byte at inum %d, pos %d\n", size, inum, curpos);

    // struct inode* node = findInode(inum);
    // // if (node->size < size) {
    // //     // need functions enlarge the file to hold the size
    // //     continue;
    // // }
    // // position in the buffer to write
    // int pos = 0;
    // // size written
    // int sz = 0;
    // for (; pos < size; pos+= sz) {
    //     // write position
    //     char* start = findNextWritingPos(curpos + sz, node);
    //     if (size - pos < BLOCKSIZE - (curpos + pos) % BLOCKSIZE) sz = size - pos;
    //     memcpy(start,buf + pos , sz);
    // }

    return inodeReadWrite(inum, buf, curpos, size, FILEWRITE);
}

int yfsRead(int inum, void* buf, int curpos, int size) {
    TracePrintf( 1, "[SERVER][LOG] Read %d byte at inum %d, pos %d\n", size, inum, curpos);

    return inodeReadWrite(inum, buf, curpos, size, FILEREAD);
}

// /* find the next writing position in the file of one block */
// char* findNextWritingPos(int curpos, struct inode* node) {
//     int blockOfFile = curpos / BLOCKSIZE;
//     char* res;
//     if (blockOfFile < NUM_DIRECT) {
//         Block* blk = read_block(node->direct[blockOfFile]);
//         res = ((char*)blk->datum + curpos % BLOCKSIZE);
        
//     }else {
//         int indirect_block_num = blockOfFile - NUM_DIRECT;
//         Block* indirectBlk = read_block(node->indirect);
        
//         int* indirect = (int*)indirectBlk->datum;
//         Block* blk = read_block(indirect[indirect_block_num]);
//         res = ((char*)blk->datum + curpos % BLOCKSIZE);
//     }
//     return res;
// }

int yfsMkdir(char* pName) {
    TracePrintf( 1, "[SERVER][LOG] Create Directory\n");

    return create(pName, INODE_DIRECTORY);
}

int create(char* pName, int type) {
    char* lName = getLastName(pName);
    if (lName[0]== '\0') {
        TracePrintf( 1, "[SERVER][ERR] Empty last name!\n");
        return ERROR;
    }

    int parent_inum;
    // finding the file name
    int file_inum = yfsOpen(INVALID_INUM, pName, &parent_inum);
    if (parent_inum == ERROR) {
        TracePrintf( 1, "[SERVER][ERR] Fail to find parent dir for %s\n", pName);
        return ERROR;
    }

    // create new file
    if (file_inum == ERROR) {
        file_inum = getFreeInode();
        if (file_inum == 0) {
            TracePrintf( 1, "[SERVER][ERR] No availiable inode!\n");
            return ERROR;
        }

        // create a new entry for the file
        createInode(file_inum, parent_inum, type);

        // put the entry in the block of parent dir
        addInodeEntry(parent_inum, file_inum, lName);

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
            TracePrintf( 1, "[SERVER][ERR] %s's inode %d type is not REGULAR\n", lName, file_inum);
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