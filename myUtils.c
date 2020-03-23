#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "myUtils.h"

// initialize flag
bool flag_global = false;

char *init_buffer(int max_len)
{
	char *buffer = (char *)malloc(sizeof(char) * max_len);
	return buffer;
}

void ready_command(char *buffer, int max_len)
{
	int c;

	printf("sicsim> ");
	// strlen(buffer) become read data size. (maxLen: char array size - 1)
	// (NULL is not counted)
	// but in this case, (max : char array size -2) reason : check below.
	fgets(buffer, max_len, stdin);

	while (buffer[strlen(buffer) - 1] != '\n' && (c = getchar()) != '\n')
		;

	if (strlen(buffer) < 2)
		return;

	// remove last lineberak --> maxLen = strlen(buffer) - 2
	buffer[strlen(buffer) - 1] = '\0';
}

char *lTrim(char *buffer)
{
	char *tBuffer = buffer;
	while (isspace((unsigned char)*tBuffer))
	{
		tBuffer++;
	}
	return tBuffer;
}

enum COMMAND get_command(char *buffer)
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