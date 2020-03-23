#include "reset.h"

void reset(char *VM, int size)
{

	for (int i = 0; i < size; i++)
	{
		VM[i] = '\0';
	}
}