#include <comp421/iolib.h>
#include <comp421/filesystem.h>
#include <comp421/yalnix.h>

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "yfs.h"

/* keep track of current innode */
int curInode = ROOTINODE;
/*
 * For an Open or Create
 * request, your file system library learns the file’s inode number from the file server process (the current
 * position within the file is initialized to 0) and stores this within the data structure in the library representing
 * that open file.
 */
typedef struct openedFile {
    int iNum;
    int curPos;
    bool isValid; /* 1: for curr entry is valid, 0 otherwise */
} OpenedFile;

/* array of struct to keep track of opened files */
// openedFile* filesOpened[MAX_OPEN_FILES];

OpenedFile openedFiles[MAX_OPEN_FILES];

bool isInit = false;

void setOpenFile(int iNum, int fd) {
    openedFiles[fd].iNum = iNum;
    openedFiles[fd].curPos = 0;
    openedFiles[fd].isValid = true;
}

// check if file is opened given inum
bool opened(int iNum) {
    int i;
    for (i = 0; i < MAX_OPEN_FILES; i++) {
        if (openedFiles[i].iNum == iNum) return true;
    }

    return false;
}

// find a new file descriptor, return -1 if not found
int findNewFd() {
    int i;
    for (i = 0; i < MAX_OPEN_FILES; i++) {
        if (!openedFiles[i].isValid) return i;
    }

    return -1;
}

// init the opened file table
void init() {
    int i;
    for (i = 0; i < MAX_OPEN_FILES; i++) {
        openedFiles[i].isValid = false;
    }

    isInit = true;
}

int Open(char *pathname) {
    if (!isInit) init();

    TracePrintf( 1, "[CLIENT][LOG] Open Request for %s\n", pathname);

    Messgae* msg = (Messgae*)malloc(sizeof(Messgae));
    OperationType tp = OPEN;
    msg->type = (short) tp;
    msg->path_oldName = pathname;

    Send((void*)msg, -FILE_SERVER);
    int res = (int)msg->data;
    if (res != ERROR) {
        int fd = findNewFd();
        if (fd == -1) {
            TracePrintf( 1, "[CLIENT][ERR] No more file can be opened\n");
            free(msg);
            return ERROR;
        }

        TracePrintf( 1, "[CLIENT][LOG] Open file inum %d, fd %d: \n", res, fd);

        setOpenFile(res, fd);
        free(msg);
        return fd;
    } else {
        TracePrintf( 1, "[CLIENT][ERR] Fail to open file\n");
        free(msg);
        return res;
    }
    free(msg);

    return res;
}

/* This request closes the open file specified by the file descriptor number fd . If fd is not the descriptor
number of a file currently open in this process, this request returns ERROR; otherwise, it returns 0. */
int Close(int fd) {
    TracePrintf( 1, "[CLIENT][LOG] Close Request for fd: %d\n", fd);
    if (fd < 0 || fd > MAX_OPEN_FILES || openedFiles[fd].isValid == false || isInit == false) {
        TracePrintf( 1, "[CLIENT][ERR] Close: Not a valid fd: %d number!\n", fd);
        return ERROR;
    }
    openedFiles[fd].isValid = false;
    openedFiles[fd].iNum = 0;
    openedFiles[fd].curPos = 0;
    TracePrintf( 1, "[CLIENT][LOG] Close: Closed file successfully with fd: %d number!\n", fd);
    return 0;
}

int Create(char *pathname) {
    if (!isInit) init();

    TracePrintf( 1, "[CLIENT][LOG] Create Request for %s\n", pathname);

    Messgae* msg = (Messgae*)malloc(sizeof(Messgae));
    OperationType tp = CREATE;
    msg->type = (short) tp;
    msg->path_oldName = pathname;

    Send((void*)msg, -FILE_SERVER);
    int res = (int)msg->data;
    if (res != ERROR) {

        int fd = findNewFd();
        if (fd == -1) {
            TracePrintf( 1, "[CLIENT][ERR] No more file can be opened\n");
            return ERROR;
        }

        TracePrintf( 1, "[CLIENT][LOG] Created file inum %d, fd %d: \n", res, fd);

        setOpenFile(res, fd);

        return fd;
    } else {
        TracePrintf( 1, "[CLIENT][ERR] Fail to create file\n");
        return res;
    }
}

// int Read(int fd, void *buf, int size) {
//     return 0;
// }

int Write(int fd, void *buf, int size) {
    // init fd
    if (!isInit) init();
    if (fd < 0 || fd > MAX_OPEN_FILES || openedFiles[fd].isValid == false || isInit == false) {
        TracePrintf( 1, "[CLIENT][ERR] Write: Not a valid fd: %d number!\n", fd);
        return ERROR;
    }
    int inum = openedFiles[fd].iNum;
    int curpos = openedFiles[fd].curPos;

    Messgae* msg = (Messgae*)malloc(sizeof(Messgae));
    OperationType tp = WRITE;
    msg->type = (short) tp;
    msg->offset_size = size;
    msg->buff = buf;
    msg->curpos_len = curpos;
    msg->whence_inum = inum;
    Send((void*)msg, -FILE_SERVER);
    return 0;
}

// int Seek(int fd, int offset, int whence) {
//     return 0;
// }

// int Link(char *oldname, char *newname) {
//     return 0;
// }

// int Unlink(char *pathname) {
//     return 0;
// }

// int SymLink(char *oldname, char *newname) {
//     return 0;
// }

// int ReadLink(char *pathname, char *buf, int len) {
//     return 0;
// }

int MkDir(char *pathname) {
    TracePrintf( 1, "[CLIENT][LOG] MkDir Request for %s\n", pathname);

    Messgae* msg = (Messgae*)malloc(sizeof(Messgae));
    OperationType tp = MKDIR;
    msg->type = (short) tp;
    msg->path_oldName = pathname;

    Send((void*)msg, -FILE_SERVER);
    int res = (int)msg->data;
    
    TracePrintf( 1, "[CLIENT][LOG] Created file return %d: \n", res);

    return res;

    return 0;
}

// int RmDir(char *pathname) {
//     return 0;
// }

// int ChDir(char *pathname) {
//     return 0;
// }

// int Stat(char *pathname, struct Stat *statbuf) {
//     return 0;
// }

// int Sync(void) {
//     return 0;
// }

int Shutdown(void) {
    return 0;
}
