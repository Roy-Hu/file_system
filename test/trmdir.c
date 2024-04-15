#include <comp421/yalnix.h>
#include <comp421/hardware.h>

#include <comp421/iolib.h>
#include "cache.h"
#include "yfs.h"
#include <stdio.h>

int
main()
{
    printf("MkDir test in progress\n");
    // int fd = Open("/sss");
    //Write(fd, "cccccccccccccccc", 16);
    printf("Mkdir: %d\n", MkDir("/hello"));
    printf("Mkdir: %d\n", MkDir("/hello/hello"));
    printf("RmDir: %d\n", RmDir("/hello/hello"));
    // should return error
    printf("RmDir: %d\n", RmDir("/hello/hello"));
    // printf("RmDir: %d\n", RmDir("/"));
    

    // This should return ERR since /sss is a file

    // This should return ERR since /aaa is not created
    // Create("/aaa/ssss");

    //Create("/ssss");
    //Create("/");
    
    return (0);
}
