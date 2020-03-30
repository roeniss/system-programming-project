#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "myUtils.h"
#include "help.h"
#include "dir.h"
#include "quit.h"
#include "history.h"
#include "dump.h"
#include "opcode.h"
#include "reset.h"
#include "debug.h"
#include "edit.h"
#include "fill.h"

extern int MEMORY_SIZE;
extern int DUMP_COLS;
extern int DUMP_ROWS;

#define MAX_INPUT_BUFFER_SIZE 100
#define MAX_PARAMETERS_COUNT 3
#define MAX_PARAMETER_SIZE 10

enum Command
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
typedef enum Command Command;

struct Buffer
{
	char *input;
	char *input_copy;
	char **parameter;
	Command command;
};
typedef struct Buffer Buffer;

// extern/global variables
int MEMORY_SIZE = 1048576,
	DUMP_COLS = 16,
	DUMP_ROWS = 10;
char *VM;

// buffer is not actual extern, but here for easy-access in this file.
Buffer *buffer;

static void _init_vm(char **VM);
static void _init_buffer();
void _receive_command();
static void _parse_command();
static void _execute_command(int *command_flag);
static Command _get_command(char *token);
static char *_left_trim(char *input);

int main(int argc, char *argv[])
{
	int command_flag = 0;

	if (argc > 1)
		check_debug_mode(argc, argv);

	_init_vm(&VM);
	_init_buffer();
	init_history();
	init_opcode();

	while (true)
	{
		command_flag = 0; // '0' means 'no error'
		_receive_command();
		_parse_command();
		_execute_command(&command_flag);
	}
	return 0;
}

//
// Allocate virtual memory.
//
void _init_vm(char **VM)
{
	int i;
	*VM = (char *)malloc(sizeof(char) * MEMORY_SIZE);

	for (i = 0; i < MEMORY_SIZE; i++)
		(*VM)[i] = '\0';

	if (is_debug_mode())
		adjust_test_case();
}

//
// Allocate buffer struct.
//
void _init_buffer()
{
	int i;
	buffer = (Buffer *)malloc(sizeof(Buffer));
	buffer->input = (char *)malloc(sizeof(char) * MAX_INPUT_BUFFER_SIZE);
	buffer->input_copy = (char *)malloc(sizeof(char) * MAX_INPUT_BUFFER_SIZE);
	buffer->parameter = (char **)malloc(sizeof(char *) * MAX_PARAMETERS_COUNT);
	for (i = 0; i < MAX_PARAMETERS_COUNT; i++)
		buffer->parameter[i] = (char *)malloc(sizeof(char) * MAX_PARAMETER_SIZE);
}

//
// Get input, then store it to pointer.
// Basically, this function corresponds to
// input("sicsim> ") function in Python.
//
void _receive_command()
{
	int tmpChar;
	printf("sicsim> ");

	// fgets in C++ reference said, "Reads characters from stream and
	// stores them as a C string into str until
	// (1) {num-1} characters have been read or
	// (2) either a newline or (3) the end-of-file is reached,
	// whichever happens first."
	// So I insert "MAX_INPUT_BUFFER_SIZE + 1" on 2nd parameter
	// to get "MAX_INPUT_BUFFER_SIZE" at most.
	fgets(buffer->input, MAX_INPUT_BUFFER_SIZE + 1, stdin);

	// get char one by one until meeting LF ('\n')
	while (buffer->input[strlen(buffer->input) - 1] != '\n' && (tmpChar = getchar()) != '\n')
		;

	// remove last character if it is '\n' because this program don't need it
	if (buffer->input[strlen(buffer->input) - 1] == '\n')
		buffer->input[strlen(buffer->input) - 1] = '\0';
}

void _parse_command()
{
	int i;
	// 1. Get input
	strcpy(buffer->input, _left_trim(buffer->input));
	// store left-trimmed full input for adding to history list
	strcpy(buffer->input_copy, buffer->input);

	// 2. Get command
	buffer->command = _get_command(strtok(buffer->input, " \t"));

	// 3. Get parameters
	for (i = 0; i < MAX_PARAMETERS_COUNT; i++)
		buffer->parameter[i] = strtok(NULL, ", \t");

	if (is_debug_mode())
		printf("[DEBUG] \ninput : %s, \ncommand : %s, \ncommand_num : %d, \nparameter 1 : %s, \nparameter 2 : %s, \nparameter 3 : %s.\n",
			   buffer->input_copy, buffer->input, buffer->command, buffer->parameter[0], buffer->parameter[1], buffer->parameter[2]);
}

void _execute_command(int *command_flag)
{
	switch (buffer->command)
	{
	case c_help:
		*command_flag = help();
		break;
	case c_dir:
		*command_flag = dir();
		break;
	case c_quit:
		*command_flag = quit();
		break;
	case c_history:
		// only this command needs to add history log before execution.
		add_history(buffer->input);
		*command_flag = history();
		// so, suppress the returned command flag by force
		*command_flag = 1;
		break;
	case c_dump:
		*command_flag = dump(buffer->parameter[0], buffer->parameter[1]);
		break;
	case c_edit:
		*command_flag = edit(buffer->parameter[0], buffer->parameter[1]);
		break;
	case c_fill:
		*command_flag = fill(buffer->parameter[0], buffer->parameter[1], buffer->parameter[2]);
		break;
	case c_reset:
		*command_flag = reset();
		break;
	case c_opcode:
		*command_flag = opcode(buffer->parameter[0]);
		break;
	case c_opcodelist:
		*command_flag = opcodelist();
		break;
	case c_unrecognized:
		*command_flag = 1;
		break;
	}
	if (*command_flag == 0)
		add_history(buffer->input_copy);
}

Command _get_command(char *token)
{
	if (!token || !(*token))
		return c_unrecognized;

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

// Make a pointer, not actual value.
char *_left_trim(char *input)
{
	char *tBuffer = input;
	while (isspace((unsigned char)*tBuffer))
		tBuffer++;
	return tBuffer;
}
