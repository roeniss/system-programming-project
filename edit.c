#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "edit.h"
#include "myUtils.h"
#include "debug.h"
#include "dump.h"

#define MEMORY_SIZE 1048576 // [TODO] dump.c 와 중복 선언중

bool _is_valid_input(char *address, char *value, int *addr, int *val)
{
	// validation check 1 : check missing parameters
	if (!address || !value)
	{
		printf("There's some missing parameters : address(%s), value(%s)\n", address, value);
		flag_global = false;
		return false;
	}

	// validation check 2 : check non-hex style string
	if (!is_valid_hex(address) || !is_valid_hex(value))
	{
		printf("Some paramters have invalid value: address(%s), value(%s)\n", address, value);
		flag_global = false;
		return false;
	}

	*addr = strtoul(address, NULL, 16);
	*val = strtoul(value, NULL, 16);

	// validation check 3 : check wrong address
	if (*addr < 0 || *addr >= MEMORY_SIZE)
	{
		printf("Invalid address : '%s'\n", address);
		flag_global = false;
		return false;
	}

	// validation check 4 : check wrong value
	if (*val < 0 || *val > 0xFF)
	{
		printf("Invalid value : '%s'\n", value);
		flag_global = false;
		return false;
	}

	return true;
}

void edit(char *VM)
{
	char *address = strtok(NULL, ",");
	char *value = strtok(NULL, " ");
	int addr, val;

	if (is_debug_mode())
		printf("[DEBUG] address : %s, value : %s\n", address, value);

	if (!_is_valid_input(address, value, &addr, &val))
		return;

	VM[addr] = val;
}
