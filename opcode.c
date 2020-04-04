#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "opcode.h"

#define HASH_SIZE 20

typedef struct _hash_block *hash_block;
struct _hash_block
{
	int code;
	char mnemonic[10];
	// char ic[4]; // instruction count
	hash_block next_block;
};

static hash_block hash_table[HASH_SIZE];
static bool _validate_input(char *mnemonic, int *opcode);
static int _get_hash(char *mnemonic);
static void _add_hash(int code, char *mnemonic, char *ic);
static int _find_opcode(char *mnemonic);

void init_opcode(void)
{
	FILE *fp;
	int code;
	char mnemonic[5], ic[5];
	fp = fopen("./opcode.txt", "r");
	while (!feof(fp))
	{
		fscanf(fp, "%x %s %s", &code, mnemonic, ic);
		_add_hash(code, mnemonic, ic);
	}
}

int opcode(char *mnemonic)
{
	int opcode;

	if (!_validate_input(mnemonic, &opcode))
		return 1;

	printf("opcode is %X\n", opcode);
	return 0;
}

int opcodelist(void)
{
	int line;
	hash_block block;
	int first_block;

	for (line = 0; line < HASH_SIZE; line++)
	{
		first_block = 1;
		printf("%d : ", line);
		block = hash_table[line];
		if (!block)
			printf("(None)\n");
		else
		{
			while (block)
			{
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

bool _validate_input(char *mnemonic, int *opcode)
{
	// 1 : Insufficient parameter
	if (!mnemonic)
	{
		printf("Insufficient parameter (mnemonic) : '%s'\n", mnemonic);
		return false;
	}

	*opcode = _find_opcode(mnemonic);

	// 2 : No corresponding opcode to the given value
	if (*opcode == -1)
	{
		printf("No corresponding opcode to the given value (mnemonic) : '%s'\n", mnemonic);
		return false;
	}

	return true;
}

int _get_hash(char *mnemonic)
{
	int i;
	int mul = 1, hash = 0;
	for (i = 0; mnemonic[i] != 0; i++)
	{
		hash = ((mnemonic[i] * mul) + hash) % HASH_SIZE;
		mul *= 17;
	}
	if (hash < 0)
		hash = -hash;
	return hash % HASH_SIZE;
}

void _add_hash(int code, char *mnemonic, char *ic)
{
	hash_block new_block, target_block;
	new_block = (hash_block)malloc(sizeof(struct _hash_block));
	new_block->code = code;
	strcpy(new_block->mnemonic, mnemonic);
	new_block->next_block = NULL;

	target_block = hash_table[_get_hash(mnemonic)];
	if (!target_block)
		hash_table[_get_hash(mnemonic)] = new_block;
	else
	{
		while (target_block->next_block)
			target_block = target_block->next_block;
		target_block->next_block = new_block;
	}
}

int _find_opcode(char *mnemonic)
{
	hash_block target_block;
	int opcode = -1;
	target_block = hash_table[_get_hash(mnemonic)];
	while (target_block)
	{
		if (strcmp(target_block->mnemonic, mnemonic) == 0)
			opcode = target_block->code;
		target_block = target_block->next_block;
	}
	return opcode;
}