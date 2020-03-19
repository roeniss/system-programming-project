#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <stdarg.h>

#include "myUtils.h"

#define MAX_INPUT_BUFFRE_SIZE 100

void execute_command(char *);

int main()
{
	char *buffer;

	while (1)
	{
		buffer = ready_command(MAX_INPUT_BUFFRE_SIZE);
		// buffer = trim(buffer);

		execute_command(buffer);
	}
	return 0;
}

void execute_command(char *buffer)
{
	if (strcmp(buffer, "help") == 0 || strcmp(buffer, "h") == 0)
	{
		// printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\n");
	}
}
