#ifndef HISTORY_H
#define HISTORY_H

//
// Make head node of command history nodes.
//
void init_history(void);

//
// Add a history node with the given command.
//
void add_history(char *command);

//
// Show all past commands from first one.
//
int history(void);

#endif