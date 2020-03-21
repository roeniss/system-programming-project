#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void dir()
{
	DIR *dr = opendir(".");
	struct dirent *dd;
	struct stat buf;

	for (dd = readdir(dr); dd; dd = readdir(dr))
	{
		printf("%-10s", dd->d_name);
		stat(dd->d_name, &buf);
		if (dd->d_type & DT_DIR)
			printf("/ ");
		else if (buf.st_mode & S_IXUSR && strcmp(".", dd->d_name) && strcmp("..", dd->d_name))
			printf("* ");
		else
			printf(" ");
	}
	printf("\n");
	closedir(dr);
}