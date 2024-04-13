#include <stdio.h>

#include <comp421/yalnix.h>
#include <comp421/iolib.h>
#include <stdlib.h>

/* After running this, try topen2 and/or tunlink2 */

int
main()
{
    struct Stat* stat = malloc(sizeof(struct Stat));
    
	printf("\n%d\n\n", Create("/foo"));
	printf("\n%d\n\n", Create("/bar"));
	// create a file? should return error
	printf("\n%d\n\n", Create("/foo"));
	printf("\n%d\n\n", Create("/foo/zzz"));

    printf("\n%d\n\n", Stat("/foo", stat));
    printf("stat inum: %d, type: %d, nlink: %d, size: %d\n", stat->inum, stat->type, stat->nlink, stat->size);
    printf("\n%d\n\n", Stat("/bar", stat));
    printf("stat inum: %d, type: %d, nlink: %d, size: %d\n", stat->inum, stat->type, stat->nlink, stat->size);s

	return 0;
	//Shutdown();
}
