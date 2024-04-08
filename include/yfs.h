#ifndef YFS_H
#define YFS_H

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
    char *path_oldName; /* used interchangeable with oldname */
    int fd_len; /* used intercahgeable with len*/
    void *buff;
    int whence;
    int offset_size; /* used interchangeable with size */
    struct Stat *statbuf;
    char *newname;
    short data; /* reply message/curr_dir */

} Messgae;

int msgHandler(Messgae* msg, int pid);

void init();

#endif