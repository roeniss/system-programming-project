#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "dump.h"
#include "myUtils.h"
#include "debug.h"

#define MEMORY_SIZE 1048576

static bool _is_valid_input(char *start, char *end, char *value)
{
	int t1, t2, t3; // t for 'test'
	if (!start || !end || !value)
	{
		printf("Insuffcient paramter (start : %s, end : %s, value : %s)\n", start, end, value);
		flag_global = false;
		return false;
	}
	if (!is_valid_hex(start) || !is_valid_hex(end) || !is_valid_hex(value))
	{
		printf("Invalid paramter (start : %s, end : %s, value : %s)\n", start, end, value);
		flag_global = false;
		return false;
	}

	t1 = strtoul(start, NULL, 16);
	t2 = strtoul(end, NULL, 16);
	t3 = strtoul(value, NULL, 16);

	if (t1 < 0 || t2 < 0 || t1 > MEMORY_SIZE || t2 > MEMORY_SIZE || t1 > t2)
	{
		printf("Invalid Range (start : %s, end : %s)\n", start, end);
		flag_global = false;
		return false;
	}
	if (t3 > 0xFF || t3 < 0)
	{
		printf("Invalid value to fill with (value : %s)\n", value);
		flag_global = false;
		return false;
	}
	return true;
}
void fill(char *VM)
{
	char *start = strtok(NULL, ", \t");
	char *end = strtok(NULL, ", \t");
	char *value = strtok(NULL, ", \t");
	int s, e, v;

	if (is_debug_mode())
	{
		printf("[DEBUG] Input --> start : %s, end : %s, value : %s\n", start, end, value);
	}

	if (!_is_valid_input(start, end, value))
		return;

	s = strtoul(start, NULL, 16);
	e = strtoul(end, NULL, 16);
	v = strtoul(value, NULL, 16);

	if (is_debug_mode())
	{
		printf("[DEBUG] Input (int) --> start : %x, end : %x, value : %x\n", s, e, v);
	}

	for (int i = s; i <= e; i++)
	{
		VM[i] = v;
	}
}