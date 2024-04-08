#include <stdio.h>

#include <comp421/yalnix.h>
#include <comp421/iolib.h>

/* After running this, try topen2 and/or tunlink2 */

int
main()
{
	printf("\n%d\n\n", Create("/foo"));
	printf("\n%d\n\n", Create("/bar"));
	// create a file? should return error
	printf("\n%d\n\n", Create("/foo"));
	printf("\n%d\n\n", Create("/foo/zzz"));
	return 0;
	//Shutdown();
}
