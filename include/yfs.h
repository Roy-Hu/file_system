#ifndef YFS_H
#define YFS_H
#include <stdio.h>
#include <stdlib.h>
#include <comp421/filesystem.h>
#include <comp421/yalnix.h>
#include <comp421/iolib.h>

/* identify request type */
typedef enum {
    OPEN,
    CLOSE,
    CREATE,
    READ,
    WRITE,
    SEEK,
    LINK,
    UNLINK,
    SYMLINK,
    READLINK,
    MKDIR,
    RMDIR,
    CHDIR,
    STAT,
    SYNC,
    SHUTDOWN
} OperationType;



/* should be exactly 32 bytes */
typedef struct messgae {
    short type; /* message type */
    char *pathName; /* used interchangeable with oldname */
    int fd; /* used intercahgeable with len*/
    void *buff;
    int whence;
    int offset; /* used interchangeable with size */
    struct Stat *statbuf;
    char *newname;
    short padding;

}messgae;


void init();

#endif