#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include "dump.h"
#include "myUtils.h"
#include "debug.h"

#define MEMORY_SIZE 1048576 // [TODO] 이렇게 선언하면 다른데서도(ex. edit) 또 선언해줘야 되니까 다른 방법을 강구해야 함
#define DEFAULT_DUMP_COL_LENGTH 160

int dump_global_offset = 0;
char test_text[100] = "This is sample Program";

bool is_valid_hex(char *input)
{
	int length = strlen(input);
	char target, filter[24] = "-1234567890abcdefABCDEF";
	char *target_addr;
	for (int i = 0; i < length; i++)
	{
		target = input[i];
		if (i == 0)
			target_addr = strchr(filter, target);
		else
			target_addr = strchr(filter + 1, target);
		if (target_addr == NULL)
		{
			return false;
		}
	}
	return true;
}

bool _validate_input(char *start, char *end)
{
	// vaildation number 1 : invalid paramter (not right number)
	int s, e;
	if (start && !is_valid_hex(start))
	{
		printf("A paramter ('start') has invalid value : '%s'\n", start);
		return false;
	}
	if (end && !is_valid_hex(end))
	{
		printf("A paramter ('end') has invalid value : '%s'\n", end);
		return false;
	}

	// vaildation number 2 : invalid number (too big or too small integer)
	if (start)
	{
		s = strtoul(start, NULL, 16);
		if (s >= 1048576 || s < 0)
		{
			printf("A paramter ('start') havs wrong address : '%s'\n", start);
			return false;
		}
	}

	// vaildation number 2 : invalid number (too big or too small integer)
	if (end)
	{
		e = strtoul(end, NULL, 16);
		if (e >= 1048576 || e < 0)
		{
			printf("A paramter ('end') has wrong address : '%s'\n", end);
			return false;
		}
	}

	// vaildation number 3 : invalid range
	if (s && e && s > e)
	{
		printf("An invalid range : %d(start) %d(end) \n", s, e);
		return false;
	}

	return true;
}

void adjust_test_case(char *VM)
{
	// copy sample screenshot
	for (int i = 25; i <= 29; i++)
		VM[i] = 0x20;
	for (int i = 30; i <= 39; i++)
		VM[i] = (i - 30) + '0';
	for (int i = 40; i <= 44; i++)
		VM[i] = 0x20;
	VM[45] = '-';
	VM[46] = '=';
	VM[47] = '+';
	VM[48] = '[';
	VM[49] = ']';
	VM[50] = '{';
	VM[51] = '}';

	for (int i = 52; i <= 58; i++)
		VM[i] = 0x20;

	for (int i = 59; i < 59 + strlen(test_text); i++)
		VM[i] = test_text[i - 59];
	for (int i = 59 + strlen(test_text); i < 999; i++)
		VM[i] = '.';
}

char _get_ascii(char value)
{
	if (value > 0x7E || value < 20)
		return '.';
	else
		return value;
}

// no start/end parameters
void _show_lines(char *VM, int start_num, int end_num)
{
	if (is_debug_mode())
		printf("[DEBUG] start/end : %d %d \n", start_num, end_num);
	int offset;
	int cur_idx;
	for (int line = (start_num) / 16; line <= (end_num) / 16; line++)
	{
		offset = line * 16;
		if (offset > 0xFFFFF)
			return;

		printf("%05X ", offset);

		for (int idx = 0; idx < 16; idx++)
		{
			cur_idx = idx + offset;
			if (cur_idx < start_num || cur_idx > end_num)
				printf("   ");
			else
				printf("%X%X ", ((VM[cur_idx] & 0xF0) >> 4), (VM[cur_idx] & 0xF));
		}

		printf("; ");

		for (int idx = 0; idx < 16; idx++)
		{
			cur_idx = idx + offset;
			if (cur_idx < start_num || cur_idx > end_num)
				printf(".");
			else
				printf("%c", _get_ascii(VM[cur_idx]));
		}

		printf("\n");
	}
}

char *init_vm()
{
	char *VM;
	VM = (char *)malloc(sizeof(char) * MEMORY_SIZE);

	for (int i = 0; i < MEMORY_SIZE; i++)
	{
		VM[i] = '\0';
	}

	if (is_debug_mode())
		adjust_test_case(VM);

	return VM;
}

void _get_range(char *start, char *end, int *start_num, int *end_num)
{
	if (start)
		*start_num = strtoul(start, NULL, 16);
	else
	{
		*start_num = dump_global_offset;
		dump_global_offset += 160;
		if (dump_global_offset > 0xFFFFF)
			dump_global_offset = 0;
	}
	if (end)
		*end_num = strtoul(end, NULL, 16);
	else
		*end_num = *start_num + 159;
}

void dump(char *VM)
{
	char *start = strtok(NULL, ",");
	char *end = strtok(NULL, " ");
	int start_num, end_num;

	bool pass_validation = _validate_input(start, end);
	if (!pass_validation)
	{
		flag_global = false;
		return;
	}

	_get_range(start, end, &start_num, &end_num);
	if (is_debug_mode())
		printf("[DEBUG] start, end, start_num, end_num, start_num, end_num : \n[DEBUG] %s, %s, %d, %d, %X, %X\n", start, end, start_num, end_num, start_num, end_num);

	_show_lines(VM, start_num, end_num);
}
