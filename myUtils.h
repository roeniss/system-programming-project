#include <stdbool.h>

#ifndef MY_UNILS_H
#define MY_UNILS_H

enum command
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
bool flag_global;

char *init_buffer(int);

// read (buffer - 2) size's stdin
void ready_command(char *, int);

// trim left
char *lTrim(char *);

#endif