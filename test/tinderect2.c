#include <stdio.h>

#include <comp421/yalnix.h>
#include <comp421/iolib.h>

/* After running this, try topen2 and/or tunlink2 */

int
main()
{

	printf("\n%d\n\n", MkDir("/a"));
	printf("\n%d\n\n", Create("/a/b"));
	printf("\n%d\n\n", Create("/a/c"));
    printf("\n%d\n\n", Create("/a/d"));
    printf("\n%d\n\n", Create("/a/e"));
    printf("\n%d\n\n", Create("/a/f"));
    printf("\n%d\n\n", Create("/a/g"));
    printf("\n%d\n\n", Create("/a/h"));
    printf("\n%d\n\n", Create("/a/i"));
    printf("\n%d\n\n", Create("/a/j"));
    printf("\n%d\n\n", Create("/a/k"));
    printf("\n%d\n\n", Create("/a/l"));
    printf("\n%d\n\n", Create("/a/m"));

    printf("\n%d\n\n", Unlink("/a/m"));
    printf("\n%d\n\n", Unlink("/a/l"));
    printf("\n%d\n\n", Unlink("/a/k"));

    printf("\n%d\n\n", Create("/a/k"));
    printf("\n%d\n\n", Create("/a/l"));
    printf("\n%d\n\n", Create("/a/m"));

	return 0;
	//Shutdown();
}
