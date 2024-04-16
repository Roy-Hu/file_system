#include "inode.h"
#include "cache.h"
#include "helper.h"
#include "log.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <comp421/yalnix.h>
#include <stdio.h>

void printInode(int inum) {
    struct inode* inode = findInode(inum);
    if (inode == NULL) {
        TracePrintf( ERR, "[SERVER][ERR] Cannot find inode %d\n", inum);
        return;
    }

    if (inode->type == INODE_REGULAR) TracePrintf( LOG, "[SERVER][LOG] Inode %d, Size %d, nlink %d, Regular File\n", inum, inode->nlink, inode->size);
    else if (inode->type == INODE_DIRECTORY)TracePrintf( LOG, "[SERVER][LOG] Inode %d, Size %d, nlink %d, Directory\n", inum,inode->nlink, inode->size);
}

void printdirentry(int inum) {
    TracePrintf( LOG, "[SERVER][INODE %d][LOG] Print Inode entry\n", inum);
    struct inode* inode = findInode(inum);
    //TracePrintf( LOG, "[SERVER][LOG] Print Inode entry: found dir %d\n", inum);
    if (inode == NULL) {
        TracePrintf( ERR, "[SERVER][ERR] Cannot find inode %d\n", inum);
        return;
    }

    struct dir_entry* entry = (struct dir_entry*)malloc(sizeof(struct dir_entry));
    int i;

    for (i = 0; i < inode->size; i += sizeof(struct dir_entry)) {
        int block_num = i / BLOCKSIZE;
        int block_offset = i % BLOCKSIZE;

        if (block_num < NUM_DIRECT) {
            Block* blk = read_block(inode->direct[block_num]);
            memcpy(entry, blk->datum + block_offset, sizeof(struct dir_entry));
        } else {
            int indirect_block_num = block_num - NUM_DIRECT;
            Block* indirectBlk = read_block(inode->indirect);
            
            int* indirect = (int*)indirectBlk->datum;
            Block* blk = read_block(indirect[indirect_block_num]);

            memcpy(entry, blk->datum + block_offset, sizeof(struct dir_entry));
        }

        if (entry->inum == 0) continue;
        
        struct inode* fileInode = findInode(entry->inum);
        if (fileInode == NULL) {
            TracePrintf( ERR, "[SERVER][ERR] Cannot find inode %d\n", entry->inum);
            return;
        }

        if (fileInode->type == INODE_REGULAR) TracePrintf( LOG, "[SERVER][INODE %d][LOG] Entry Name %s, Inum %d, Regular File\n", inum, entry->name, entry->inum);
        else TracePrintf( LOG, "[SERVER][INODE %d][LOG] Entry Name %s, Inum %d, Directory\n", inum, entry->name, entry->inum);
    }

    free(entry);
}
/*
 * Create a new inode for directory/file
 * 
 */
int inodeCreate(int inum, int parent_inum, int type) {
    TracePrintf( LOG, "[SERVER][LOG] Create Inode %d at parent inode %d\n", inum, parent_inum);

    struct inode* inode = findInode(inum);
    
    inode = findInode(inum);
    inode->type = type;
    inode->nlink = 0;
    
    inode->indirect = getFreeBlock();

    if (writeInode(inum, inode) == ERROR) {
        TracePrintf( ERR, "[SERVER][ERR] Cannot write inode %d\n", inum);
        return ERROR;
    }

    if (type == INODE_DIRECTORY) {
        TracePrintf( INF, "[SERVER][INF] Create Directory Inode\n");

        inodeAddEntry(inum, inum, ".");
        inodeAddEntry(inum, parent_inum, "..");
    } else {
        TracePrintf( INF, "[SERVER][INF] Create Regular Inode\n");

        inode->size = 0;
    }

    TracePrintf( INF, "[SERVER][INF] Inode %d created\n", inum);

    return 0;
}

int inodeDelete(int inum) {
    TracePrintf( LOG, "[SERVER][LOG] Deleting Inode %d\n", inum);

    struct inode* inode = findInode(inum);
    if (inode == NULL) {
        TracePrintf( ERR, "[SERVER][ERR] Cannot find inode %d\n", inum);
        return ERROR;
    }

    if (inode->nlink > 0) {
        TracePrintf( ERR, "[SERVER][ERR] Inode %d has %d links\n", inum, inode->nlink);
        return ERROR;
    }

    int i;
    for (i = 0; i < inode->size; i += BLOCKSIZE) {
        int block_num = i / BLOCKSIZE;

        if (block_num < NUM_DIRECT) {
            freeBlocks[inode->direct[block_num]] = 0;
        } else {
            int indirect_block_num = block_num - NUM_DIRECT;
            Block* indirectBlk = read_block(inode->indirect);
            
            int* indirect = (int*)indirectBlk->datum;
            freeBlocks[indirect[indirect_block_num]] = 0;
        }
    }

    inode->type = INODE_FREE;
    inode->size = 0;
    
    if (writeInode(inum, inode) == ERROR) {
        TracePrintf( ERR, "[SERVER][ERR] Cannot write inode %d\n", inum);
        return ERROR;
    }

    freeInodes[inum] = 0;

    TracePrintf( INF, "[SERVER][INF] InodeDel: successfully deleted inode: %d \n", inum);

    return 0;
}

int inodeDelEntry(int parentInum, int fileInum, char* eName) {
    TracePrintf( LOG, "[SERVER][LOG] Deleting Inode %d from Inode %d\n", fileInum, parentInum);

    if (decrementNlink(fileInum) == ERROR) {
        TracePrintf( ERR, "[SERVER][ERR] Cannot decrement nlink of inode %d\n", fileInum);
        return ERROR;
    }

    struct inode* parentInode = findInode(parentInum);
    if (parentInode == NULL) {
        TracePrintf( ERR, "[SERVER][ERR] Cannot find inode %d\n", parentInum);
        return ERROR;
    }

    struct inode* fileInode = findInode(fileInum);
    if (fileInode == NULL) {
        TracePrintf( ERR, "[SERVER][ERR] Cannot find inode %d\n", fileInum);
        return ERROR;
    }


    TracePrintf( INF, "[SERVER][INF] fileInum %d nlink %d\n", fileInum, fileInode->nlink);

    struct dir_entry* entry = (struct dir_entry*)malloc(sizeof(struct dir_entry));
    Block* blk = NULL;
    Block* indirectBlk = NULL;
    int* indirect = NULL;
    int indirect_block_num = 0;

    bool found = false;
    int i;
    for (i = 0; i < parentInode->size; i += sizeof(struct dir_entry)) {
        int block_num = i / BLOCKSIZE;
        int block_offset = i % BLOCKSIZE;

        if (block_num < NUM_DIRECT) {
            blk = read_block(parentInode->direct[block_num]);
            memcpy(entry, blk->datum + block_offset, sizeof(struct dir_entry));
        } else {
            indirect_block_num = block_num - NUM_DIRECT;
            indirectBlk = read_block(parentInode->indirect);
            
            indirect = (int*)indirectBlk->datum;
            blk = read_block(indirect[indirect_block_num]);

            memcpy(entry, blk->datum + block_offset, sizeof(struct dir_entry));
        }

        if (entry->inum == fileInum && i >= 64) {
            TracePrintf( TRC, "[SERVER][TRC] Found entry %s\n", entry->name);

            if (!cmpDirName(entry, eName)) {
                TracePrintf( ERR, "[SERVER][ERR] Inum %d Found entry %s does not match %s\n", fileInum, entry->name, eName);
                continue;
            }

            entry->inum = 0;
            
            memcpy(blk->datum + block_offset, entry, sizeof(struct dir_entry));

            if (block_num < NUM_DIRECT) {
                if (write_block(parentInode->direct[block_num], (void *) blk->datum) == ERROR) {
                    TracePrintf( ERR, "[SERVER][ERR] Cannot write block %d\n", parentInode->direct[block_num]);
                    return ERROR;
                }
            } else {
                if (write_block(indirect[indirect_block_num], (void *) blk->datum) == ERROR) {
                    TracePrintf( ERR, "[SERVER][ERR] Cannot write block %d\n", indirect[indirect_block_num]);
                    return ERROR;
                }
            }

            if (writeInode(parentInum, parentInode) == ERROR) {
                TracePrintf( ERR, "[SERVER][ERR] Cannot write inode %d\n", parentInum);
                return ERROR;
            }

            printdirentry(parentInum);
            found = true;

            break;
        }
        
        if (found) break;
    }

    free(entry);

    if (fileInode->nlink == 0) {
        if (inodeDelete(fileInum) == ERROR) {
            TracePrintf( ERR, "[SERVER][ERR] Cannot delete inode %d\n", fileInum);
            return ERROR;
        }
    }

    return 0;
}

int inodeReadWrite(int inum, void* buf, int curpos, int size, int type) {
    struct inode* inode = findInode(inum);
    if (inode == NULL) {
        TracePrintf( ERR, "[SERVER][ERR] Cannot find inode %d\n", inum);
        return ERROR;
    }

    if (inode->type == INODE_DIRECTORY && type == FILEWRITE) {
        TracePrintf( ERR, "[SERVER][ERR] Cannot read/write directory\n");
        return ERROR;
    }
    
    if (curpos == END_POS) curpos = inode->size;

    TracePrintf( TRC, "[SERVER][INODE %d][TRC] Read/Write %d bytes from/to inode %d's\n", inum, size, inum);

    int i;
    for (i = 0; i < curpos + size; i += BLOCKSIZE) {
        int block_num = i / BLOCKSIZE;

        if (block_num == inode->size / BLOCKSIZE) {
            if (inode->size % BLOCKSIZE == 0) {
                TracePrintf( TRC, "[SERVER][INODE %d][TRC] Allocate Block num %d\n", inum, block_num);
                inode->direct[block_num] = getFreeBlock();
            }
        } else if (block_num > inode->size / BLOCKSIZE) {
            if (block_num < NUM_DIRECT) {
                inode->direct[block_num] = getFreeBlock();
            } else {
                Block* indirectBlk = read_block(inode->indirect);
                int* indirect = (int*)indirectBlk->datum;

                if (indirect[block_num - NUM_DIRECT] == 0) {
                    indirect[block_num - NUM_DIRECT] = getFreeBlock();

                    if (write_block(inode->indirect, (void *) indirect) == ERROR) {
                        TracePrintf( ERR, "[SERVER][ERR] Cannot write block %d\n", indirect[block_num - NUM_DIRECT]);
                        return ERROR;
                    }
                }
            }

            TracePrintf( TRC, "[SERVER][INODE %d][TRC] Allocate Block num %d\n", inum, block_num);
        }
    }

    int totalbyte = 0;
    while (size > 0) {
        if (type == FILEREAD && curpos >= inode->size) break;

        int block_num = curpos / BLOCKSIZE;
        int block_offset = curpos % BLOCKSIZE;
        TracePrintf( TRC, "[SERVER][INODE %d][TRC] Read/Write at curpos %d, Block num %d, Block offset %d\n", inum, curpos, block_num, block_offset);

        int len = BLOCKSIZE - block_offset;
        if (len > size) len = size;

        if (block_num < NUM_DIRECT) {
            Block* blk = read_block(inode->direct[block_num]);

            if (type == FILEREAD) memcpy(buf, blk->datum + block_offset, len);
            else if (type == FILEWRITE || type == DIRUPDATE) {
                memcpy(blk->datum + block_offset, buf, len);

                if (write_block(inode->direct[block_num], (void *) blk->datum) == ERROR) {
                    TracePrintf( ERR, "[SERVER][ERR] Cannot write block %d\n", inode->direct[block_num]);
                    return ERROR;
                }
            } else {
                TracePrintf( ERR, "[SERVER][ERR] Invalid type\n");
                return ERROR;
            }
        } else {
            int indirect_block_num = block_num - NUM_DIRECT;
            Block* indirectBlk = read_block(inode->indirect);
            
            int* indirect = (int*)indirectBlk->datum;
            Block* blk = read_block(indirect[indirect_block_num]);

            if (type == FILEREAD) memcpy(buf, blk->datum + block_offset, len);
            else if (type == FILEWRITE || type == DIRUPDATE) {
                memcpy(blk->datum + block_offset, buf, len);

                if (write_block(indirect[indirect_block_num], (void *) blk->datum) == ERROR) {
                    TracePrintf( ERR, "[SERVER][ERR] Cannot write block %d\n", indirect[indirect_block_num]);
                    return ERROR;
                }
            } else {
                TracePrintf( ERR, "[SERVER][ERR] Invalid type\n");
                return ERROR;
            }
        }

        totalbyte += len;
        TracePrintf( TRC, "[SERVER][INODE %d][TRC] Write %d to block %d\n", inum, len, block_num);

        curpos += len;
        buf += len;
        size -= len;

        if (size > 0) {
            TracePrintf( WAR, "[SERVER][WAR] Buf data write through different blocks, be careful\n");
        }
    }

    if (curpos > inode->size) inode->size = curpos;

    if (writeInode(inum, inode) == ERROR) {
        TracePrintf( ERR, "[SERVER][ERR] Cannot write inode %d\n", inum);
        return ERROR;
    }

    return totalbyte;
}

int incrementNlink(int inum) {
    struct inode* inode = findInode(inum);
    if (inode == NULL) {
        TracePrintf( ERR, "[SERVER][ERR] Cannot find inode %d\n", inum);
        return ERROR;
    }
    
    inode->nlink++;

    if (writeInode(inum, inode) == ERROR) {
        TracePrintf( ERR, "[SERVER][ERR] Cannot write inode %d\n", inum);
        return ERROR;
    }

    return 0;
}

int decrementNlink(int inum) {
    struct inode* inode = findInode(inum);
    if (inode == NULL) {
        TracePrintf( ERR, "[SERVER][ERR] Cannot find inode %d\n", inum);
        return ERROR;
    }
    
    inode->nlink--;

    writeInode(inum, inode);

    return 0;
}

void inodeAddEntry(int parent_inum, int file_inum, char* name) {
    struct dir_entry* entry = (struct dir_entry*)malloc(sizeof(struct dir_entry));
    entry->inum = file_inum;
    // cannot use strcpy here since name is not necessarily null terminated
    setdirName(entry, name);

    TracePrintf( LOG, "[SERVER][INODE %d][LOG] Add %s to entry\n", parent_inum, name);

    inodeReadWrite(parent_inum, (void*)entry, END_POS, sizeof(struct dir_entry), DIRUPDATE);

    if (incrementNlink(file_inum) == ERROR) {
        TracePrintf( ERR, "[SERVER][ERR] Cannot increment nlink of inode %d\n", file_inum);
        return;
    }
}

/* 
 * find the inum of last dir (before the last slash)
 * inum is the currInode
 */
int inumFind(char* pathname, int currInum){
    TracePrintf( LOG, "[SERVER][LOG] Finding Inum for %s\n", pathname);

    // pathname should be valid (checked before calling the function)
    char* pName = (char *)malloc(MAXPATHLEN * sizeof(char));
    memcpy(pName, pathname, strlen(pathname));

    if (pName[0] == '/') currInum = ROOTINODE; 

    if (currInum <= 0 || currInum > INODE_NUM) {
        TracePrintf( ERR, "[SERVER][ERR] inumFind: Invalid Inum %d\n", currInum);
        return ERROR;
    }

    // null-terminated dir_name
    char dir_name[DIRNAMELEN + 1];
    char* end = pName; // Start scanning from the beginning of pName
    char* start = end; // Initialize start at the beginning of pName

    // Start from the root directory for absolute paths

    // Continue looping until the end of the string is reached
    while (*end != '\0') {
        // Advance 'end' to the next '/' or to the end of the string
        while (*end != '/' && *end != '\0') end++;

        // Check if this is potentially the last segment and it doesn't end with a slash
        // and if so, exit the loop to disregard it
        if (*end == '\0' && pName[strlen(pName) - 1] != '/') {
            TracePrintf( TRC, "[SERVER][TRC] Found last segment\n");
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
            int temp_inum = inumRetrieve(currInum, dir_name, INODE_DIRECTORY, 0);
            if (temp_inum == 0) {
                TracePrintf( ERR, "[SERVER][ERR] Found invalid Inum\n");
                return ERROR;
            } else if (temp_inum == ERROR) {
                TracePrintf( ERR, "[SERVER][ERR] Fail to find Inum of a subdirectory\n");
                return ERROR;
            }

            TracePrintf( TRC, "[SERVER][TRC] Found Directory: %s\n", dir_name);
            // update the curr inum
            currInum = temp_inum;
        }

        // If we're at the end of the string, break out of the loop
        if (*end == '\0') break;

        // Otherwise, skip over the '/' and prepare for the next segment
        start = ++end;
    }

    TracePrintf( INF, "[SERVER][INF] Find Inum %d for %s\n", currInum, pathname);
    return currInum;
}

/* 
 * inumRetrieve the directory inum give the dir/filename
 * return ERROR if not found
 * Remember to add indirect check...
 */
int inumRetrieve(int inum, char* name, int type, int open_flag) {
    TracePrintf( LOG, "[SERVER][LOG] Retrieving %s at parent Inum %d\n", name, inum);
    // find the inum of parent dir
    if (inum <= 0 || inum > INODE_NUM) {
        TracePrintf( ERR, "[SERVER][ERR] inumRetrieve: Invalid Inum %d\n", inum);
        return ERROR;
    }
    
    struct inode* inode = findInode(inum);
    if (open_flag == 0 && inode->type != INODE_DIRECTORY) {
        TracePrintf( ERR, "[SERVER][ERR] %d Not a directory Inode\n", inum);
        return ERROR;
    }

    int i, dir_size = inode->size / sizeof(struct dir_entry);    
    TracePrintf( TRC, "[SERVER][TRC] Directory Size %d, inode size %d\n", dir_size, inode->size);
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
        
        if (dir->inum == 0) continue;

        TracePrintf( TRC, "[SERVER][TRC] Found %s\n", dir->name);
        if (dir != NULL && strncmp(name, dir->name, DIRNAMELEN) == 0) {
            int dir_inum = (int)dir->inum;

            struct inode* found_inode = findInode(dir_inum);
            if (found_inode == NULL) {
                TracePrintf( ERR, "[SERVER][ERR] Cannot find Inode %d\n", dir_inum);
                return ERROR;
            }

            TracePrintf( TRC, "[SERVER][TRC] find Inode %d\n", dir_inum);

            if (open_flag == 0 && found_inode->type != type) {
                TracePrintf( ERR, "[SERVER][ERR] Found directory entry %s type and request type do not match\n", name);
                return ERROR;
            }


            return dir_inum;
        } else if (dir == NULL) TracePrintf( LOG, "[SERVER][LOG] Directory Entry is NULL!\n");
    }

    TracePrintf( INF, "[SERVER][INF] retrivedir: Directory/file:%s does not exist!\n", name);

    return ERROR;
}