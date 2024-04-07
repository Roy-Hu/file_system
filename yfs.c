#include "yfs.h"
#include "cache.h"
#include <string.h>
#include <stddef.h>

/*
 * Send msg to the server
 * Input: pointer to message
 *
 */
// void SendMsg(struct *message, int pid) {
//     return void;
// }
/*
 * normalize the file name/dir name
 * e.g.: "/aa///a/./aa/" --> /aa/a/aa/
 *
 */
int checkNnormalizePathname(char* pathname) {
    if (pathname == NULL) return ERROR;
    int readPos = 0, writePos = 0;
    int len = strlen(pathname);
    char* temp =  (char *)malloc(MAXPATHLEN * sizeof(char));
    memcpy(temp, pathname, strlen(pathname));
    // Assuming the path doesn't start with unnecessary "./"
    while (readPos < len && readPos < MAXPATHLEN) {
        // Skip multiple slashes
        if (pathname[readPos] == '/' && pathname[readPos + 1] == '/') {
            readPos++;
            continue;
        }
        // Skip "./"
        if (pathname[readPos] == '.' && (pathname[readPos + 1] == '/' || pathname[readPos + 1] == '\0')) {
            if (pathname[readPos + 1] != '\0') {
                readPos += 2; // Skip both characters
            } else {
                readPos++; // Only skip the dot if it's the last character
            }
            continue;
        }
        // Copy the current character to its new position
        pathname[writePos++] = temp[readPos++];
    }
    // Null-terminate the cleaned path
    pathname[writePos] = '\0';
    free(temp);
    return 0;
}

/* find the inum of last dir (before the last slash) */
int findInum(char* pathname, int curr_inum){
    TracePrintf(1, "Trying to find directory's inode number...\n");
    // pathname should be valid (checked before calling the function)
    char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
    memcpy(pName, pathname, strlen(pathname));
    int inum = curr_inum;
    // null-terminated dir_name
    char dir_name[DIRNAMELEN + 1];
    char* end = pName; // Start scanning from the beginning of pName
    if (pName[0] == '/') {
        inum = ROOTINODE; // Start from the root directory for absolute paths
    }
    char* start = end; // Initialize start at the beginning of pName

    // Continue looping until the end of the string is reached
    while (*end != '\0') {
        // Advance 'end' to the next '/' or to the end of the string
        while (*end != '/' && *end != '\0') {
            end++;
        }

        // Check if this is potentially the last segment and it doesn't end with a slash
        // and if so, exit the loop to disregard it
        if (*end == '\0' && pName[strlen(pName) - 1] != '/') {
            break;
        }

        // Copy the directory name into dir_name if we've found a valid segment
        if (start != end) { // Ensure we're not looking at consecutive slashes or a slash at the start
            // Calculate the length of this path component
            ptrdiff_t len = end - start;
            if (len > DIRNAMELEN) len = DIRNAMELEN; // Truncate if it's too long

            strncpy(dir_name, start, len); // Copy the segment into dir_name
            dir_name[len] = '\0'; // Null-terminate the directory name

            // Process the directory name stored in dir_name here
            int temp_inum = retrieveDir(curr_inum, dir_name);
            if (temp_inum == 0) {
                TracePrintf(1, "Found invalid Inum\n");
                return ERROR;
            }

        TracePrintf(1, "Found Directory: %s\n", dir_name);
        // update the curr inum
        inum = temp_inum;
        }

        // If we're at the end of the string, break out of the loop
        if (*end == '\0') {
            break;
        }

        // Otherwise, skip over the '/' and prepare for the next segment
        end++;
        start = end;
    }
    return inum;
}

/* 
 * retrieve the directory inum 
 * return 0 if not found
 * Remember to add indirect check...
 */
int retrieveDir(int inum, char* dirname) {
    TracePrintf(1, "Retrieving Directory Inum...\n");
    struct inode* Inode = findInode(inum);
    if (Inode->type != INODE_DIRECTORY) {
        TracePrintf(1, "Error! Not a directory inode\n");
        return ERROR;
    }
    int dir_size = Inode->size / sizeof(struct dir_entry);
    int i = 0;
    while (i < dir_size) {
        // get the
        int idx = (i*sizeof(struct dir_entry))/BLOCKSIZE;
        // direct entry
        if (idx < NUM_DIRECT) {
            int bNum = Indoe->direct[idx];
        }
        // indirect entry
        struct Block* blk = read_block(bNum);
        struct dir_entry dir = ((struct dir_entry*)blk->datum)[i % (BLOCKSIZE/sizeof(struct dir_entry))];
        if (dir != 0) {
            // found a match
            if (!strncmp(dirname, dir->name, DIRNAMELEN)) {
                return (int)dir->inum;
            }
            else {
                i++;
            }
        }

    }
    return 0;
}


void create_file(struct message* msg, int pid) {
    TracePrintf(1, "Creating a file now...\n");
    char pName = (char *)malloc(MAXPATHLEN * sizeof(char));
    // copy the path name from the calling process
    int res = CopyFrom(pid, pName, msg->path_oldName, MAXPATHNAMELEN);
    if (res == ERROR || checkNnormalizePathname(pName) == ERROR) return ERROR;
    int curr_inode = msg->data;
    
    // found the inum of the last dir (before the last slash)
    int inum = findInum(pName, curr_inum);
    // TO-DO:
    // 1. check the file name is created or not
    // 2. if not, check parent dir for inodes if there is any space
    //    if there is space, allocate a new inode and 
}

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
