#ifndef ASSEMBLE_H
#define ASSEMBLE_H

//
// Make Object file(.obj) and Listing file(.lst).
//
int assemble(char *file);

//
// Show symbol table(SYMTAB) of last assembled file.
//
int show_symbol(void);

#endif