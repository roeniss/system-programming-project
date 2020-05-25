#ifndef VM_H
#define VM_H

/**
 * Dump the virtual memory from start to end
 * if omitted, follow inner counter
 */
int dump(char *start, char *end);

/**
 * Update the given address's value to the given value
 */
int edit(char *address, char *value);

/**
 * Fill with the given value from 'start' to 'end' addresses
 */
int fill(char *start, char *end, char *value);

/**
 * Reset the virtual memory
 */
int reset(void);


#endif
