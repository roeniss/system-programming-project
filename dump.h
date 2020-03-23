#ifndef DUMP_H
#define DUMP_H

// initialize virtual memory.
extern char *init_vm();

// dump virtual memory
extern void dump();

// check if only input string consists of valid hex characters
extern bool is_valid_hex(char *);

#endif
