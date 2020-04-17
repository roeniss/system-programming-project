#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define MEMORY_SIZE 1048576
#define DUMP_COLS 16
#define DUMP_ROWS 10

static char _VM[MEMORY_SIZE];
static int _dump_counter = 0;

static bool _check_and_assign_dump(char *start, char *end, unsigned long *s, unsigned long *e);
static bool _check_and_assign_edit(char *address, char *value, unsigned long *addr, unsigned long *val);
static bool _check_and_assign_fill(char *start, char *end, char *value);
static char _get_ascii(char value);
static bool _is_valid_hex(char *value);

//
// First, check if the given parameters ('start', 'end') are valid.
// - if not valid, dump() will return 1 (error).
// - if 'start' is not given, 'start' point will follow the
//   internal value, '_dump_counter'.
// - if 'start' is given but 'end' is not given,
//   'end' will be set to ('start' + DUMP_COLS * DUMP_ROWS - 1)
//   to show DUMP_COLS * DUMP_ROWS sized block.
//
int dump(char *start, char *end) {
  unsigned long line, idx;
  unsigned long s, e;
  unsigned long offset, cur;

  if (!_check_and_assign_dump(start, end, &s, &e))
    return 1;

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
        printf("%X%X ", ((_VM[cur] & 0xF0) >> 4), (_VM[cur] & 0xF));
    }

    printf("; ");

    for (idx = 0; idx < DUMP_COLS; idx++) {
      cur = idx + offset;
      if (cur < s || cur > e)
        printf(".");
      else
        printf("%c", _get_ascii(_VM[cur]));
    }

    printf("\n");
  }
  return 0;
}

int edit(char *address, char *value) {
  unsigned long addr, val;

  if (!_check_and_assign_edit(address, value, &addr, &val))
    return 1;

  _VM[addr] = (char) val;

  return 0;
}

int fill(char *start, char *end, char *value) {
  unsigned long i;
  unsigned long s, e, val;

  if (!_check_and_assign_fill(start, end, value))
    return 1;

  s = strtoul(start, NULL, 16);
  e = strtoul(end, NULL, 16);
  val = strtoul(value, NULL, 16);

  for (i = s; i <= e; i++)
    _VM[i] = (char) val;

  return 0;
}

int reset(void) {
  for (int i = 0; i < MEMORY_SIZE; i++) _VM[i] = '\0';
  return 0;
}

bool _check_and_assign_dump(char *start, char *end, unsigned long *s, unsigned long *e) {
  // Check 1 : Invalid parameter
  if (start && !_is_valid_hex(start)) {
    printf("Invalid parameter (start) : '%s'\n", start);
    return false;
  } else if (end && !_is_valid_hex(end)) {
    printf("Invalid parameter (end) : '%s'\n", end);
    return false;
  }

  // Assign
  if (start)
    *s = strtoul(start, NULL, 16);
  else {
    *s = (unsigned int) _dump_counter;
    _dump_counter += DUMP_COLS * DUMP_ROWS;
    if (MEMORY_SIZE <= _dump_counter)
      _dump_counter = 0;
  }
  if (end)
    *e = strtoul(end, NULL, 16);
  else
    *e = *s + DUMP_COLS * DUMP_ROWS - 1;


  // Check 2 : Unreachable address
  if (start && (*s < 0 || MEMORY_SIZE <= *s)) {
    printf("Unreachable address (start) : '%s'\n", start);
    return false;
  } else if (end && (*e < 0 || MEMORY_SIZE <= *e)) {
    printf("Unreachable address (end) : '%s'\n", end);
    return false;
  }

  // Check 3 : Wrong range
  if (start && end && *s > *e) {
    printf("Wrong range (start ~ end) : '%s ~ %s'\n", start, end);
    return false;
  }

  return true;
}

bool _check_and_assign_edit(char *address, char *value, unsigned long *addr, unsigned long *val) {

  // Check 1 : Insufficient parameter
  if (!address) {
    printf("Insufficient parameter (address) : '%s'\n", address);
    return false;
  }
  if (!value) {
    printf("Insufficient parameter (value) : '%s'\n", value);
    return false;
  }

  // Check 2 : Invalid parameter
  if (!_is_valid_hex(address)) {
    printf("Invalid parameter (address) : '%s'\n", address);
    return false;
  }
  if (!_is_valid_hex(value)) {
    printf("Invalid parameter (value) : '%s'\n", value);
    return false;
  }

  // Assign
  *addr = strtoul(address, NULL, 16);
  *val = strtoul(value, NULL, 16);

  // Check 3 : Unreachable address
  if (*addr < 0 || MEMORY_SIZE <= *addr) {
    printf("Unreachable address (address) : '%s'\n", address);
    return false;
  }

  // Check 4 : Unwritable address
  if (*val < 0 || 0xFF < *val) {
    printf("Unwritable value (value) : '%s'\n", value);
    return false;
  }

  return true;
}

bool _check_and_assign_fill(char *start, char *end, char *value) {
  unsigned long s, e, val;

  // Check 1 : Insufficient parameter
  if (!start) {
    printf("Insufficient parameter (start) : '%s'\n", start);
    return false;
  } else if (!end) {
    printf("Insufficient parameter (end) : '%s'\n", end);
    return false;
  } else if (!value) {
    printf("Insufficient parameter (value) : '%s'\n", value);
    return false;
  }

  // Check 2 : Invalid parameter
  if (!_is_valid_hex(start)) {
    printf("Invalid parameter (start) : '%s'\n", start);
    return false;
  } else if (!_is_valid_hex(end)) {
    printf("Invalid parameter (end) : '%s'\n", end);
    return false;
  } else if (!_is_valid_hex(value)) {
    printf("Invalid parameter (value) : '%s'\n", value);
    return false;
  }


  // Assign
  s = strtoul(start, NULL, 16);
  e = strtoul(end, NULL, 16);
  val = strtoul(value, NULL, 16);

  // Check 3 : Unreachable address
  if (s < 0 || MEMORY_SIZE <= s) {
    printf("Unreachable address (start) : '%s'\n", start);
    return false;
  } else if (e < 0 || MEMORY_SIZE <= e) {
    printf("Unreachable address (end) : '%s'\n", end);
    return false;
  }

  // Check 4 : Wrong range
  if (s > e) {
    printf("Wrong range (start ~ end) : '%s ~ %s'\n", start, end);
    return false;
  }

  // Check 5 : Unwritable address
  if (val < 0 || 0xFF < val) {
    printf("Unwritable value (value) : '%s'\n", value);
    return false;
  }

  return true;
}

char _get_ascii(char value) {
  if (value > 0x7E || value < 20)
    return '.';
  else
    return value;
}

bool _is_valid_hex(char *value) {
  int i;
  if (!value)
    return false;
  int length = (int) strlen(value);
  char target, filter[24] = "-1234567890abcdefABCDEF";
  char *target_addr;
  for (i = 0; i < length; i++) {
    target = value[i];
    if (i == 0)
      target_addr = strchr(filter, target);
    else
      target_addr = strchr(filter + 1, target);
    if (target_addr == NULL) {
      return false;
    }
  }
  return true;
}
