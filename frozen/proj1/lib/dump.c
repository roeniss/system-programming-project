#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include "debug.h"
#include "myUtils.h"

extern int MEMORY_SIZE;
extern int DUMP_COLS;
extern int DUMP_ROWS;
extern char *VM;

static int global_dump_offset = 0;

static bool _validate_input(char *start, char *end);

static void _convert_to_number(char *start, char *end, unsigned long *s, unsigned long *e);

static char _get_ascii(char value);

//
// First, check if the given parameters ('start', 'end') are valid or not.
// if invalid, dump() will return 1 (error) with printf().
// if 'start' is not given, 'start' point will follow
// internal value, 'global_dump_offset'.
// if 'start' is given but 'end' is not given,
// 'end' will be set to ('start' + DUMP_COLS * DUMP_ROWS - 1)
//
int dump(char *start, char *end) {
    unsigned long line, idx;
    unsigned long s, e;
    unsigned long offset, cur;

    if (!_validate_input(start, end))
        return 1;

    _convert_to_number(start, end, &s, &e);

    if (is_debug_mode())
        printf("[DEBUG] start, end, start_num, end_num, start_num_hex, end_num_hex : \n[DEBUG] %s, %s, %ld, %ld, %X, %X\n",
               start, end, s, e, (unsigned int) s, (unsigned int) e);

    for (line = s / DUMP_COLS; line <= e / DUMP_COLS; line++) {
        offset = line * DUMP_COLS;
        if (offset >= MEMORY_SIZE)
            break;

        printf("%05X ", (unsigned int) offset);

        for (idx = 0; idx < DUMP_COLS; idx++) {
            cur = idx + offset;
            if (cur < s || cur > e)
                printf("   ");
            else
                printf("%X%X ", ((VM[cur] & 0xF0) >> 4), (VM[cur] & 0xF));
        }

        printf("; ");

        for (idx = 0; idx < DUMP_COLS; idx++) {
            cur = idx + offset;
            if (cur < s || cur > e)
                printf(".");
            else
                printf("%c", _get_ascii(VM[cur]));
        }

        printf("\n");
    }
    return 0;
}

bool _validate_input(char *start, char *end) {
    // 1 : Invalid parameter
    unsigned long s, e;
    if (start && !is_valid_hex(start)) {
        printf("Invalid parameter (start) : '%s'\n", start);
        return false;
    }
    if (end && !is_valid_hex(end)) {
        printf("Invalid parameter (end) : '%s'\n", end);
        return false;
    }

    // 2 : Unreachable address
    if (start && (s = strtoul(start, NULL, 16)) && (s < 0 || MEMORY_SIZE <= s)) {
        printf("Unreachable address (start) : '%s'\n", start);
        return false;
    }

    if (end && (e = strtoul(end, NULL, 16)) && (e < 0 || MEMORY_SIZE <= e)) {
        printf("Unreachable address (end) : '%s'\n", end);
        return false;
    }

    // 3 : Wrong range
    if (start && end && s > e) {
        printf("Wrong range (start ~ end) : '%s ~ %s'\n", start, end);
        return false;
    }

    return true;
}

void _convert_to_number(char *start, char *end, unsigned long *s, unsigned long *e) {
    if (start)
        *s = strtoul(start, NULL, 16);
    else {
        *s = (unsigned int) global_dump_offset;
        global_dump_offset += DUMP_COLS * DUMP_ROWS;
        if (MEMORY_SIZE <= global_dump_offset)
            global_dump_offset = 0;
    }
    if (end)
        *e = strtoul(end, NULL, 16);
    else
        *e = *s + DUMP_COLS * DUMP_ROWS - 1;
}

char _get_ascii(char value) {
    if (value > 0x7E || value < 20)
        return '.';
    else
        return value;
}