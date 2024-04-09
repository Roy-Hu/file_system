## TO-DO:
1. Init free blocks and inodes in the file system (finished-ish, not tested)
2. Finish handling functions to deal with requests send by the client (kernel) (request sending and receiving work!)
3. Finish each functions in iolib.c
    - ### in progress
    - ### Need to be done:
        - Link
        - Unlink
        - Readlink
        - RmDir
        - ChDir
        - Stat
        - Sync
        - Shutdown
    - ### Finished
        - Create
        - Close
        - Open
        - Write
        - Read
        - Seek
4. LRU cache

### use the bash for testing
```
chmod +x testshell.sh
```
```
testshell.sh
```
### Dummy cahce for now
- Given a block number, return a pointer to a copy of that block in memory
- In the real cache, that pointer should point to one of the BLOCK_CACHESIZE number of blocks in the cache
- But in the initial “dummy” cache, always read the block from disk and just return a pointer to some static buffer or malloc a buffer for it
- implemented as **read_block** function in chache.c

