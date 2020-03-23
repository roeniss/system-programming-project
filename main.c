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
#include "reset.h"
#include "debug.h"

#define MAX_INPUT_BUFFRE_SIZE 100

int main(int argc, char *argv[])
{
	char *buffer, *tBuffer, *VM, *hBuffer; // t for "trimmed", h for "history"

	buffer = init_buffer(MAX_INPUT_BUFFRE_SIZE);
	hBuffer = init_buffer(MAX_INPUT_BUFFRE_SIZE);

	if (argc > 1)
		check_debug_mode(argv);

	init_history();
	init_opcode();
	VM = init_vm();

	while (1)
	{
		flag_global = true;
		ready_command(buffer, MAX_INPUT_BUFFRE_SIZE);
		strcpy(hBuffer, buffer);
		if (is_debug_mode())
			printf("[DEBUG] buffer  : %s\n", buffer);
		tBuffer = lTrim(buffer);
		if (is_debug_mode())
			printf("[DEBUG] tBuffer : %s\n", tBuffer);

		switch (get_command(tBuffer))
		{
		case c_help:
			help();
			break;
		case c_dir:
			dir();
			break;
		case c_quit:
			quit();
			break;
		case c_history:
			// history is exception
			add_history(hBuffer);
			flag_global = false;
			show_history();
			break;
		case c_dump:
			dump(VM);
			break;
		// case c_edit:
		// 	edit();
		// 	break;
		// case c_fill:
		// 	fill();
		// 	break;
		case c_reset:
			reset(VM, MAX_INPUT_BUFFRE_SIZE);
			break;
		case c_opcode:
			opcode();
			break;
		case c_opcodelist:
			opcodelist();
			break;
		case c_unrecognized:
			break;
		}
		if (flag_global)
			add_history(hBuffer);
	}
	return 0;
}
