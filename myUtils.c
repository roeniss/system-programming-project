#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "myUtils.h"

char *ready_command(int max_len)
{
	int c;
	char *buffer;
	buffer = (char *)malloc(max_len * sizeof(char));

	printf("sicsim> ");
	// strlen(buffer) become read data size. (maxLen: char array size - 1)
	// (NULL is not counted)
	// but in this case, (max : char array size -2) reason : check below.
	fgets(buffer, max_len, stdin);

	while (buffer[strlen(buffer) - 1] != '\n' && (c = getchar()) != '\n')
		;

	if (strlen(buffer) < 2)
		return buffer;

	// remove last lineberak --> maxLen = strlen(buffer) - 2
	buffer[strlen(buffer) - 1] = '\0';
	return buffer;
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