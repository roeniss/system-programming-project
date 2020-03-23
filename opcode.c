#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HASH_SIZE 20

typedef struct _hash_block *hash_block;
struct _hash_block
{
	int code;
	char mnemonic[5];
	// char instrunction_cycle[4];
	hash_block next_block;
};

hash_block hash_table[HASH_SIZE];

int _get_hash(char *mne)
{
	int mul = 1, hash = 0;
	for (int i = 0; mne[i] != 0; i++)
	{
		hash = ((mne[i] * mul) + hash) % HASH_SIZE;
		mul *= 17;
	}
	if (hash < 0)
		hash = -hash;
	return hash % HASH_SIZE;
}

void _add_hash(int co, char *mne, char *cycle)
{
	hash_block new_block, target_block;
	new_block = (hash_block)malloc(sizeof(struct _hash_block));
	new_block->code = co;
	strcpy(new_block->mnemonic, mne);
	new_block->next_block = NULL;

	target_block = hash_table[_get_hash(mne)];
	if (!target_block)
		hash_table[_get_hash(mne)] = new_block;
	else
	{
		while (target_block->next_block)
			target_block = target_block->next_block;
		target_block->next_block = new_block;
	}
}

void init_opcode()
{
	FILE *fp;
	int co;
	char mne[5], cycle[5];
	fp = fopen("./opcode.txt", "r");
	while (!feof(fp))
	{
		fscanf(fp, "%x %s %s", &co, mne, cycle);
		_add_hash(co, mne, cycle);
	}
}

int _find_opcode(char *mne)
{
	hash_block target_block;
	int opcode = -1;
	target_block = hash_table[_get_hash(mne)];
	while (target_block)
	{
		if (strcmp(target_block->mnemonic, mne) == 0)
		{
			opcode = target_block->code;
		}
		target_block = target_block->next_block;
	}
	return opcode;
}

void opcode()
{
	char *mne = strtok(NULL, " ");
	int opcode;
	if (!mne)
	{
		printf("Lack of parameter : mnemonic\n");
		return;
	}
	opcode = _find_opcode(mne);
	if (opcode == -1)
	{
		printf("There is no mnemonic named : %s\n", mne);
	}
	else
	{
		printf("opcode is %X\n", opcode);
	}
}

void opcodelist()
{
	hash_block block;
	int first_block;

	for (int line = 0; line < HASH_SIZE; line++)
	{
		first_block = 1;
		printf("%d : ", line);
		block = hash_table[line];
		if (!block)
		{
			printf("(None)\n");
		}
		else
		{
			while (block)
			{
				if (first_block)
					first_block = 0;
				else
					printf(" -> ");
				printf("[ %s, %x ]", block->mnemonic, block->code);
				block = block->next_block;
			}
			printf("\n");
		}
	}
}