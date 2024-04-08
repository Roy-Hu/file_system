#include <comp421/yalnix.h>
#include <comp421/hardware.h>

#include <comp421/iolib.h>
#include "cache.h"
#include "yfs.h"
#include <stdio.h>

int
main()
{
    TracePrintf( 1, "Communicate test in progress\n");
    Create("/sss");

    // This should return ERR since /sss is a file
	Create("/sss/ssss");

    // This should return ERR since /aaa is not created
    // Create("/aaa/ssss");

    //Create("/ssss");
    //Create("/");
    
    return (0);
}
