#include "inode.h"
#include "cache.h"
#include "helper.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <comp421/yalnix.h>
#include <stdio.h>

/* 
 * Find free inodes in the inode list
 * Return inum if found
 * Return ERROR if not found
 */
int getFreeInode() {
    int i;
    for (i = 0; i < INODE_NUM; ++i) {
        if (freeInodes[i] == 0) {
            freeInodes[i] = 1;
            return i;
        }
    }

    TracePrintf( 1, "[SERVER][ERR] No free inode available\n");

    return ERROR;
}

/*
 * Create a new inode for directory/file
 * 
 */
void createInode(int inum, int parent_inum, int type) {
    struct inode* inode = findInode(inum);

    inode->type = type;
    inode->nlink = 0;

    if (type == INODE_DIRECTORY) {
        TracePrintf( 1, "[SERVER][LOG] Create Directory Inode\n");
        // for . and ..
        inode->direct[0] = getFreeBlock();
        if (inode->direct[0] == 0) {
            TracePrintf( 1, "[SERVER][ERR] No free block available\n");
            return;
        }

        addInodeEntry(inum, inum, ".");
        addInodeEntry(inum, parent_inum, "..");
    } else {
        TracePrintf( 1, "[SERVER][LOG] Create Regular Inode\n");

        inode->size = 0;
    }

    inode->indirect = getFreeBlock();

    writeInode(inum, inode);
}

/* set the newly created directory/file name */
void setdirName(struct dir_entry* entry, char* filename) {
    memset(entry->name, '\0', DIRNAMELEN);
    int len = strlen(filename);
    if(len > DIRNAMELEN){
        memcpy(entry->name, filename, DIRNAMELEN);
    }
    else {
        memcpy(entry->name, filename, len);
    }
}
/*
 * Add a directory entry given the inum and name
 *
 * 
 */

int inodeWrite(int inum, void* buf, int curpos, int size) {
    struct inode* inode = findInode(inum);
    if (inode == NULL) {
        TracePrintf( 1, "[SERVER][ERR] Cannot find inode %d\n", inum);
        return ERROR;
    }

    if (curpos == END_POS) curpos = inode->size;

    int totalWrite = 0;
    while (size > 0) {
        int block_num = curpos / BLOCKSIZE;
        int block_offset = curpos % BLOCKSIZE;
        TracePrintf( 1, "[SERVER][TRC] curpos %d, Block num %d, Block offset %d\n", curpos, block_num, block_offset);

        int write_size = BLOCKSIZE - block_offset;
        if (write_size > size) write_size = size;
        totalWrite += size;

        TracePrintf( 1, "Write %d to block %d\n", write_size, block_num);

        if (block_num < NUM_DIRECT) {
            Block* blk = read_block(inode->direct[block_num]);

            memcpy(blk->datum + block_offset, buf, write_size);
            WriteSector(inode->direct[block_num], (void *) blk->datum);

            free(blk);
        } else {
            int indirect_block_num = block_num - NUM_DIRECT;
            Block* indirectBlk = read_block(inode->indirect);
            
            int* indirect = (int*)indirectBlk->datum;
            Block* blk = read_block(indirect[indirect_block_num]);

            memcpy(blk->datum + block_offset, buf, write_size);
            WriteSector(indirect[indirect_block_num], (void *) blk->datum);

            free(indirectBlk);
            free(blk);
        }

        curpos += write_size;
        buf += write_size;
        size -= write_size;

        if (size > 0) {
            printf("[SERVER][WAR] Buf data write through different blocks, be careful\n");
        }
    }

    if (curpos > inode->size) inode->size = curpos;

    writeInode(inum, inode);

    return totalWrite;
}

void addInodeEntry(int parent_inum, int file_inum, char* name) {
    struct dir_entry* entry = (struct dir_entry*)malloc(sizeof(struct dir_entry));
    entry->inum = file_inum;
    TracePrintf(1, "set entry with name %s and inum %d\n",name, parent_inum);
    // cannot use strcpy here since name is not necessarily null terminated
    setdirName(entry, name);

    inodeWrite(parent_inum, (void*)entry, END_POS, sizeof(struct dir_entry));
}

/* find the inum of last dir (before the last slash) */
int findInum(char* pathname, int curr_inum){
    TracePrintf( 1, "[SERVER][LOG] Finding Inum for %s\n", pathname);

    // pathname should be valid (checked before calling the function)
    char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
    memcpy(pName, pathname, strlen(pathname));

    int inum = curr_inum;

    // null-terminated dir_name
    char dir_name[DIRNAMELEN + 1];
    char* end = pName; // Start scanning from the beginning of pName
    char* start = end; // Initialize start at the beginning of pName

    // Start from the root directory for absolute paths
    if (pName[0] == '/') inum = ROOTINODE; 

    // Continue looping until the end of the string is reached
    while (*end != '\0') {
        // Advance 'end' to the next '/' or to the end of the string
        while (*end != '/' && *end != '\0') end++;

        // Check if this is potentially the last segment and it doesn't end with a slash
        // and if so, exit the loop to disregard it
        if (*end == '\0' && pName[strlen(pName) - 1] != '/') {
            TracePrintf( 1, "[SERVER][TRC] Found last segment\n");
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
                TracePrintf( 1, "[SERVER][ERR] Found invalid Inum\n");
                return ERROR;
            } else if (temp_inum == ERROR) {
                TracePrintf( 1, "[SERVER][ERR] Fail to find Inum of a subdirectory\n");
                return ERROR;
            }

            TracePrintf( 1, "[SERVER][TRC] Found Directory: %s\n", dir_name);
            // update the curr inum
            inum = temp_inum;
        }

        // If we're at the end of the string, break out of the loop
        if (*end == '\0') break;

        // Otherwise, skip over the '/' and prepare for the next segment
        start = ++end;
    }

    TracePrintf( 1, "[SERVER][LOG] Find Inum %d for %s\n", inum, pathname);
    return inum;
}

/* 
 * retrieve the directory inum give the dir/filename
 * return ERROR if not found
 * Remember to add indirect check...
 */
int retrieveDir(int inum, char* dirname) {
    TracePrintf( 1, "[SERVER][LOG] Retrieving Directory %s at parent Inum %d\n", dirname, inum);
    // find the inum of parent dir
    struct inode* inode = findInode(inum);
    if (inode->type != INODE_DIRECTORY) {
        TracePrintf( 1, "[SERVER][ERR] %d Not a directory Inode\n", inum);
        return ERROR;
    }

    int i, dir_size = inode->size / sizeof(struct dir_entry);    
    TracePrintf( 1, "[SERVER][TRC] Directory Size %d, inode size %d\n", dir_size, inode->size);
    for (i = 0; i < dir_size; ++i) {
        int idx = (i * sizeof(struct dir_entry)) / BLOCKSIZE;

        int bNum;
        Block* indirectBlk;
        // direct entry
        if (idx < NUM_DIRECT) bNum = inode->direct[idx];
        else {
            bNum = inode->indirect;

            indirectBlk = read_block(bNum);
            int* array = (int*)indirectBlk->datum;
            
            bNum = array[idx - NUM_DIRECT];
        }

        // indirect entry to be implemented
        Block* blk = read_block(bNum);
        struct dir_entry* dir = &(((struct dir_entry*)blk->datum)[i % (BLOCKSIZE / sizeof(struct dir_entry))]);
            
        TracePrintf( 1, "[SERVER][TRC] Found Directory %s\n", dir->name);
        if (dir != NULL && strncmp(dirname, dir->name, DIRNAMELEN) == 0) {
            // found a match
            if (idx >= NUM_DIRECT) free(indirectBlk);

            free(blk);
            
            TracePrintf( 1, "[SERVER][LOG] Retrieving Directory %s inum: %d\n", dirname, (int)dir->inum);
            return (int)dir->inum;
        }
        
        if (dir == NULL) TracePrintf( 1, "[SERVER][LOG] retrivedir: Directory is NULL!\n");
       
        if (idx >= NUM_DIRECT) free(indirectBlk);

        free(blk);
    }
    TracePrintf( 1, "[SERVER][LOG] retrivedir: Directory/file:%s does not exist!\n", dirname);
    return ERROR;
}