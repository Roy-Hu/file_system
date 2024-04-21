#include <stdio.h>

#include <comp421/yalnix.h>
#include <comp421/iolib.h>
#include <stdlib.h>

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

    Seek(fd, 20, SEEK_SET);

	nch = Read(fd, &ch, 128);
	printf("Read nch %d\n", nch);
    printf("ch %s \n", ch);

    nch = Write(fd, "abcdefghijklmnopqrstuvwxyz", 26);
    printf("Write nch %d\n", nch);


    Seek(fd, 11, SEEK_SET);

	nch = Read(fd, &ch, 128);
	printf("Read nch %d\n", nch);
    printf("ch %s \n", ch);

    Seek(fd, 15, SEEK_SET);

	nch = Read(fd, &ch, 128);
	printf("Read nch %d\n", nch);
    printf("ch %s \n", ch);


    Seek(fd, 19, SEEK_SET);

	nch = Read(fd, &ch, 128);
	printf("Read nch %d\n", nch);
    printf("ch %s \n", ch);

    Seek(fd, 20, SEEK_SET);

	nch = Read(fd, &ch, 128);
	printf("Read nch %d\n", nch);
    printf("ch %s \n", ch);

    struct Stat* stat = malloc(sizeof(struct Stat));

    printf("\n%d\n\n", Stat("/xxxxxx", stat));
    printf("stat inum: %d, type: %d, nlink: %d, size: %d\n", stat->inum, stat->type, stat->nlink, stat->size);

    status = Close(fd);
    printf("Close status %d\n", status);

    Shutdown();
}
