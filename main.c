#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define MAX_PROMPT_INPUT 100

char *ready_command(int);
void execute_command(char *);
void is_same_str(char *, ...);

int main()
{
	char *input;

	while (1)
	{
		input = ready_command(MAX_PROMPT_INPUT);
		// input = trim(input);

		execute_command(input);
	}
	return 0;
}

// read (input - 1) size's stdin
char *ready_command(int max_len)
{
	int c;
	char *input;
	input = (char *)malloc(max_len * sizeof(char));
	printf("sicsim > ");
	// strlen(input) become read data size. (maxLen: char array size - 1)
	// (NULL is not counted)
	// but in this case, (max : char array size -2) reason : check below.
	fgets(input, max_len, stdin);
	while (input[strlen(input) - 1] != '\n' && (c = getchar()) != '\n')
		;
	input[strlen(input) - 1] = '\0'; // remove last lineberak --> maxLen = strlen(input) - 2
	return input;
}
void execute_command(char *input)
{
	if (strcmp(input, "help") == 0 || strcmp(input, "h") == 0)
	{
		// printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\n");
	}
}
