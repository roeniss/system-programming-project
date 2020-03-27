#include <stdbool.h>

#ifndef DEBUG_H
#define DEBUG_H

//
// Check if a parameter named '-d' exists.
// if true, debug mode will be activated.
//
void check_debug_mode(int argc, char *argv[]);

//
// If debug mode is on, it will return true, otherwise false
//
bool is_debug_mode(void);

#endif