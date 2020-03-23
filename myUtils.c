#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "myUtils.h"

char *init_buffer(int max_len)
{
	char *buffer = (char *)malloc(sizeof(char) * max_len);
	return buffer;
}

void ready_command(char *buffer, int max_len)
{
	int c;

	printf("sicsim> ");
	// strlen(buffer) become read data size. (maxLen: char array size - 1)
	// (NULL is not counted)
	// but in this case, (max : char array size -2) reason : check below.
	fgets(buffer, max_len, stdin);

	while (buffer[strlen(buffer) - 1] != '\n' && (c = getchar()) != '\n')
		;

	if (strlen(buffer) < 2)
		return;

	// remove last lineberak --> maxLen = strlen(buffer) - 2
	buffer[strlen(buffer) - 1] = '\0';
}

char *lTrim(char *buffer)
{
	char *tBuffer = buffer;
	while (isspace((unsigned char)*tBuffer))
	{
		tBuffer++;
	}
	return tBuffer;
}