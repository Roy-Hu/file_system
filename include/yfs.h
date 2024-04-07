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
typedef struct message {
    short type; /* message type */
    char *path_oldName; /* used interchangeable with oldname */
    int fd_len; /* used intercahgeable with len*/
    void *buff;
    int whence;
    int offset_size; /* used interchangeable with size */
    struct Stat *statbuf;
    char *newname;
    short data; /* reply message/curr_dir */

}messgae;

int msgHandler(struct message* msg, int pid);
int checkNnormalizePathname(char* pathname);
int findInum(char* pathname, int curr_inum);
int retrieveDir(int inum, char* dirname);
void init();

#endif