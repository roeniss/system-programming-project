#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

/**
 * Use dirent() to get iterable file list,
 * use stat() to get file's information.
 */
int dir(void) {
  DIR *dr = opendir(".");
  struct dirent *dd;
  struct stat buf;
  char *fname; // filename

  int no = 0; // serial number of specific file
  for (dd = readdir(dr); dd; dd = readdir(dr), ++no) {
    fname = dd->d_name;
    printf("%s", fname);
    stat(fname, &buf);
    if (S_ISDIR(buf.st_mode)) printf("/"); // 'folder'
    else if (buf.st_mode & S_IXUSR
        || buf.st_mode & S_IXGRP
        || buf.st_mode & S_IXOTH)
      printf("*"); // 'executable'
    else printf(" "); // 'others'

    printf("\n");
  }

  closedir(dr);
  return 0;
}

int type(char *filename) {
  FILE *fp = fopen(filename, "r");
  int bufferLength = 255;
  char buffer[bufferLength];

  if (fp == NULL) {
    printf("There is no such file\n");
    return 1;
  }

  while (fgets(buffer, bufferLength, fp))
    printf("%s", buffer);

  printf("\n");
  fclose(fp);
  return 0;
}

int help(void) {
  printf("h[elp]\n");
  printf("d[ir]\n");
  printf("q[uit]\n");
  printf("hi[story]\n");
  printf("du[mp] [start, end]\n");
  printf("e[dit] address, value\n");
  printf("f[ill] start, end, value\n");
  printf("reset\n");
  printf("opcode mnemonic\n");
  printf("opcodelist\n");
  printf("assemble filename\n");
  printf("type filename\n");
  printf("symbol\n");
  printf("progaddr [address]\n");
  printf("loader [object filename1] [object filename2] […]\n");
  printf("bp [address]\n");
  printf("run\n");
  return 0;
}

int quit(void) {
  exit(0);
}
