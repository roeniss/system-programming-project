extern char *VM;
extern int MEMORY_SIZE;

int reset(void) {
    int i;
    for (i = 0; i < MEMORY_SIZE; i++)
        VM[i] = '\0';
    return 0;
}