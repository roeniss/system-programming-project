#include "reset.h"

extern char *VM;
extern int MEMORY_SIZE;

int reset(void)
{
	for (int i = 0; i < MEMORY_SIZE; i++)
		VM[i] = '\0';
	return 0;
}