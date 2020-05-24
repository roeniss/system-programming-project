#ifndef LINKER_H
#define LINKER_H

/**
 * ESTAB (External Symbol Table) structure
 */
typedef struct _estab_node *estab_node;
struct _estab_node {
    int cs_length; // 0: this node is external symbol / 1 or more: this is control section and the value is control section's length
    unsigned int address;
    char symbol[20];
    estab_node next_block;
};

/**
 * set program address to start linker(by `loader` or loader(by 'run')
 */
int progaddr(char *addr);


/**
 * load object file(s)
 */
int loader(char *filename1, char *filename2, char *filename3);

/**
 * add/clear/show breakpoint
 */
int bp(char *addr);

/**
 * run the program starting from current prog_addr
 */
int run(void);
#endif
