#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "history.h"

typedef struct _History_Node *History_Node;
struct _History_Node
{
	char *command;
	History_Node next_node;
};

static History_Node history_head = NULL;
static History_Node history_tail_node = NULL;

void init_history()
{
	if (history_head == NULL)
	{
		history_head = (History_Node)malloc(sizeof(struct _History_Node));
		history_head->command = NULL;
		history_tail_node = history_head;
	}
}

void add_history(char *command)
{
	History_Node new_node;
	new_node = (History_Node)malloc(sizeof(struct _History_Node));
	char *command_copy = (char *)malloc(sizeof(char) * strlen(command));
	strcpy(command_copy, command);
	new_node->command = command_copy;
	new_node->next_node = NULL;
	history_tail_node->next_node = new_node;
	history_tail_node = new_node;
}
int history()
{
	int idx = 1;
	History_Node cur_node;
	for (cur_node = history_head->next_node; cur_node; cur_node = cur_node->next_node)
		printf("%-4d %s\n", idx++, cur_node->command);
	return 0;
}
