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

// read (buffer - 1) size's stdin
char *ready_command(int);

// trim left for target string
char *lTrim(char *);

#endif