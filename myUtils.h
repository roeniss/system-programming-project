#ifndef MY_UNILS_H
#define MY_UNILS_H

//
// Buffer-relative
//

// read (buffer - 1) size's stdin
char *ready_command(int);

//
// String-relative
//

// trim left and right of target string
void trim(char *);
// wrapper for strcmp of boolean version
void is_same_str(char *, ...);

#endif