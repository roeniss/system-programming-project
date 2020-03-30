#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "dump.h"
#include "myUtils.h"
#include "debug.h"

extern int MEMORY_SIZE;
extern char*VM;

static bool _validate_input(char *start, char *end, char *value);

int fill(char *start, char *end, char *value)
{
	int i;
	int s, e, val;

	if (is_debug_mode())
		printf("[DEBUG] Input --> start : %s, end : %s, value : %s\n", start, end, value);

	if (!_validate_input(start, end, value))
		return 1;

	s = strtoul(start, NULL, 16);
	e = strtoul(end, NULL, 16);
	val = strtoul(value, NULL, 16);

	for (i = s; i <= e; i++)
		VM[i] = val;

	return 0;
}

bool _validate_input(char *start, char *end, char *value)
{
	int s, e, val;

	// 1 : Insuffcient paramter
	if (!start)
	{
		printf("Insuffcient paramter (start) : '%s'\n", start);
		return false;
	}
	if (!end)
	{
		printf("Insuffcient paramter (end) : '%s'\n", end);
		return false;
	}
	if (!value)
	{
		printf("Insuffcient paramter (value) : '%s'\n", value);
		return false;
	}

	// 2 : Invalid paramter
	if (!is_valid_hex(start))
	{
		printf("Invalid paramter (start) : '%s'\n", start);
		return false;
	}
	if (!is_valid_hex(end))
	{
		printf("Invalid paramter (end) : '%s'\n", end);
		return false;
	}
	if (!is_valid_hex(value))
	{
		printf("Invalid paramter (value) : '%s'\n", value);
		return false;
	}

	s = strtoul(start, NULL, 16);
	e = strtoul(end, NULL, 16);
	val = strtoul(value, NULL, 16);

	// 3 : Unreachable address
	if (s < 0 || MEMORY_SIZE <= s)
	{
		printf("Unreachable address (start) : '%s'\n", start);
		return false;
	}
	if (e < 0 || MEMORY_SIZE <= e)
	{
		printf("Unreachable address (end) : '%s'\n", end);
		return false;
	}

	// 4 : Wrong range
	if (s > e)
	{
		printf("Wrong range (start ~ end) : '%s ~ %s'\n", start, end);
		return false;
	}

	// 5 : Unwritable address
	if (val < 0 || 0xFF < val)
	{
		printf("Unwritable value (value) : '%s'\n", value);
		return false;
	}
	return true;
}