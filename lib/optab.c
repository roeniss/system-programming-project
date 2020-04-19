#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "optab.h"

#define HASH_SIZE 20

static optab_node optab_hash_table[HASH_SIZE];
static bool _check_and_assign(char *mnemonic, optab_node *target_node); // if no problem, assign node
static void _add_node(int code, char *mnemonic, char *ic);
static int _get_hash(char *mnemonic);

void init_opcode(char *filename) {
  FILE *fp = fopen(filename, "r");
  int code;
  char mnemonic[10], avail_format[10], _code[5];
  while (!feof(fp)) {
    fscanf(fp, "%s %s %s", _code, mnemonic, avail_format);
    code = (int)strtol(_code, NULL, 16);
    _add_node(code, mnemonic, avail_format);
  }
}

int opcode(char *mnemonic) {
  optab_node target_node;

  if (!_check_and_assign(mnemonic, &target_node))
    return 1;

  printf("opcode is %X\n", target_node->code);
  return 0;
}

int opcodelist(void) {
  int line;
  optab_node block;
  int first_block;

  for (line = 0; line < HASH_SIZE; line++) {
    first_block = 1;
    printf("%d : ", line);
    block = optab_hash_table[line];
    if (!block)
      printf("(None)\n");
    else {
      while (block) {
        if (first_block)
          first_block = 0;
        else
          printf(" -> ");
        printf("[%s,%X]", block->mnemonic, block->code);
        block = block->next_block;
      }
      printf("\n");
    }
  }
  return 0;
}

bool _check_and_assign(char *mnemonic, optab_node *target_node) {
  // Check 1 : Insufficient parameter
  if (!mnemonic) {
    printf("Insufficient parameter (mnemonic) : '%s'\n", mnemonic);
    return false;
  }

  // Assign
  *target_node = get_optab_node(mnemonic);

  // Check 2 : No corresponding opcode to the given value
  if (*target_node == NULL) {
    printf("No corresponding opcode to the given value (mnemonic) : '%s'\n", mnemonic);
    return false;
  }

  return true;
}

void _add_node(int code, char *mnemonic, char *avail_format) {
  optab_node new_block, target_block;
  new_block = (optab_node) malloc(sizeof(struct _optab_node));
  new_block->code = code;
  strcpy(new_block->format, avail_format);
  strcpy(new_block->mnemonic, mnemonic);
  new_block->next_block = NULL;

  target_block = optab_hash_table[_get_hash(mnemonic)];
  if (!target_block)
    optab_hash_table[_get_hash(mnemonic)] = new_block;
  else {
    while (target_block->next_block)
      target_block = target_block->next_block;
    target_block->next_block = new_block;
  }
}

int _get_hash(char *mnemonic) {
  int mul = 1, hash = 0;
  for (int i = 0; mnemonic[i] != 0; i++) {
    hash = ((mnemonic[i] * mul) + hash) % HASH_SIZE;
    mul *= 17;
  }
  if (hash < 0) hash = -hash;
  return hash % HASH_SIZE;
}

optab_node get_optab_node(char *mnemonic) {
  optab_node target_block = NULL;
  target_block = optab_hash_table[_get_hash(mnemonic)];
  while (target_block) {
    if (strcmp(target_block->mnemonic, mnemonic) == 0)
      break;
    target_block = target_block->next_block;
  }
  return target_block;
}
