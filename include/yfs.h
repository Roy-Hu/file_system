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
    short type; 
    short reply;

    int size;
    int inum;
    int pos;

    void* bufPtr;
    char* pathnamePtr;
} Messgae;

int msgHandler(Messgae* msg, int pid);

void init();

#endif