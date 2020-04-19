#ifndef OTHERS_H
#define OTHERS_H


/**
 * Show current working directory's file list including folders
 * with category (file, directory, executable)
 */
int dir(void);

/**
 * Show whole text of the file
 */
int type(char *filename);

/**
 * Show available commands
 */
int help(void);

/**
 * Quit this program
 */
int quit(void);

#endif
