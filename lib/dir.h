#ifndef DIR_H
#define DIR_H

//
// Show current working directory's file list including folders
// with category (file, directory, executable).
//
int dir(void);

//
// Show text of the file which has a name of a given filename
//
int type(char *filename);

#endif