#include <stdbool.h>

#ifndef MY_UNILS_H
#define MY_UNILS_H

enum COMMAND
{
	c_help,
	c_dir,
	c_quit,
	c_history,
	c_dump,
	c_edit,
	c_fill,
	c_reset,
	c_opcode,
	c_opcodelist,
	c_unrecognized
};

// check whether previous command works successfully or not
extern bool flag_global;

// allocate new buffer memory
extern char *init_buffer(int);

// read (buffer - 2) size's stdin
extern void ready_command(char *, int);

// trim left
extern char *lTrim(char *);

// determine what the paramter is command of
enum COMMAND get_command(char *);

#endif