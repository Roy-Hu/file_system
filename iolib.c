#include <comp421/iolib.h>
#include <comp421/filesystem.h>
#include "cache.h"
#include "yfs.h"
int Open(char *pathname) {
    TracePrintf(1, "Enter Open File Request, attempting to send message to server...\n");
    struct message* msg = (struct message*)malloc(sizeof(struct message));
    OperationType tp = OPEN;
    msg->type = (short) tp;
    msg->path_oldName = pathname;
    Send((void*)msg, -FILE_SERVER);
    return 0;
}

// int Close(int fd) {
//     return 0;
// }

// int Create(char *pathname) {
//     return 0;
// }

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

// int MkDir(char *pathname) {
//     return 0;
// }

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