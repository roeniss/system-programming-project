#include <stdio.h>
#include <string.h>

char input[100];

void get_input();

int main()
{
	get_input();
	printf("--->%s<---", input);
	get_input();
	printf("--->%s<---", input);
	get_input();
	printf("--->%s<---", input);
	return 0;
}

// read (input - 1) size's stdin
void get_input()
{
	int c;
	printf("sicsim > ");
	// strlen(input) become read data size. (maxLen: char array size - 1)
	// (NULL is not counted)
	// but in this case, (max : char array size -2) reason : check below.
	fgets(input, sizeof(input), stdin);
	while (input[strlen(input) - 1] != '\n' && (c = getchar()) != '\n')
		;
	input[strlen(input) - 1] = '\0'; // remove last lineberak --> maxLen -= 1
}