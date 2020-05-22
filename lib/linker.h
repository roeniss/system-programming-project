#ifndef LINKER_H
#define LINKER_H

/**
 * set program address to start linker(by `loader` or loader(by 'run')
 */
int progaddr(char *addr);

/**
 * get program starting address
 */
int getProgAddr(void);

#endif
