#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "myUtils.h"
#include "help.h"
#include "dir.h"
#include "quit.h"
#include "history.h"
#include "dump.h"

#include "opcode.h"

#define MAX_INPUT_BUFFRE_SIZE 100

enum command _get_command(char *);

int main()
{
	char *buffer, *tBuffer; // t for "trimmed", tok for "tokenized"
	enum command cmd;

	init_history();
	init_opcode();

	while (1)
	{
		buffer = ready_command(MAX_INPUT_BUFFRE_SIZE);
		printf("[DEBUG] buffer  : %s\n", buffer);
		tBuffer = lTrim(buffer);
		printf("[DEBUG] tBuffer : %s\n", tBuffer);
		cmd = _get_command(tBuffer);

		if (cmd != c_unrecognized)
		{
			printf("[DEBUG] command : %d\n", cmd);
			add_history(buffer);
			if (cmd == c_help)
				help();
			else if (cmd == c_dir)
				dir();
			else if (cmd == c_quit)
				quit();
			else if (cmd == c_history)
				show_history();
			else if (cmd == c_dump)
				dump();
			// else if (cmd == c_edit)
			// 	edit();
			// else if (cmd == c_fill)
			// 	fill();
			// else if (cmd == c_reset)
			// 	reset();
			else if (cmd == c_opcode)
				opcode();
			else if (cmd == c_opcodelist)
				opcodelist();
		}
		else
		{
			// free buffer only when invalid command.
			// otherwise, use it for command history.
			free(buffer);
		}
	}
	return 0;
}

enum command _get_command(char *buffer)
{
	if (!(*buffer))
		return c_unrecognized;

	char *token = strtok(buffer, " ");

	if (strcmp(token, "h") == 0 || strcmp(token, "help") == 0)
		return c_help;
	else if (strcmp(token, "d") == 0 || strcmp(token, "dir") == 0)
		return c_dir;
	else if (strcmp(token, "q") == 0 || strcmp(token, "quit") == 0)
		return c_quit;
	else if (strcmp(token, "hi") == 0 || strcmp(token, "history") == 0)
		return c_history;
	else if (strcmp(token, "du") == 0 || strcmp(token, "dump") == 0)
		return c_dump;
	else if (strcmp(token, "e") == 0 || strcmp(token, "edit") == 0)
		return c_edit;
	else if (strcmp(token, "f") == 0 || strcmp(token, "fill") == 0)
		return c_fill;
	else if (strcmp(token, "reset") == 0)
		return c_reset;
	else if (strcmp(token, "opcode") == 0)
		return c_opcode;
	else if (strcmp(token, "opcodelist") == 0)
		return c_opcodelist;
	else
		return c_unrecognized;
}