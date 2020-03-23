#include <string.h>

#include "debug.h"

bool debug_mode = false;

void check_debug_mode(char *argv[])
{
	if (strcmp(argv[1], "-d") == 0)
	{
		debug_mode = true;
	}
}

bool is_debug_mode()
{
	return debug_mode;
}