#include <comp421/iolib.h>
#include <comp421/filesystem.h>
#include <comp421/yalnix.h>

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "yfs.h"
#include "log.h"

/* keep track of current innode */
int CURR_INODE = ROOTINODE;
/*
 * For an Open or Create
 * request, your file system library learns the file’s inode number from the file server process (the current
 * position within the file is initialized to 0) and stores this within the data structure in the library representing
 * that open file.
 */
typedef struct openedFile {
    int inum;
    int curPos;
    bool isValid; /* 1: for curr entry is valid, 0 otherwise */
} OpenedFile;

/* array of struct to keep track of opened files */
// openedFile* filesOpened[MAX_OPEN_FILES];

OpenedFile files[MAX_OPEN_FILES];

bool isInit = false;

void openFile(int fd, int inum, int pos) {
    files[fd].inum = inum;
    files[fd].curPos = pos;
    files[fd].isValid = true;
}

void updateFile(int fd, int pos) {
    files[fd].curPos = pos;
}

void closeFile(int fd) {
    files[fd].inum = 0;
    files[fd].isValid = false;
    files[fd].curPos = 0;
}

// check if file is opened given inum
bool opened(int inum) {
    int i;
    for (i = 0; i < MAX_OPEN_FILES; i++) {
        if (files[i].inum == inum) return true;
    }

    return false;
}

// find a new file descriptor, return -1 if not found
int findNewFd() {
    int i;
    for (i = 0; i < MAX_OPEN_FILES; i++) {
        if (!files[i].isValid) return i;
    }

    return -1;
}

// init the opened file table
void init() {
    int i;
    for (i = 0; i < MAX_OPEN_FILES; i++) {
        files[i].isValid = false;
    }

    isInit = true;
}

int Open(char *pathname) {
    if (!isInit) init();

    TracePrintf( LOG, "[CLIENT][LOG] Open Request for %s\n", pathname);

    Messgae* msg = (Messgae*)malloc(sizeof(Messgae));
    OperationType tp = OPEN;
    msg->type = (short) tp;
    msg->inum = CURR_INODE;
    msg->pathnamePtr = pathname;

    Send((void*)msg, -FILE_SERVER);

    short res = msg->reply;
    if (res != ERROR) {
        int fd = findNewFd();
        if (fd == -1) {
            printf( "[CLIENT][ERR] No more file can be opened\n");
            free(msg);
            return ERROR;
        }

        TracePrintf( LOG, "[CLIENT][LOG] Open file inum %d, fd %d: \n", msg->inum, fd);

        openFile(fd, msg->inum, 0);

        free(msg);

        return fd;
    } else {
        printf( "[CLIENT][ERR] Fail to open file\n");
        free(msg);

        return res;
    }
    free(msg);

    return res;
}

/* This request closes the open file specified by the file descriptor number fd . If fd is not the descriptor
number of a file currently open in this process, this request returns ERROR; otherwise, it returns 0. */
int Close(int fd) {
    TracePrintf( LOG, "[CLIENT][LOG] Close Request for fd: %d\n", fd);
    if (fd < 0 || fd > MAX_OPEN_FILES || files[fd].isValid == false || isInit == false) {
        printf( "[CLIENT][ERR] Close: Not a valid fd: %d number!\n", fd);
        return ERROR;
    }

    closeFile(fd);
    
    TracePrintf( LOG, "[CLIENT][LOG] Close: Closed file successfully with fd: %d number!\n", fd);
    return 0;
}

int Create(char *pathname) {
    if (!isInit) init();

    TracePrintf( LOG, "[CLIENT][LOG] Create Request for %s\n", pathname);

    Messgae* msg = (Messgae*)malloc(sizeof(Messgae));
    OperationType tp = CREATE;
    msg->type = (short) tp;
    msg->pathnamePtr = pathname;
    msg->inum = CURR_INODE;

    Send((void*)msg, -FILE_SERVER);
    short res = msg->reply;

    if (res != ERROR) {
        int fd = findNewFd();
        if (fd == -1) {
            printf( "[CLIENT][ERR] No more file can be opened\n");
            return ERROR;
        }

        TracePrintf( LOG, "[CLIENT][LOG] Created file inum %d, fd %d: \n", msg->inum, fd);

        openFile(fd, msg->inum, 0);

        free(msg);
        return fd;
    } else {
        printf( "[CLIENT][ERR] Fail to create file\n");

        free(msg);
        return res;
    }
}

int Read(int fd, void *buf, int size) {
    // init fd
    if (!isInit) init();
    if (fd < 0 || fd > MAX_OPEN_FILES || files[fd].isValid == false || isInit == false) {
        printf( "[CLIENT][ERR] Write: Not a valid fd: %d number!\n", fd);
        return ERROR;
    }

    int inum = files[fd].inum;
    int curpos = files[fd].curPos;

    Messgae* msg = (Messgae*)malloc(sizeof(Messgae));
    OperationType tp = READ;
    msg->type = (short) tp;
    msg->size = size;
    msg->bufPtr = buf;
    msg->pos = curpos;
    msg->inum = inum;

    Send((void*)msg, -FILE_SERVER);
    short res = msg->reply;

    if (res != ERROR) {
        int byte = msg->size;
        TracePrintf( LOG, "[CLIENT][LOG] Read %d bytes at fd %d: \n", byte, fd);
        updateFile(fd, files[fd].curPos + byte);
        free(msg);
        return byte;
    } else {
        printf( "[CLIENT][ERR] Fail to read file\n");

        free(msg);
        return res;
    }

    return 0;
}

int Write(int fd, void *buf, int size) {
    // init fd
    if (!isInit) init();
    if (fd < 0 || fd > MAX_OPEN_FILES || files[fd].isValid == false || isInit == false) {
        printf( "[CLIENT][ERR] Write: Not a valid fd: %d number!\n", fd);
        return ERROR;
    }

    int inum = files[fd].inum;
    int curpos = files[fd].curPos;

    Messgae* msg = (Messgae*)malloc(sizeof(Messgae));
    OperationType tp = WRITE;
    msg->type = (short) tp;
    msg->size = size;
    msg->bufPtr = buf;
    msg->pos = curpos;
    msg->inum = inum;

    Send((void*)msg, -FILE_SERVER);
    short res = msg->reply;

    if (res != ERROR) {
        int byte = msg->size;
        TracePrintf( LOG, "[CLIENT][LOG] Write %d bytes at fd %d: \n", byte, fd);
        updateFile(fd, files[fd].curPos + byte);
        free(msg);
        return byte;
    } else {
        printf( "[CLIENT][ERR] Fail to create file\n");

        free(msg);
        return res;
    }

    return 0;
}

int Seek(int fd, int offset, int whence) {
    if (fd < 0 || fd > MAX_OPEN_FILES || files[fd].isValid == false || isInit == false) {
        printf( "[CLIENT][ERR] Seek: Not a valid fd: %d number!\n", fd);
        return ERROR;
    }
    Messgae* msg = (Messgae*)malloc(sizeof(Messgae));
    OperationType tp = SEEK;
    msg->type = (short) tp;
    int seekPos = 0;
    switch(whence) {
        case SEEK_SET: {
            seekPos = offset;
            break;
        }
        case SEEK_CUR: {
            seekPos = files[fd].curPos + offset;
            break;
        }
        case SEEK_END: {
            msg->inum = files[fd].inum;
            Send((void*)msg, -FILE_SERVER);
            // should return the end of the file
            short res = msg->reply;
            if (res == ERROR) {
                printf( "[CLIENT][ERR] Seek: fail to find current fileszie\n");
                return ERROR;
            }
            seekPos = res + offset;
            break;
        }
        default: {
            printf( "[CLIENT][ERR] Seek: Not a valid whence\n");
            break;
        }
    }
    if (seekPos < 0) {
        printf( "[CLIENT][ERR] Seek: seekPos goes beyond the beginning of the file\n");
        free(msg);
        return ERROR;
    }

    updateFile(fd, seekPos);
    free(msg);

    return seekPos;
}

int Link(char *oldname, char *newname) {
        // init fd
    if (!isInit) init();

    Messgae* msg = (Messgae*)malloc(sizeof(Messgae));
    msg->type = LINK;
    msg->pathnamePtr = oldname;
    msg->bufPtr = newname;
    msg->inum = CURR_INODE;

    Send((void*)msg, -FILE_SERVER);
    if (msg->reply == ERROR) {
        free(msg);
        return ERROR;
    } 
    free(msg);

    return 0;
}

int Unlink(char *pathname) {
    if (!isInit) init();

    Messgae* msg = (Messgae*)malloc(sizeof(Messgae));
    msg->type = UNLINK;
    msg->pathnamePtr = pathname;
    msg->inum = CURR_INODE;

    Send((void*)msg, -FILE_SERVER);
    if (msg->reply == ERROR) {
        free(msg);
        return ERROR;
    } 
    free(msg);

    return 0;
}

// int SymLink(char *oldname, char *newname) {
//     return 0;
// }

// int ReadLink(char *pathname, char *buf, int len) {
//     return 0;
// }

int MkDir(char *pathname) {
    TracePrintf( LOG, "[CLIENT][LOG] MkDir Request for %s\n", pathname);

    Messgae* msg = (Messgae*)malloc(sizeof(Messgae));
    OperationType tp = MKDIR;
    msg->type = (short) tp;
    msg->inum = CURR_INODE;
    msg->pathnamePtr = pathname;

    Send((void*)msg, -FILE_SERVER);
    int res = (int)msg->reply;
    free(msg);
    TracePrintf( LOG, "[CLIENT][LOG] MkDir return %d: \n", res);
    if (res==ERROR)
        return ERROR;

    return 0;
}

int RmDir(char *pathname) {
    TracePrintf( LOG, "[CLIENT][LOG] RmDir Request for %s\n", pathname);

    Messgae* msg = (Messgae*)malloc(sizeof(Messgae));
    OperationType tp = RMDIR;
    msg->type = (short) tp;
    msg->inum = CURR_INODE;
    msg->pathnamePtr = pathname;

    Send((void*)msg, -FILE_SERVER);
    int res =  (int)msg->reply;
    free(msg);

    TracePrintf( LOG, "[CLIENT][LOG] RmDir file return %d: \n", res);
    
    return res;
}

int ChDir(char *pathname) {
    TracePrintf( LOG, "[CLIENT][LOG] CHDir Request for %s\n", pathname);

    Messgae* msg = (Messgae*)malloc(sizeof(Messgae));
    OperationType tp = CHDIR;
    msg->type = (short) tp;
    msg->inum = CURR_INODE;
    msg->pathnamePtr = pathname;

    Send((void*)msg, -FILE_SERVER);
    int res =  (int)msg->reply;
    
    if (res == ERROR) {
         printf( "[CLIENT][ERR] CHDir file errored %d: \n", res);
    }
    CURR_INODE = msg->inum;
    free(msg);

    TracePrintf( LOG, "[CLIENT][LOG] CHDir file return %d: \n", res);

    return res;
}

int Stat(char *pathname, struct Stat *statbuf) {
    TracePrintf( LOG, "[CLIENT][LOG] Stat Request for %s\n", pathname);

    Messgae* msg = (Messgae*)malloc(sizeof(Messgae));
    OperationType tp = STAT;
    msg->type = (short) tp;
    msg->inum = CURR_INODE;
    msg->pathnamePtr = pathname;
    msg->bufPtr = statbuf;

    Send((void*)msg, -FILE_SERVER);
    int res =  (int)msg->reply;
    free(msg);
    if (res == ERROR) {
        printf( "[CLIENT][ERR] Stat file errored %d: \n", res);
        return ERROR;
    }
    
    TracePrintf( LOG, "[CLIENT][LOG] Stat %s, inum %d, type %d, nlink %d, size %d: \n", pathname, statbuf->inum, statbuf->type, statbuf->nlink, statbuf->size);

    return 0;
}

int Sync(void) {
    TracePrintf( LOG, "[CLIENT][LOG] Sync Request\n");

    Messgae* msg = (Messgae*)malloc(sizeof(Messgae));
    OperationType tp = SYNC;
    msg->type = (short) tp;

    Send((void*)msg, -FILE_SERVER);

    int res =  (int)msg->reply;
    free(msg);
    if (res != 0) {
        printf( "[CLIENT][ERR] Sync should alway return 0 instead of %d: \n", res);
    }

    TracePrintf( LOG, "[CLIENT][LOG] Finish Sync\n");

    return 0;
}

int Shutdown(void) {
    TracePrintf( LOG, "[CLIENT][LOG] Shutdown Request\n");

    Messgae* msg = (Messgae*)malloc(sizeof(Messgae));
    OperationType tp = SHUTDOWN;
    msg->type = (short) tp;

    Send((void*)msg, -FILE_SERVER);

    int res =  (int)msg->reply;
    free(msg);
    if (res != 0) {
        printf( "[CLIENT][ERR] Shutdown should alway return 0 instead of %d: \n", res);
    }

    TracePrintf( LOG, "[CLIENT][LOG] Finish Shutdown\n");
    
    return 0;
}
