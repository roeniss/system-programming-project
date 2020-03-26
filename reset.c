#include "reset.h"

extern char*VM;

int reset(void)
{

	for (int i = 0; i < sizeof(VM); i++)
		VM[i] = '\0';
	return 0;
}