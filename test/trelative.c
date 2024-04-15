#include <stdio.h>

#include <comp421/yalnix.h>
#include <comp421/iolib.h>

/* Try tcreate2 before this, or try this just by itself */

int
main()
{

    printf("\n%d\n\n", MkDir("a"));
    printf("\n%d\n\n", ChDir("a"));
    printf("\n%d\n\n", Create("b"));

    int fd = Open("b");
    printf("\n%d\n\n", fd);

    char ch[11];

    int nch = Write(fd, "0123456789", 10);
    printf("Write nch %d\n", nch);

    int status = Close(fd);
    printf("Close status %d\n", status);

    fd = Open("b");
    printf("\n%d\n\n", fd);

    nch = Read(fd, &ch, 10);

    printf("Read nch %d, %s\n", nch, ch);

	status = Link("b", "c");
	printf("Link status %d\n", status);

    printf("\n%d\n\n", Unlink("c"));

    printf("\n%d\n\n", Unlink("b"));

    printf("\n%d\n\n", ChDir(".."));
    
    printf("\n%d\n\n", RmDir("a"));



	return 0;
	// Shutdown();
}
