#include "yfs.h"
#include "cache.h"
#include <string.h>

/*
 * Send msg to the server
 * Input: pointer to message
 *
 */
// void SendMsg(struct *message, int pid) {
//     return void;
// }

int msgHandler(struct message* msg, int pid) {
    OperationType myType = msg->type;
    switch(myType) {
        case OPEN: {
            TracePrintf(1, "Received an open file request!\n");
            break;
        }
        case CLOSE: {
            break;
        }
        case CREATE: {
            break;
        }
        case READ: {
            break;
        }
        case WRITE: {
            break;
        }
        case SEEK: {
            break;
        }
        case LINK: {
            break;
        }
        case UNLINK: {
            break;
        }
        case MKDIR: {
            break;
        }
        case RMDIR: {
            break;
        }
        case CHDIR: {
            break;
        }
        case STAT: {
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
    return pid;
}

/**
 * initialization function for yfs 
 * 1. initilize freeBlocks, freeInodes
 * 2. check and mark occupied blocks that contains
 *    all of the inodes
 * 3. 
 *  
**/
void init() {
    TracePrintf(1, "Start YFS Initialization\n");
    // read the first block to get the header
    struct Block* block = read_block(1);
    struct fs_header* header = (struct fs_header*)malloc(sizeof(struct fs_header));
    memcpy(header, block->datum, sizeof(struct fs_header));
    INODE_NUM = header->num_inodes;
    BLOCK_NUM = header->num_blocks;
    freeBlocks = (bool*)malloc(sizeof(bool)*BLOCK_NUM);
    freeInodes = (bool*)malloc(sizeof(bool)*INODE_NUM);
    freeInodes[0] = 1;
    // root inode
    freeInodes[ROOTINODE] = 1;
    freeBlocks[0] = 1;
    freeBlocks[1] = 1;
    int i = 0;
    // init inode list as free
    for (i = 2; i < INODE_NUM + 1; ++i) {
        freeInodes[i] = 0;
    }
    // init block list as free,skip 0 and 1
    for (i = 2; i < BLOCK_NUM; ++i) {
        freeBlocks[i] = 0;
    } 
    // set occupied blocks as 1 for inodes
    for (i = 2; i < ((INODE_NUM+1)*INODESIZE)/BLOCKSIZE + 1; ++i) {
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
                if ((j+1)*BLOCKSIZE > temp->size && !(j*BLOCKSIZE < temp->size))
                    break;
                freeBlocks[temp->direct[j]] = 1;
            }
            // check indirect block
            if (temp->size > NUM_DIRECT*BLOCKSIZE) {
                struct Block* Indirect = read_block(temp->indirect);
                int* array = (int*)Indirect->datum;
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
    TracePrintf(1, "Init succeed\n");
    if (Register(FILE_SERVER) != 0) return ERROR;
    TracePrintf(1, "Register succeed\n");
    // It should then Fork and Exec a first client user process.
    if(Fork() == 0) {
		Exec(argv[1], argv + 1);
	}

    // receiving messages
    while(1) {
        TracePrintf(1, "Begin receiving message\n");
        struct message msg;
        int pid = Receive((void*)&msg);
        if (pid == ERROR) {
            TracePrintf(1, "ERROR\n");
            return ERROR;
        }
        if (pid == 0) {
            TracePrintf(1, "Prevent Deadlock!\n");
            Halt();
        }
        int opt = msgHandler((struct message*)&msg, pid);
        if (opt == ERROR) return ERROR;
        msg.output = (short)opt;
        Reply((void*)&msg, pid);
    }
    return 0;
}
