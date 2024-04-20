## COMP 521 Lab3: Yalnix File System
### Introduction
This is a simple file system for the Yalnix operating system, called YFS (Yalnix File System). The file system support multiple users, with a tree-structured (hierarchical) directory, and it is similar in functionality and disk layout to the “classical” Unix file system. Unlike the Unix file system implementation, YFS is implemented as a server process running on top of the Yalnix kernel, rather than being implemented within the kernel itself.

### Authors
* Chien-An Hu(ch155), Qiyuan Yang(qy28)

### Structures
There are mainly three components in YFS. First and foremost, the io library (iolib). It serves as the interfaces for each processes. Process (client) can use the function interfaces in this library, which will sending requests to our YFS server. Then the next compoentes will be the YFS. It is continuously listening for requests sending by process and handle different requests by calling corresponding functions. The third component is the cache. The cache is implemented using a public library called uthash<sup id="a1">[1](#f1)</sup>. The cache is an LRU cache, which will evict the least recent used block/inodes.

### sources
#### yfs.c
Contains the functions for initializing the YFS system as well as the codes for handling request from the iolibs.
- msgHandler: for making function calls to handle different types of requests
- init: for initilizing the disk contents
- main: loop for receiving and reply to requests

#### call.c
Contains functions to deal with requests, called in msgHandler
- yfsOpen: handling the open request
- yfsWrite: handling the write request
- yfsRead: handling the read request
- yfsSeek: handling the seek request
- yfsMkdir: handling the make directions request
- yfsRmDir: handling the remove direction request
- yfsChDir: handling the change direction request
- create: creating a new inode number given current direction and name
- yfsUnLink: handling the unlink request
- yfsStat: handling the stat request
- yfsSync: handling the syncronization request
- yfsShutdown: handling the shutdown request

#### disk.c
Contains helper functions related to disk operations
- getInodeBlockNum: get the block number to which given Inum belongs
- getFreeBlock: check if there is any free blocks in an array we maintained called **freeBlocks**
- getFreeInode: Find free inodes in the inode list we maintained called **freeInodes**

#### helper.c
Contains some general helper functions
- normPathname: given a random pathname and normalize it
- getLastName: given a path name, get the last name before the last slash (parent direction)
- setdirName: set the newly created directory/file name, similar function to strncpy
- cmpDirName: compare a directory entry's name with a given eName

#### inode.c
Contains some functions related to inode handling
- printInode: used for debug purposes, check what does this inode contain
- printdirentry: used for debug purposes, check what does this direntry contain
- inodeCreate: Create a new inode for directory/file
- inodeDelete: delete an inode given inum
- inodeDelEntry: delete an inode entry given name and parent directory
- inodeReadWrte: read/write to an inode given buf
- incrementNlink: increase the number of hard links given inum
- decrementNlink: decrease the number of hard links given inum
- inodeAddEntry: andd an inode entry given parent inum
- inumFind: find the inum of last dir (before the last slash)
- inumRetrieve: retrieve the directory inum give the dir/filename

#### cache.c
Contains some functions to handle cache read, write, and replacement
- writeInode: write the inode
- findInode: find the inode given inum
- read_block: given block number, read the content of the block
- init_node_lru: initialize the size of the block cache and inode cache
- lRUGetNode: get the inode given key (inum)
- lRUGetBlk: get the block given key (block num)
- lRUNodePut: put the inode to the cache given key (inum)
- lRUBlkPut: put the block to the cache given key (block num)
- lRUWriteDirty: write dirty blocks from inodes/blocks back to disk







### References

<div id="refs" class="references">
<p><span id="f1">1. </span>troydhanson et al. <em>uthash</em>.  <a href="https://github.com/troydhanson/uthash">https://github.com/troydhanson/uthash</a></p>
</div>