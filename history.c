#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "history.h"

typedef struct _history_node *history_node;
struct _history_node
{
	char *command;
	history_node next_node;
};

history_node history_head_node = NULL;
history_node history_tail_node = NULL;
void init_history()
{
	if (history_head_node == NULL)
	{
		history_head_node = (history_node)malloc(sizeof(struct _history_node));
		history_head_node->command = NULL;
		history_tail_node = history_head_node;
	}
}

void add_history(char *cmd)
{
	history_node new_node;
	new_node = (history_node)malloc(sizeof(struct _history_node));
	char *command_copy = (char *)malloc(sizeof(char) * strlen(cmd));
	strcpy(command_copy, cmd);
	new_node->command = command_copy;
	new_node->next_node = NULL;
	history_tail_node->next_node = new_node;
	history_tail_node = new_node;
}
void show_history()
{
	int idx = 1;
	for (history_node cur_node = history_head_node->next_node; cur_node; cur_node = cur_node->next_node)
	{
		printf("%-4d %s\n", idx++, cur_node->command);
	}
}