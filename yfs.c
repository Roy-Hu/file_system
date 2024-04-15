#include "yfs.h"
#include "cache.h"
#include "inode.h"
#include "disk.h"
#include "helper.h"
#include "call.h"

#include <comp421/yalnix.h>
#include <comp421/iolib.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

int msgHandler(Messgae* msg, int pid) {
    OperationType myType = msg->type;

    switch(myType) {
        case OPEN: {
            TracePrintf( 1, "[SERVER][LOG] Received Open request!\n");
            char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
            if (CopyFrom(pid, (void*)pName, msg->pathnamePtr, MAXPATHNAMELEN) == ERROR) {
                TracePrintf( 1, "[SERVER][ERR] Open: Fail to copy path name %s\n", pName);
            }

            int parent_inum;
            msg->inum = yfsOpen(msg->inum, pName, &parent_inum);
            if (msg->inum == ERROR) {
                msg->reply = ERROR;
                TracePrintf( 1, "[SERVER][ERR] Open: Fail to open file\n");
                break;
            }

            break;
        }
        case CREATE: {
            TracePrintf( 1, "[SERVER][LOG] Received Create request!\n");

            char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
            if (CopyFrom(pid, (void*)pName, msg->pathnamePtr, MAXPATHNAMELEN) == ERROR) {
                TracePrintf( 1, "[SERVER][ERR] Create: Fail copy path name %s\n", pName);
            }

            msg->inum = yfsCreate(pName, msg->inum);
            if (msg->inum == ERROR) {
                msg->reply = ERROR;
                TracePrintf( 1, "[SERVER][ERR] Create: Fail to create file\n");
                break;
            }

            break;
        }
        case READ: {
            TracePrintf( 1, "[SERVER][LOG] Received Read request!\n");

            char* buf = (char *)malloc(msg->size * sizeof(char));


            msg->size = yfsRead(msg->inum, buf, msg->pos, msg->size);

            if (CopyTo(pid, msg->bufPtr, (void*)buf, msg->size) == ERROR) {
                TracePrintf( 1, "[SERVER][ERR] Create: Fail copy to buf\n");
            }

            if (msg->size == ERROR) {
                msg->pos = ERROR;
                TracePrintf( 1, "[SERVER][ERR] Read: Fail to read file\n");
                break;
            }

            break;
        }
        case WRITE: {
            TracePrintf( 1, "[SERVER][LOG] Received Write request!\n");

            char* buf = (char *)malloc(msg->size * sizeof(char));
            if (CopyFrom(pid, (void*)buf, msg->bufPtr, msg->size) == ERROR) {
                TracePrintf( 1, "[SERVER][ERR] Create: Fail copy from buf\n");
            }

            msg->size = yfsWrite(msg->inum, (void*)buf, msg->pos, msg->size);
            if (msg->size == ERROR) {
                msg->reply = ERROR;
                TracePrintf( 1, "[SERVER][ERR] Read: Fail to read file\n");
                break;
            }

            TracePrintf( 1, "[SERVER][ERR] Write %d byte tp msg\n", msg->size);

            break;
        }
        case SEEK: {
            TracePrintf( 1, "[SERVER][LOG] Received Seek request!\n");
            msg->reply = yfsSeek(msg->inum);
            if (msg->reply == ERROR) {
                TracePrintf( 1, "[SERVER][ERR] Seek: Fail to return inum of the file\n");
                break;
            }
            break;
        }
        case LINK: {
            TracePrintf( 1, "[SERVER][LOG] Received Link request!\n");
            char* oldname = (char *)malloc(MAXPATHLEN * sizeof(char));
            if (CopyFrom(pid, (void*)oldname, msg->pathnamePtr, MAXPATHNAMELEN) == ERROR) {
                TracePrintf( 1, "[SERVER][ERR] Link: Fail copy old name %s\n", oldname);
            }

            char* newname = (char *)malloc(MAXPATHLEN * sizeof(char));
            if (CopyFrom(pid, (void*)newname, msg->bufPtr, MAXPATHNAMELEN) == ERROR) {
                TracePrintf( 1, "[SERVER][ERR] Link: Fail copy new name %s\n", newname);
            }

            // pos will be the cur dir inum while LINK
            msg->reply = yfsLink(oldname, newname, msg->inum);
            if (msg->inum == ERROR) {
                msg->reply = ERROR;
                TracePrintf( 1, "[SERVER][ERR] Link: Fail to link %s to %s\n", newname, oldname);
                break;
            }

            break;
        }
        case UNLINK: {
            TracePrintf( 1, "[SERVER][LOG] Received Unlink request!\n");

            char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
            if (CopyFrom(pid, (void*)pName, msg->pathnamePtr, MAXPATHNAMELEN) == ERROR) {
                TracePrintf( 1, "[SERVER][ERR] Create: Fail copy path name %s\n", pName);
            }

            msg->reply = yfsUnLink(pName, msg->inum);
            if (msg->inum == ERROR) {
                msg->reply = ERROR;
                TracePrintf( 1, "[SERVER][ERR] UnLink: Fail to unlink %s\n", pName);
                break;
            }

            printdirentry(msg->inum);

            break;
        }
        case MKDIR: {
            TracePrintf( 1, "[SERVER][LOG] Received Mkdir request!\n");

            char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
            if (CopyFrom(pid, (void*)pName, msg->pathnamePtr, MAXPATHNAMELEN) == ERROR) {
                TracePrintf( 1, "[SERVER][ERR] Mkdir: Fail copy path name %s\n", pName);
            }

            msg->inum = yfsMkdir(pName, msg->inum);
            if (msg->inum == ERROR) {
                msg->reply = ERROR;
                TracePrintf( 1, "[SERVER][ERR] Create: Fail to create dir\n");
                break;
            }

            break;
        }
        case RMDIR: {
            TracePrintf( 1, "[SERVER][LOG] Received RmDir request!\n");
            char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
            if (CopyFrom(pid, (void*)pName, msg->pathnamePtr, MAXPATHNAMELEN) == ERROR) {
                TracePrintf( 1, "[SERVER][ERR] Mkdir: Fail copy path name %s\n", pName);
            }
            msg->inum = yfsRmDir(pName, msg->inum);
            if (msg->inum == ERROR) {
                msg->reply = ERROR;
                TracePrintf( 1, "[SERVER][ERR] RmDir: Fail to remove dir\n");
                break;
            }

            break;
        }
        case CHDIR: {
            TracePrintf( 1, "[SERVER][LOG] Received ChDir request!\n");
            char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
            if (CopyFrom(pid, (void*)pName, msg->pathnamePtr, MAXPATHNAMELEN) == ERROR) {
                TracePrintf( 1, "[SERVER][ERR] Chdir: Fail copy path name %s\n", pName);
            }
            // pathname currInode
            msg->inum = yfsChDir(pName, msg->inum);
            if (msg->inum == ERROR) {
                msg->reply = ERROR;
                TracePrintf( 1, "[SERVER][ERR] ChDir: Fail to remove dir\n");
                break;
            }
            break;
        }
        case STAT: {
            TracePrintf( 1, "[SERVER][LOG] Received Stat request!\n");
            char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
            if (CopyFrom(pid, (void*)pName, msg->pathnamePtr, MAXPATHNAMELEN) == ERROR) {
                TracePrintf( 1, "[SERVER][ERR] Stat: Fail copy path name %s\n", pName);
            }

            struct Stat* stat = (struct Stat*)malloc(sizeof(struct Stat));
            msg->reply = yfsStat(pName, msg->inum, stat);
            if (msg->reply == ERROR) {
                TracePrintf( 1, "[SERVER][ERR] Stat: Fail to stat file\n");
                break;
            }

            if (CopyTo(pid, msg->bufPtr, (void*)stat, sizeof(struct Stat)) == ERROR) {
                TracePrintf( 1, "[SERVER][ERR] Stat: Fail to copy stat\n");
            }

            free(stat);

            break;
        }
        case SYNC: {
            break;
        }
        case SHUTDOWN: {
            break;
        }
        default: {
            break;
        }
    }

    return 0;
}

/**
 * initialization function for yfs 
 * 1. initilize freeBlocks, freeInodes
 * 2. check and mark occupied blocks that contains
 *    all of the inodes
 *  
**/
void init() {
    TracePrintf( 1, "[SERVER][LOG] Start YFS Initialization\n");
    // read the first block to get the header
    Block* block = read_block(1);
    struct fs_header* header = (struct fs_header*)malloc(sizeof(struct fs_header));
    memcpy(header, block->datum, sizeof(struct fs_header));
    INODE_NUM = header->num_inodes;
    BLOCK_NUM = header->num_blocks;
    freeBlocks = (bool*)malloc(sizeof(bool) * BLOCK_NUM);
    freeInodes = (bool*)malloc(sizeof(bool) * (INODE_NUM+1));
    freeInodes[0] = 1;
    // root inode
    freeInodes[ROOTINODE] = 1;
    freeBlocks[0] = 1;
    freeBlocks[1] = 1;
    int i = 0;
    // init inode list as free
    for (i = 2; i < INODE_NUM + 1; ++i) freeInodes[i] = 0;

    // init block list as free,skip 0 and 1
    for (i = 2; i < BLOCK_NUM; ++i) freeBlocks[i] = 0;

    // set occupied blocks as 1 for inodes
    for (i = 2; i < ((INODE_NUM + 1) * INODESIZE) / BLOCKSIZE + 1; ++i) {
        freeBlocks[i] = 1;
    }

    // traverse into direct and indirect
    for (i = 1; i < INODE_NUM + 1; ++i) {
        struct inode* temp = findInode(i);

        if (temp->type != INODE_FREE) {
            freeInodes[i] = 1;
            int j;
            // check direct block
            for (j = 0; j < NUM_DIRECT; ++j) {
                // !!! Check the condition
                if ((j + 1) * BLOCKSIZE > temp->size && !(j * BLOCKSIZE < temp->size)) {
                    break;
                }

                freeBlocks[temp->direct[j]] = 1;
            }

            // check indirect block
            if (temp->size > NUM_DIRECT * BLOCKSIZE) {
                Block* indirect = read_block(temp->indirect);
                int* array = (int*)indirect->datum;
                
                freeBlocks[temp->indirect] = 1;
                for (; j <  (temp->size / BLOCKSIZE) + 1; ++j) {
                    int idx = j - NUM_DIRECT;
                    freeBlocks[array[idx]] = 1;
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    (void) argc;
    // initialization
    init();
    TracePrintf( 1, "[SERVER][TRC] Init succeed\n");

    if (Register(FILE_SERVER) != 0) return ERROR;
    TracePrintf( 1, "[SERVER][TRC] Register succeed\n");

    // It should then Fork and Exec a first client user process.
    if(Fork() == 0) {
		Exec(argv[1], argv + 1);
	}

    // receiving messages
    while(1) {
        TracePrintf( 1, "\n[SERVER][LOG] Start receiving message\n");
        Messgae msg;
        assert(sizeof(msg) == 32);

        int pid = Receive(&msg);
        if (pid == ERROR) {
            TracePrintf( 1, "[SERVER][ERR] Fail to receive msg\n");
            return ERROR;
        }

        if (pid == 0) {
            TracePrintf( 1, "[SERVER][LOG] Halt at pid 0 to prevent deadlock\n");
            Halt();
        }

        if (msgHandler(&msg, pid) == ERROR) return ERROR;

        Reply((void*)&msg, pid);
    }

    return 0;
}
