#include <stdbool.h>

#ifndef DEBUG_H
#define DEBUG_H

// check main parameter if exist
extern void check_debug_mode(char *[]);

// used to determine whethere print DEBUG message or not
extern bool is_debug_mode();

#endif