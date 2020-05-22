#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 1048576

static int progAddr = 0x00;

int progaddr(char *addr) {
    if (!addr) {
        printf("Error: No address value\n");
        return 1;
    }
    int address = (int) strtoul(addr, NULL, 16);
    progAddr = address;
    progAddr = address;
    printf("%d", progAddr);
    if (address < 0 || MEMORY_SIZE < address) {
        printf("Error: Unavailable address value\n");
        return 1;
    }
    return 0;
}

int getProgAddr(void){
    return progAddr;
}