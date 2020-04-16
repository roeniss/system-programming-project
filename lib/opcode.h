#ifndef OPCODE_H
#define OPCODE_H

//
// struct declaration
//
typedef struct _hash_block *hash_block;
struct _hash_block {
  int code;
  char mnemonic[20];
  char format[10];
  hash_block next_block;
};

//
// Make a hash table for opcodes.
//
void init_opcode(void);

//
// Silently return corresponding opcode to given mnemonic.
// If fail to find, return -1.
//
hash_block opcode_s(char *mnemonic);

//
// Show corresponding opcode to given mnemonic.
//
int opcode(char *mnemonic);

//
// Show all available opcode
//
int opcodelist(void);

#endif