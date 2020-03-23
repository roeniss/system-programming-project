#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "dir.h"

void dir()
{
	DIR *dr = opendir(".");
	struct dirent *dd;
	struct stat buf;

	int no = 0;
	for (dd = readdir(dr); dd; dd = readdir(dr), ++no)
	{
		printf("%10s", dd->d_name);
		stat(dd->d_name, &buf);

		if (no < 2)
			printf(" ");
		else if (S_ISDIR(buf.st_mode))
			printf("/ ");
		else if (buf.st_mode & S_IXUSR || buf.st_mode & S_IXGRP || buf.st_mode & S_IXOTH)
			printf("* ");
		else
			printf(" ");
	}
	printf("\n");
	closedir(dr);
}