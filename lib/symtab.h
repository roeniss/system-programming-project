#ifndef SYMTAB_H
#define SYMTAB_H

#define MAX_SYMBOL_SIZE 30

/**
 * SYMTAB (Symbol Table) structure
 */
typedef struct _symtab_node *symtab_node;
struct _symtab_node {
  char symbol[MAX_SYMBOL_SIZE];
  int address;
  symtab_node next_node;
};

/**
 * Assign new table after removing existing table
 */
void _init_symtab(void);

/**
 * Add new symbol with address to symtab
 */
void _add_symbol(char *symbol, int address);

/**
 * Return symtab node corresponding to a given symbol
 */

symtab_node find_symbol(char *symbol);

/**
 * Show full of symbol table
 */
int show_symbol(void);

#endif
