#include <stdio.h>

#include <comp421/yalnix.h>
#include <comp421/iolib.h>

int
main()
{
    int fd;
    int nch;
    int status;
    char ch[128];

    fd = Create("/xxxxxx");
    printf("Create fd %d\n", fd);

    nch = Write(fd, "\0", 1);
    printf("Write nch %d\n", nch);

    nch = Write(fd, "0123456789", 10);
    printf("Write nch %d\n", nch);

    Seek(fd, 1, SEEK_SET);

	nch = Read(fd, &ch, 128);
	printf("Read nch %d\n", nch);
    printf("ch %s \n", ch);

    nch = Write(fd, "abcdefghijklmnopqrstuvwxyz", 26);
    printf("Write nch %d\n", nch);

    status = Close(fd);
    printf("Close status %d\n", status);

    Sync();

    fd = Open("/xxxxxx");
    printf("Open fd %d\n", fd);

	nch = Read(fd, &ch, 128);
	printf("Read nch %d\n", nch);
    printf("ch %s \n", ch);

    status = Close(fd);
    printf("Close status %d\n", status);

    Shutdown();
}
