#ifndef OPCODE_H
#define OPCODE_H

/**
 * OPTAB (Opcode Table) structure
 */
typedef struct _optab_node *optab_node;
struct _optab_node {
  int code;
  char mnemonic[20];
  char format[10];
  optab_node next_block;
};

/**
 * Make a optab with a given file
 */
void init_opcode(char *filename);

/**
 * Get target which has a given mnemonic
 */
optab_node get_optab_node(char *mnemonic);

/**
 * Show opcode number of a given mnemonic
 */
int opcode(char *mnemonic);

/**
 * Show full of opcode table
 */
int opcodelist(void);

#endif
