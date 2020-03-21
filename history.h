#ifndef HISTORY_H
#define HISTORY_H

// make head node
void init_history();

// add one history node
void add_history(char *cmd);

// display all histories from beginning
void show_history();

#endif