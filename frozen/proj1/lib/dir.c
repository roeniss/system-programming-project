#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

//
// Use dirent() to get iterable file list,
// use stat() to get file's information.
//
int dir(void) {
    DIR *dr = opendir(".");
    struct dirent *dd;
    struct stat buf;

    int no = 0;   //serial number of specific file
    int cols = 0; // for prettier formatting
    for (dd = readdir(dr); dd; dd = readdir(dr), ++no) {
        cols++;
        printf("%s", dd->d_name);
        stat(dd->d_name, &buf);
        if (S_ISDIR(buf.st_mode))
            printf("/\t");
        else if (buf.st_mode & S_IXUSR || buf.st_mode & S_IXGRP || buf.st_mode & S_IXOTH)
            printf("*\t");
        else
            printf(" \t");

        if (cols == 3) {
            cols = 0;
            printf("\n");
        }
    }
    printf("\n");
    closedir(dr);
    return 0;
}