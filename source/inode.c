#include "inode.h"
#include "disk.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <comp421/yalnix.h>
#include <stdio.h>

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

void createInode(struct inode* inode, int type) {
    inode->type = type;
    inode->nlink = 0;

    if (type == INODE_DIRECTORY) {
        // for . and ..
        inode->size = 2 * sizeof(struct dir_entry);
    } else {
        inode->size = 0;
    }

    inode->indirect = getFreeBlock();
}

void addInodeEntry(struct inode* inode, int inum, char* name) {
    struct dir_entry* entry = (struct dir_entry*)malloc(sizeof(struct dir_entry));
    entry->inum = inum;
    strcpy(entry->name, name);

    int block_num = inode->size / BLOCKSIZE;
    int block_offset = inode->size % BLOCKSIZE;

    if (block_num < NUM_DIRECT) {
        struct Block* blk = read_block(inode->direct[block_num]);
        memcpy(blk->datum + block_offset, entry, sizeof(struct dir_entry));
        WriteSector(inode->direct[block_num], (void *) blk->datum);

        free(blk);
    } else {
        int indirect_block_num = block_num - NUM_DIRECT;
        struct Block* indirectBlk = read_block(inode->indirect);
        
        int* indirect = (int*)indirectBlk->datum;
        struct Block* blk = read_block(indirect[indirect_block_num]);

        memcpy(blk->datum + block_offset, entry, sizeof(struct dir_entry));
        WriteSector(indirect[indirect_block_num], (void *) blk->datum);

        free(indirectBlk);
        free(blk);
    }

    inode->size += sizeof(struct dir_entry);
}

/* find the inum of last dir (before the last slash) */
int findInum(char* pathname, int curr_inum){
    TracePrintf( 1, "[SERVER][LOG] Find Inum for %s\n", pathname);

    // pathname should be valid (checked before calling the function)
    char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
    memcpy(pName, pathname, strlen(pathname));

    int inum = curr_inum;

    // null-terminated dir_name
    char dir_name[DIRNAMELEN + 1];
    char* end = pName; // Start scanning from the beginning of pName
    char* start = end; // Initialize start at the beginning of pName

    if (pName[0] == '/') {
        inum = ROOTINODE; // Start from the root directory for absolute paths
    }

    // Continue looping until the end of the string is reached
    while (*end != '\0') {
        // Advance 'end' to the next '/' or to the end of the string
        while (*end != '/' && *end != '\0') {
            end++;
        }

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
            }
            else if (temp_inum == ERROR) {
                TracePrintf( 1, "[SERVER][ERR] Fail to find Inum of a subdirectory\n");
                return ERROR;
            }

            TracePrintf( 1, "[SERVER][TRC] Found Directory: %s\n", dir_name);
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

    TracePrintf( 1, "[SERVER][LOG] Find Inum %d for %s\n", inum, pathname);
    return inum;
}

/* 
 * retrieve the directory inum 
 * return ERROR if not found
 * Remember to add indirect check...
 */
int retrieveDir(int inum, char* dirname) {
    TracePrintf( 1, "[SERVER][LOG] Retrieving Directory %s Inum\n", dirname);

    struct inode* inode = findInode(inum);
    if (inode->type != INODE_DIRECTORY) {
        TracePrintf( 1, "[SERVER][ERR] Not a directory Inode\n");
        return ERROR;
    }

    int i, dir_size = inode->size / sizeof(struct dir_entry);    
    for (i = 0; i < dir_size; ++i) {
        int idx = (i * sizeof(struct dir_entry)) / BLOCKSIZE;

        int bNum;
        struct Block* indirectBlk;
        // direct entry
        if (idx < NUM_DIRECT) {
            bNum = inode->direct[idx];
        } else {
            bNum = inode->indirect;

            indirectBlk = read_block(bNum);
            int* array = (int*)indirectBlk->datum;
            
            bNum = array[idx - NUM_DIRECT];
        }

        // indirect entry to be implemented
        struct Block* blk = read_block(bNum);
        struct dir_entry* dir = &((struct dir_entry*)blk->datum)[i % (BLOCKSIZE / sizeof(struct dir_entry))];
        if (dir != NULL && strncmp(dirname, dir->name, DIRNAMELEN) == 0) {
            // found a match
            if (idx >= NUM_DIRECT) free(indirectBlk);

            free(blk);
            
            TracePrintf( 1, "[SERVER][LOG] Retrieving Directory %s inum: %d\n", dirname, (int)dir->inum);
            return (int)dir->inum;
        }

        if (idx >= NUM_DIRECT) free(indirectBlk);

        free(blk);
    }

    return ERROR;
}