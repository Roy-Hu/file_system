#include "yfs.h"
#include "cache.h"
#include "inode.h"
#include "disk.h"
#include "helper.h"
#include "call.h"
#include "log.h"

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
            TracePrintf( LOG, "[SERVER][LOG] Received Open request!\n");
            char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
            if (CopyFrom(pid, (void*)pName, msg->pathnamePtr, MAXPATHNAMELEN) == ERROR) {
                printf( "[SERVER][ERR] Open: Fail to copy path name %s\n", pName);
            }

            int parent_inum;
            msg->inum = yfsOpen(msg->inum, pName, &parent_inum);
            if (msg->inum == ERROR) {
                msg->reply = ERROR;
                printf( "[SERVER][ERR] Open: Fail to open file\n");
                break;
            }

            struct inode* inode = findInode(msg->inum);
            msg->pos = inode->reuse;

            break;
        }
        case CREATE: {
            TracePrintf( LOG, "[SERVER][LOG] Received Create request!\n");

            char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
            if (CopyFrom(pid, (void*)pName, msg->pathnamePtr, MAXPATHNAMELEN) == ERROR) {
                printf( "[SERVER][ERR] Create: Fail copy path name %s\n", pName);
            }

            msg->inum = yfsCreate(pName, msg->inum);
            if (msg->inum == ERROR) {
                msg->reply = ERROR;
                printf( "[SERVER][ERR] Create: Fail to create file\n");
                break;
            }

            struct inode* inode = findInode(msg->inum);
            msg->pos = inode->reuse;
            
            break;
        }
        case READ: {
            TracePrintf( LOG, "[SERVER][LOG] Received Read request!\n");

            char* buf = (char *)malloc(msg->size * sizeof(char));


            int size = yfsRead(msg->inum, buf, msg->pos, msg->size, msg->reply);
            
            if (size == ERROR) {
                msg->reply = ERROR;
                printf( "[SERVER][ERR] Read: Fail to read file\n");
                break;
            }

            if (size < msg->size) buf[size] = '\0';
            
            if (CopyTo(pid, msg->bufPtr, (void*)buf, msg->size) == ERROR) {
                printf( "[SERVER][ERR] Create: Fail copy to buf\n");
            }

            msg->size = size;

            break;
        }
        case WRITE: {
            TracePrintf( LOG, "[SERVER][LOG] Received Write request!\n");

            char* buf = (char *)malloc(msg->size * sizeof(char));
            if (CopyFrom(pid, (void*)buf, msg->bufPtr, msg->size) == ERROR) {
                printf( "[SERVER][ERR] Create: Fail copy from buf\n");
            }

            msg->size = yfsWrite(msg->inum, (void*)buf, msg->pos, msg->size, msg->reply);
            if (msg->size == ERROR) {
                msg->reply = ERROR;
                printf( "[SERVER][ERR] Read: Fail to read file\n");
                break;
            }

            TracePrintf( LOG, "[SERVER][LOG] Write %d byte tp msg\n", msg->size);

            break;
        }
        case SEEK: {
            TracePrintf( LOG, "[SERVER][LOG] Received Seek request!\n");
            msg->reply = yfsSeek(msg->inum);
            if (msg->reply == ERROR) {
                printf( "[SERVER][ERR] Seek: Fail to return inum of the file\n");
                break;
            }
            break;
        }
        case LINK: {
            TracePrintf( LOG, "[SERVER][LOG] Received Link request!\n");
            char* oldname = (char *)malloc(MAXPATHLEN * sizeof(char));
            if (CopyFrom(pid, (void*)oldname, msg->pathnamePtr, MAXPATHNAMELEN) == ERROR) {
                printf( "[SERVER][ERR] Link: Fail copy old name %s\n", oldname);
            }

            char* newname = (char *)malloc(MAXPATHLEN * sizeof(char));
            if (CopyFrom(pid, (void*)newname, msg->bufPtr, MAXPATHNAMELEN) == ERROR) {
                printf( "[SERVER][ERR] Link: Fail copy new name %s\n", newname);
            }

            // pos will be the cur dir inum while LINK
            msg->reply = yfsLink(oldname, newname, msg->inum);
            if (msg->inum == ERROR) {
                msg->reply = ERROR;
                printf( "[SERVER][ERR] Link: Fail to link %s to %s\n", newname, oldname);
                break;
            }

            break;
        }
        case UNLINK: {
            TracePrintf( LOG, "[SERVER][LOG] Received Unlink request!\n");

            char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
            if (CopyFrom(pid, (void*)pName, msg->pathnamePtr, MAXPATHNAMELEN) == ERROR) {
                printf( "[SERVER][ERR] Create: Fail copy path name %s\n", pName);
            }

            msg->reply = yfsUnLink(pName, msg->inum);
            if (msg->inum == ERROR) {
                msg->reply = ERROR;
                printf( "[SERVER][ERR] UnLink: Fail to unlink %s\n", pName);
                break;
            }

            printdirentry(msg->inum);

            break;
        }
        case MKDIR: {
            TracePrintf( LOG, "[SERVER][LOG] Received Mkdir request!\n");

            char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
            if (CopyFrom(pid, (void*)pName, msg->pathnamePtr, MAXPATHNAMELEN) == ERROR) {
                printf( "[SERVER][ERR] Mkdir: Fail copy path name %s\n", pName);
            }

            msg->inum = yfsMkdir(pName, msg->inum);
            if (msg->inum == ERROR) {
                msg->reply = ERROR;
                printf( "[SERVER][ERR] Create: Fail to create dir\n");
                break;
            }

            break;
        }
        case RMDIR: {
            TracePrintf( LOG, "[SERVER][LOG] Received RmDir request!\n");
            char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
            if (CopyFrom(pid, (void*)pName, msg->pathnamePtr, MAXPATHNAMELEN) == ERROR) {
                printf( "[SERVER][ERR] Mkdir: Fail copy path name %s\n", pName);
            }
            msg->inum = yfsRmDir(pName, msg->inum);
            if (msg->inum == ERROR) {
                msg->reply = ERROR;
                printf( "[SERVER][ERR] RmDir: Fail to remove dir\n");
                break;
            }

            break;
        }
        case CHDIR: {
            TracePrintf( LOG, "[SERVER][LOG] Received ChDir request!\n");
            char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
            if (CopyFrom(pid, (void*)pName, msg->pathnamePtr, MAXPATHNAMELEN) == ERROR) {
                printf( "[SERVER][ERR] Chdir: Fail copy path name %s\n", pName);
            }
            // pathname currInode
            msg->inum = yfsChDir(pName, msg->inum);
            if (msg->inum == ERROR) {
                msg->reply = ERROR;
                printf( "[SERVER][ERR] ChDir: Fail to remove dir\n");
                break;
            }
            break;
        }
        case STAT: {
            TracePrintf( LOG, "[SERVER][LOG] Received Stat request!\n");
            char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
            if (CopyFrom(pid, (void*)pName, msg->pathnamePtr, MAXPATHNAMELEN) == ERROR) {
                printf( "[SERVER][ERR] Stat: Fail copy path name %s\n", pName);
            }

            struct Stat* stat = (struct Stat*)malloc(sizeof(struct Stat));
            msg->reply = yfsStat(pName, msg->inum, stat);
            if (msg->reply == ERROR) {
                printf( "[SERVER][ERR] Stat: Fail to stat file\n");
                break;
            }

            if (CopyTo(pid, msg->bufPtr, (void*)stat, sizeof(struct Stat)) == ERROR) {
                printf( "[SERVER][ERR] Stat: Fail to copy stat\n");
            }

            free(stat);

            break;
        }
        case SYNC: {
            TracePrintf( LOG, "[SERVER][LOG] Received Sync request!\n");

            yfsSync();

            msg->reply = 0;

            break;
        }
        case SHUTDOWN: {
            TracePrintf( LOG, "[SERVER][LOG] Received Shutdown request!\n");

            yfsShutdown();

            msg->reply = 0;

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
    TracePrintf( LOG, "[SERVER][LOG] Start YFS Initialization\n");
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
        temp->reuse = 0;

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
        
        writeInode(i, temp);
    }
}

int main(int argc, char** argv) {
    (void) argc;
    // initialization
    init();
    TracePrintf( TRC, "[SERVER][TRC] Init succeed\n");

    if (Register(FILE_SERVER) != 0) return ERROR;
    TracePrintf( TRC, "[SERVER][TRC] Register succeed\n");

    // It should then Fork and Exec a first client user process.
    int fork_pid = Fork();
    if(fork_pid == 0) {
		Exec(argv[1], argv + 1);
	}

    // receiving messages
    while(1) {
        TracePrintf( LOG, "[SERVER][LOG] Start receiving message\n\n");
        Messgae msg;
        assert(sizeof(msg) == 32);

        int pid = Receive(&msg);
        if (pid == ERROR) {
            printf( "[SERVER][ERR] Fail to receive msg\n");
            return ERROR;
        }

        if (pid == 0) {
            TracePrintf( LOG, "[SERVER][LOG] Halt at pid 0 to prevent deadlock\n");
            Halt();
        }

        if (msgHandler(&msg, pid) == ERROR) return ERROR;

        Reply((void*)&msg, pid);

        if (msg.type == SHUTDOWN) {
            Exit(0);
        }
    }

    Exit(fork_pid);

    return 0;
}
