#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define MEMORY_SIZE 1048576
char VM[MEMORY_SIZE];
int dump_idx = 0;

bool _is_valid_interger(char *input)
{
	int length = strlen(input);
	for (int i = 0; i < length; i++)
		if (!isdigit(input[i]))
		{
			return false;
		}
	return true;
}

bool _validate_input(char *start, char *end)
{
	// vaildation number 1 : invalid paramter (not right number)
	int s, e;
	if (start && !_is_valid_interger(start))
	{
		printf("A paramter ('start') has invalid value : %s\n", start);
		return false;
	}
	if (end && !_is_valid_interger(end))
	{
		printf("A paramter ('end') has invalid value : %s\n", end);
		return false;
	}

	// vaildation number 2 : invalid number (too big integer)
	if (start && (s = atoi(start)) >= 65535)
	{
		printf("A paramter ('start') has invalid integer : %s\n", start);
		return false;
	}
	if (end && (e = atoi(end)) >= 65535)
	{
		printf("A paramter ('end') has invalid integer : %s\n", end);
		return false;
	}

	// vaildation number 3 : invalid range
	if (start && end && s > e)
	{
		printf("A invalid range : %d(start) %d(end) \n", s, e);
		return false;
	}

	return true;
}

void _show_lines(int start_num, int end_num)
{
	int idx;
	printf("[DEBUG] start_num, end_num : %d %d\n", start_num, end_num);
	for (idx = start_num - 16; idx < MEMORY_SIZE; idx++)
	{
		if (idx < start_num)
			continue;
		else if (idx > end_num)
			break;

		printf("! ");
	}
	dump_idx = idx;
}

void dump()
{
	char *start = strtok(NULL, ",");
	char *end = strtok(NULL, " ");
	int start_num = dump_idx, end_num = dump_idx + 159;

	printf("[DEBUG] start, end : %s %s\n", start, end);
	bool pass_validation = _validate_input(start, end);
	if (!pass_validation)
		return;

	if (start)
		start_num = atoi(start);
	if (end)
		end_num = atoi(end);
	_show_lines(start_num, end_num);
}
