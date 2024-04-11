#include "call.h"
#include "inode.h"
#include "cache.h"
#include "helper.h"

#include <comp421/yalnix.h>


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern int CURR_INODE;

// On success, return the file's inum
int yfsOpen(int inode, char* pName, int *parentInum) {
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
    *parentInum = inumFind(pName, inode);
    if (*parentInum == ERROR) {
        TracePrintf( 1, "[SERVER][ERR] Fail to find parent dir for %s, non-existing dir!\n", pName);
        return ERROR;
    }

    return inumRetrieve(*parentInum, lName, INODE_REGULAR);
}

int yfsCreate(char* pName, int currInum) {
    TracePrintf( 1, "[SERVER][LOG] Create file %s\n", pName);

    if (normPathname(pName) == ERROR) {
        TracePrintf( 1, "[SERVER][ERR] Cannot normalize path name %s\n", pName);
        return ERROR;
    }

    return create(pName, INODE_REGULAR, currInum);
}

/* return the bytes written to the file*/
int yfsWrite(int inum, void* buf, int curpos, int size) {
    TracePrintf( 1, "[SERVER][LOG] Write %d byte at inum %d, pos %d\n", size, inum, curpos);

    return inodeReadWrite(inum, buf, curpos, size, FILEWRITE);
}

int yfsRead(int inum, void* buf, int curpos, int size) {
    TracePrintf( 1, "[SERVER][LOG] Read %d byte at inum %d, pos %d\n", size, inum, curpos);

    return inodeReadWrite(inum, buf, curpos, size, FILEREAD);
}

// return the size of inode (eof)
int yfsSeek(int inum) {
    struct inode* node = findInode(inum); 
    if (node == NULL) return ERROR;
    return node->size;
}

int yfsMkdir(char* pName, int currInum) {
    TracePrintf( 1, "[SERVER][LOG] Create Directory\n");

    if (normPathname(pName) == ERROR) {
        TracePrintf( 1, "[SERVER][ERR] Cannot normalize path name %s\n", pName);
        return ERROR;
    }

    return create(pName, INODE_DIRECTORY, currInum);
}

int yfsRmDir(char* pName, int currInum) {
    TracePrintf(1, "[SERVER][LOG] Remove Directory\n");
    int parent_inum = inumFind(pName, currInum);
    if (parent_inum == ERROR) {
        TracePrintf(1, "[SERVER][LOG] RmDir: Fail to find parent_inum\n");
        return ERROR;
    }
    char* lName = getLastName(pName);
    int dir_inum = inumRetrieve(parent_inum, lName, INODE_DIRECTORY);
    if (dir_inum == ERROR || dir_inum == ROOTINODE) {
        TracePrintf(1, "[SERVER][LOG] RmDir: Fail to find dir_inum\n");
        return ERROR;
    }
    struct inode* dir_inode = findInode(dir_inum);
    if (dir_inode->type != INODE_DIRECTORY || dir_inode == NULL) {
        TracePrintf(1, "[SERVER][LOG] RmDir: Found Invalid Inode\n");
        return ERROR;
    }
    TracePrintf(1, "[SERVER][LOG] RmDir: Start checking dir inodes %d\n", dir_inode->size);
    int i =  2*sizeof(struct dir_entry);
    struct dir_entry* entry = (struct dir_entry*)malloc(sizeof(struct dir_entry));
    Block* blk = NULL;
    for (; i < dir_inode->size; i += sizeof(struct dir_entry)) {
        int block_num = i / BLOCKSIZE;
        int block_offset = i % BLOCKSIZE;

        if (block_num < NUM_DIRECT) {
            blk = read_block(dir_inode->direct[block_num]);
            memcpy(entry, blk->datum + block_offset, sizeof(struct dir_entry));
        } else {
            int indirect_block_num = block_num - NUM_DIRECT;
            Block* indirectBlk = read_block(dir_inode->indirect);
            
            int* indirect = (int*)indirectBlk->datum;
            blk = read_block(indirect[indirect_block_num]);

            memcpy(entry, blk->datum + block_offset, sizeof(struct dir_entry));
        }
        TracePrintf(1, "[SERVER][LOG] RmDir: Entry inum: %d!\n", entry->inum);
        if (entry->inum != 0) {
            TracePrintf(1, "[SERVER][LOG] RmDir: Directory not empty, cannot be removed!\n");
            return ERROR;
        }
    }
    free(entry);

    int res = inodeDelEntry(parent_inum, dir_inum);
    if (res == ERROR) {
        TracePrintf(1, "[SERVER][LOG] RmDir: Fail to remove directory entry from its parent dir!\n");
        return ERROR;
    }
    return res;
}

int create(char* pName, int type, int currInum) {
    char* lName = getLastName(pName);
    if (lName[0]== '\0') {
        TracePrintf( 1, "[SERVER][ERR] Empty last name!\n");
        return ERROR;
    }

    int parentInum;
    // finding the file name
    int fileInum = yfsOpen(currInum, pName, &parentInum);
    if (parentInum == ERROR) {
        TracePrintf( 1, "[SERVER][ERR] Fail to find parent dir for %s\n", pName);
        return ERROR;
    }

    // create new file
    if (fileInum == ERROR) {
        fileInum = getFreeInode();
        if (fileInum == 0) {
            TracePrintf( 1, "[SERVER][ERR] No availiable inode!\n");
            return ERROR;
        }
        //TracePrintf( 1, "[SERVER][ERR] No availiable inode!\n");
        // create a new entry for the file
        inodeCreate(fileInum, parentInum, type);

        // put the entry in the block of parent dir
        inodeAddEntry(parentInum, fileInum, lName);

        TracePrintf( 1, "[SERVER][INF] Successfully create %s with inum %d\n", lName, fileInum);
    }
    // already existed
    // more operations need to be added
    else {
        if (type == INODE_DIRECTORY) {
            TracePrintf( 1, "[SERVER][ERR] Dir %s already exists\n", lName);
            return ERROR;
        }

        struct inode* inode = findInode(fileInum);

        if (inode->type != INODE_REGULAR) {
            TracePrintf( 1, "[SERVER][ERR] %s's inode %d type is not REGULAR\n", lName, fileInum);
            return ERROR;
        }

        TracePrintf( 1, "[SERVER][LOG] File %s already exists\n", lName);

        // truncates the size to 0 
        inode->size = 0;
    }
    
    return fileInum;
}

int yfsLink(char* oldname, char* newname) {
    TracePrintf( 1, "[SERVER][LOG] Link %s to %s\n", newname, oldname);

    int parentInum;
    int fileinum = yfsOpen(INVALID_INUM, oldname, &parentInum);
    if (fileinum == ERROR) {
        TracePrintf( 1, "[SERVER][ERR] Fail to find file %s\n", oldname);
        return ERROR;
    }

    if (parentInum == ERROR) {
        TracePrintf( 1, "[SERVER][ERR] Fail to find parent dir for %s\n", oldname);
        return ERROR;
    }

    inodeAddEntry(parentInum, fileinum, newname);
    
    return 0;
}

int yfsUnLink(char* pName) {
    TracePrintf( 1, "[SERVER][LOG] UnLink %s\n", pName);
    printdirentry(1);
    int parentInum;
    int fileinum = yfsOpen(INVALID_INUM, pName, &parentInum);
    if (fileinum == ERROR) {
        TracePrintf( 1, "[SERVER][ERR] Fail to find file %s\n", pName);
        return ERROR;
    }

    struct inode* inode = findInode(fileinum);
    if (inode->type == INODE_DIRECTORY) {
        TracePrintf( 1, "[SERVER][ERR] %s is a directory, cannot unlink\n", pName);
        return ERROR;
    }
    
    if (parentInum == ERROR) {
        TracePrintf( 1, "[SERVER][ERR] Fail to find parent dir for %s\n", pName);
        return ERROR;
    }

    return inodeDelEntry(parentInum, fileinum);
}