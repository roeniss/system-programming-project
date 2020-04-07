#include <stdio.h>
#include <string.h>

#include "debug.h"

static bool debug_mode = false;

void check_debug_mode(int argc, char *argv[]) {
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            printf("[DEBUG] Debug Mode Activated.\n");
            debug_mode = true;
        }
    }
}

bool is_debug_mode(void) {
    return debug_mode;
}