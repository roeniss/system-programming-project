#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symtab.h"

static symtab_node _symtab_head = NULL;

void _init_symtab(void) {
  symtab_node next = NULL, cur = _symtab_head;

  for (; cur || next; cur = next) {
    next = cur->next_node;
    free(cur);
  }
  _symtab_head = NULL;
}

void _add_symbol(char *symbol, int address) {
  // malloc new node
  symtab_node new_node = (symtab_node) malloc(sizeof(struct _symtab_node));
  strcpy(new_node->symbol, symbol);
  new_node->address = address;
  new_node->next_node = NULL;

  if (!_symtab_head) // first node
    _symtab_head = new_node;
  else if (strcmp(_symtab_head->symbol, new_node->symbol) >= 1) {
    new_node->next_node = _symtab_head;
    _symtab_head = new_node;
  } else {
    symtab_node cur;
    for (cur = _symtab_head; cur->next_node; cur = cur->next_node) {
      if (strcmp(cur->next_node->symbol, new_node->symbol) >= 1) {
        // new symbol's symbol (label) < current node's symbol
        // --> attach to front of current node
        new_node->next_node = cur->next_node;
        cur->next_node = new_node;
        return;
      }
    }
    cur->next_node = new_node;
  }
}

symtab_node find_symbol(char *symbol) {
  if(!symbol) return NULL;
  for (symtab_node cur = _symtab_head; cur; cur = cur->next_node) {
    if (!strcmp(cur->symbol, symbol)) return cur;
  }
  return NULL;
}

int show_symbol(void) {
  for (symtab_node cur = _symtab_head; cur; cur = cur->next_node) {
    printf("        %-7s %04X\n", cur->symbol, cur->address);
  }
  return 0;
}
