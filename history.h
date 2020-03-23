#ifndef HISTORY_H
#define HISTORY_H

// make head node
extern void init_history();

// add one history node
extern void add_history(char *cmd);

// display all histories from beginning
extern void show_history();

#endif