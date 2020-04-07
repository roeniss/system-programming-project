#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "debug.h"
#include "myUtils.h"

extern char *VM;
extern int MEMORY_SIZE;

static bool _validate_input(char *address, char *value, unsigned long *addr, unsigned long *val);

int edit(char *address, char *value) {
    unsigned long addr, val;

    if (is_debug_mode())
        printf("[DEBUG] address : %s, value : %s\n", address, value);

    if (!_validate_input(address, value, &addr, &val))
        return 1;

    VM[addr] = (char) val;

    return 0;
}

bool _validate_input(char *address, char *value, unsigned long *addr, unsigned long *val) {

    // 1 : Insufficient parameter
    if (!address) {
        printf("Insufficient parameter (address) : '%s'\n", address);
        return false;
    }
    if (!value) {
        printf("Insufficient parameter (value) : '%s'\n", value);
        return false;
    }

    // 2 : Invalid parameter
    if (!is_valid_hex(address)) {
        printf("Invalid parameter (address) : '%s'\n", address);
        return false;
    }
    if (!is_valid_hex(value)) {
        printf("Invalid parameter (value) : '%s'\n", value);
        return false;
    }

    *addr = strtoul(address, NULL, 16);
    *val = strtoul(value, NULL, 16);

    // 3 : Unreachable address
    if (*addr < 0 || MEMORY_SIZE <= *addr) {
        printf("Unreachable address (address) : '%s'\n", address);
        return false;
    }

    // 4 : Unwritable address
    if (*val < 0 || 0xFF < *val) {
        printf("Unwritable value (value) : '%s'\n", value);
        return false;
    }

    return true;
}