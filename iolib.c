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

bool opened(int iNum) {
    int i;
    for (i = 0; i < MAX_OPEN_FILES; i++) {
        if (openedFiles[i].iNum == iNum) return true;
    }

    return false;
}

int findNewFd() {
    int i;
    for (i = 0; i < MAX_OPEN_FILES; i++) {
        if (!openedFiles[i].isValid) return i;
    }

    return -1;
}

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
            return ERROR;
        }

        TracePrintf( 1, "[CLIENT][LOG] Open file inum %d, fd %d: \n", res, fd);

        setOpenFile(res, fd);

        return fd;
    } else {
        TracePrintf( 1, "[CLIENT][ERR] Fail to open file\n");
        return res;
    }


    return res;
}

// int Close(int fd) {
//     return 0;
// }

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

// int Write(int fd, void *buf, int size) {
//     return 0;
// }

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

// int Shutdown(void) {
//     return 0;
// }