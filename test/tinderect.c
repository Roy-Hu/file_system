#include <stdio.h>

#include <comp421/yalnix.h>
#include <comp421/iolib.h>

/* After running this, try topen2 and/or tunlink2 */

int
main()
{
	printf("\n%d\n\n", MkDir("/a"));
	printf("\n%d\n\n", MkDir("/b"));
	printf("\n%d\n\n", MkDir("/c"));
    printf("\n%d\n\n", MkDir("/d"));
    printf("\n%d\n\n", MkDir("/e"));
    printf("\n%d\n\n", MkDir("/f"));
    printf("\n%d\n\n", MkDir("/g"));
    printf("\n%d\n\n", MkDir("/h"));
    printf("\n%d\n\n", MkDir("/i"));
    printf("\n%d\n\n", MkDir("/j"));
    printf("\n%d\n\n", MkDir("/k"));
    printf("\n%d\n\n", MkDir("/l"));
    printf("\n%d\n\n", MkDir("/m"));
    printf("\n%d\n\n", MkDir("/n"));
    printf("\n%d\n\n", MkDir("/o"));
    printf("\n%d\n\n", MkDir("/p"));

    printf("\n%d\n\n", RmDir("/p"));
    printf("\n%d\n\n", RmDir("/o"));
    printf("\n%d\n\n", RmDir("/n"));

    printf("\n%d\n\n", MkDir("/n"));
    printf("\n%d\n\n", MkDir("/o"));
    printf("\n%d\n\n", MkDir("/p"));

	return 0;
	//Shutdown();
}
