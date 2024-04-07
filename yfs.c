#include "yfs.h"
#include "cache.h"
#include "inode.h"
#include "disk.h"
#include "helper.h"

#include <comp421/yalnix.h>
#include <comp421/iolib.h>
#include <stdlib.h>
#include <string.h>


void create_file(struct message* msg, int pid) {
    TracePrintf(1, "Creating a file now...\n");
    char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
    // copy the path name from the calling process given pid
    int res = CopyFrom(pid, (void*)pName, msg->path_oldName, MAXPATHNAMELEN);
    if (res == ERROR || checkNnormalizePathname(pName) == ERROR) {
         TracePrintf(1, "Error creating file, cannot normalize filename\n");
    }
    // get the current node from client's iolib
    int curr_inode = msg->data;
    
    // found the inum of the last dir (before the last slash)
    int inum = findInum(pName, curr_inode);
    if (inum == ERROR) {
        TracePrintf(1, "ERROR when trying to find parent dir, possibly due to an non-existing dir!\n");
        msg->data = ERROR;
        return;
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
    if (filename[0]=='\0') {
        TracePrintf(1, "Empty file name!\n");
        msg->data = ERROR;
        return;
    }
    int file_inum = retrieveDir(inum, filename);
    // create new file
    if (file_inum == 0) {
        int new_inum = touch(file_inum, filename);
        if (new_inum == ERROR) {
            TracePrintf(1, "Error Occurred when creating file\n");
            // reply error
        }
        TracePrintf(1, "Successfully created file with inum %d\n", new_inum);
        // adding to the opened_file table
        msg->data = new_inum;
    }
    // already existed
    // more operations need to be added
    else {
        msg->data = file_inum;
    }
    
}

/* create a file during the */
int touch(int inum, char* filename) {
    (void) inum;
    (void) filename;
    TracePrintf(1, "Touch a new file!\n");
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
        TracePrintf(1, "No availiable inode!\n");
        return ERROR;
    }
    // create a new entry for the file
    struct inode* inode_entry = findInode(new_inum);
    inode_entry->size=0;
    inode_entry->type=INODE_REGULAR;
    inode_entry->reuse++;
    inode_entry->nlink = 0;

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

int msgHandler(struct message* msg, int pid) {
    OperationType myType = msg->type;
    int res;
    switch(myType) {
        case OPEN: {
            TracePrintf(1, "Received an open file request!\n");
            break;
        }
        case CLOSE: {
            break;
        }
        case CREATE: {
            TracePrintf(1, "Received an create file request!\n");
            create_file(msg, pid);
            res = msg->data;
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
    return res;
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
    freeInodes = (bool*)malloc(sizeof(bool)*(INODE_NUM+1));
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
        msg.data = (short)opt;
        Reply((void*)&msg, pid);
    }
    return 0;
}
