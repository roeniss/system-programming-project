#include <string.h>
#include <stdbool.h>

extern char *VM;

void adjust_test_case()
{
	char test_text[100] = "This is sample Program";

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
		strncpy(&VM[i], &(test_text[i - 59]), sizeof(char));
	for (int i = 59 + strlen(test_text); i < 999; i++)
		VM[i] = '.';
}

bool is_valid_hex(char *value)
{
	if (!value)
		return false;
	int length = strlen(value);
	char target, filter[24] = "-1234567890abcdefABCDEF";
	char *target_addr;
	for (int i = 0; i < length; i++)
	{
		target = value[i];
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