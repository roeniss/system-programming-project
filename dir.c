#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "dir.h"

//
// Use dirent() to get iterable file list,
// use stat() to get file's information.
//
int dir(void)
{
	DIR *dr = opendir(".");
	struct dirent *dd;
	struct stat buf;

	int no = 0;   //serial number of speicifc file
	int cols = 0; // for prettier formatting
	for (dd = readdir(dr); dd; dd = readdir(dr), ++no)
	{
		cols++;
		printf("%14s", dd->d_name);
		stat(dd->d_name, &buf);

		if (S_ISDIR(buf.st_mode))
			printf("/\t");
		else if (buf.st_mode & S_IXUSR || buf.st_mode & S_IXGRP || buf.st_mode & S_IXOTH)
			printf("*\t");
		else
			printf("\t");

		if (cols == 3)
		{
			cols = 0;
			printf("\n");
		}
	}
	printf("\n");
	closedir(dr);
	return 0;
}