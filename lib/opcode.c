#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "opcode.h"

#define HASH_SIZE 20

static hash_block hash_table[HASH_SIZE];
static bool _validate_input(char *mnemonic, int *opcode);
static int _get_hash(const char *mnemonic);
static void _add_hash(int code, char *mnemonic, char *ic);
static hash_block _find_opcode(char *mnemonic);

void init_opcode(void) {
  FILE *fp;
  int code;
  char mnemonic[5], avail_format[10];
  fp = fopen("./lib/opcode.txt", "r");
  while (!feof(fp)) {
    fscanf(fp, "%x %s %s", &code, mnemonic, avail_format);
    _add_hash(code, mnemonic, avail_format);
  }
}

hash_block opcode_s(char *mnemonic) {
  return _find_opcode(mnemonic);
}

int opcode(char *mnemonic) {
  int opcode;

  if (!_validate_input(mnemonic, &opcode))
    return 1;

  printf("opcode is %X\n", opcode);
  return 0;
}

int opcodelist(void) {
  int line;
  hash_block block;
  int first_block;

  for (line = 0; line < HASH_SIZE; line++) {
    first_block = 1;
    printf("%d : ", line);
    block = hash_table[line];
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

bool _validate_input(char *mnemonic, int *opcode) {
  // 1 : Insufficient parameter
  if (!mnemonic) {
    printf("Insufficient parameter (mnemonic) : '%s'\n", mnemonic);
    return false;
  }

  *opcode = _find_opcode(mnemonic)->code;

  // 2 : No corresponding opcode to the given value
  if (*opcode == -1) {
    printf("No corresponding opcode to the given value (mnemonic) : '%s'\n", mnemonic);
    return false;
  }

  return true;
}

int _get_hash(const char *mnemonic) {
  int i;
  int mul = 1, hash = 0;
  for (i = 0; mnemonic[i] != 0; i++) {
    hash = ((mnemonic[i] * mul) + hash) % HASH_SIZE;
    mul *= 17;
  }
  if (hash < 0)
    hash = -hash;
  return hash % HASH_SIZE;
}

void _add_hash(int code, char *mnemonic, char *avail_format) {
  hash_block new_block, target_block;
  new_block = (hash_block) malloc(sizeof(struct _hash_block));
  new_block->code = code;
  strcpy(new_block->format, avail_format);
  strcpy(new_block->mnemonic, mnemonic);
  new_block->next_block = NULL;

  target_block = hash_table[_get_hash(mnemonic)];
  if (!target_block)
    hash_table[_get_hash(mnemonic)] = new_block;
  else {
    while (target_block->next_block)
      target_block = target_block->next_block;
    target_block->next_block = new_block;
  }
}

hash_block _find_opcode(char *mnemonic) {
  hash_block target_block;
  target_block = hash_table[_get_hash(mnemonic)];
  while (target_block) {
    if (strcmp(target_block->mnemonic, mnemonic) == 0) {
      break;
    }
    target_block = target_block->next_block;
  }
  return target_block;
}