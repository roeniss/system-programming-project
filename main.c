#include <stdio.h>

char input[100];

void get_input();

int main()
{
    get_input();

    printf("--->%s<---", input);
    return 0;
}

void get_input()
{
    printf("sicsim > ");
    fgets(input, sizeof(input), stdin);
}
