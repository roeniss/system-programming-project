#ifndef OPCODE_H
#define OPCODE_H

//
// Make a hash table for opcodes.
//
void init_opcode(void);

//
// Show corresponding opcode to given mnemonic.
//
int opcode(char *mnemonic);

//
// Show all available opcode
//
int opcodelist(void);

#endif