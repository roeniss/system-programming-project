#include <string.h>
#include <stdbool.h>

#include "myUtils.h"

extern char *VM;

void adjust_test_case(void) {
  int i;
  char test_text[100] = "This is sample Program";

  for (i = 25; i <= 29; i++)
    VM[i] = 0x20;
  for (i = 30; i <= 39; i++)
    VM[i] = (char) ((i - 30) + '0');
  for (i = 40; i <= 44; i++)
    VM[i] = 0x20;
  VM[45] = '-';
  VM[46] = '=';
  VM[47] = '+';
  VM[48] = '[';
  VM[49] = ']';
  VM[50] = '{';
  VM[51] = '}';

  for (i = 52; i <= 58; i++)
    VM[i] = 0x20;

  for (i = 59; i < 59 + (int) strlen(test_text); i++)
    strncpy(&VM[i], &(test_text[i - 59]), sizeof(char));
  for (i = 59 + (int) strlen(test_text); i < 999; i++)
    VM[i] = '.';
}

bool is_valid_hex(char *value) {
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
