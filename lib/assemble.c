#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "opcode.h"

#define READ_LINE_LENGTH 255
#define MAX_TOKEN_LENGTH 30

/*
 * Symbol Table related variables & functions (declaration)
 */
typedef struct _symtab_node *symtab_node;
struct _symtab_node {
  char symbol[MAX_TOKEN_LENGTH];
  int address;
  symtab_node next_node;
};
static symtab_node symtab_head = NULL;
static void _init_symtab(void);
static void _add_symbol(char *symbol, int address);
static symtab_node _find_symbol(char *symbol);

/*
 * Line Parsing related variables & function (declaration)
 * - char symbol : comment line일 경우 한 라인을 다 읽을 필요가 있어서, 255 칸을 할당
 * - 'order': opcode function과 혼동을 방지하기 위해 이 단어를 선택
 */
static char symbol[READ_LINE_LENGTH], order[MAX_TOKEN_LENGTH], target[MAX_TOKEN_LENGTH];
static bool _parse_asm_line();

/*
 * Assemble related variables & function (declaration)
 * bool base : if(base) Base-relative addressing 가능
 */
static void _init_asm_file(char *file);
static FILE *fp_asm, *fp_itm, *fp_lst, *fp_obj;
static int line_no, LOCCTR, PC, program_len;
static hash_block op;
static bool base = false;
static bool _is_exist(char *token);
static int pass1(char *file_fullname);
static int pass2(char *file_fullname);

/*
 * Intermediate(itm) File related functions (declaration)
 * Instruction file structure: line_no / Loc / Symbol / Order or Comment / Target
 */
static void _init_itm_file(char *file);
static void _write_itm_line();

/*
 * Module Functions
 */
int assemble(char *file_fullname) {

  _init_asm_file(file_fullname);
  if (!fp_asm) {
    // Error Handling : FILE_NOT_FOUND
    printf("There is no such file\n");
    goto ERROR_HANDLING;
  }
  _init_symtab();

  int result = pass1(file_fullname);
  if (result != 0) goto ERROR_HANDLING;

  result = pass2(file_fullname);
  if (result != 0) goto ERROR_HANDLING;

  fclose(fp_asm);

  return 0;

  ERROR_HANDLING:

  // close all files
  if (fp_itm) fclose(fp_itm);
  if (fp_asm) fclose(fp_asm);
  if (fp_lst) fclose(fp_lst);
  if (fp_obj) fclose(fp_obj);

  // remove all generated files
  file_fullname[strlen(file_fullname) - 3] = 'i';
  file_fullname[strlen(file_fullname) - 2] = 't';
  file_fullname[strlen(file_fullname) - 1] = 'm';
  remove(file_fullname);
  file_fullname[strlen(file_fullname) - 3] = 'l';
  file_fullname[strlen(file_fullname) - 2] = 's';
  file_fullname[strlen(file_fullname) - 1] = 't';
  remove(file_fullname);
  file_fullname[strlen(file_fullname) - 3] = 'o';
  file_fullname[strlen(file_fullname) - 2] = 'b';
  file_fullname[strlen(file_fullname) - 1] = 'j';
  remove(file_fullname);

  return 1;

}

int show_symbol(void) {
  for (symtab_node node = symtab_head;
       node;
       node = node->next_node) {
    printf("%15s     %04X\n", node->symbol, node->address);
  }
  return 0;
}

/*
 * Symbol Table related variables & functions (implementation)
 */

//
// 기존에 존재하는 symbol table's nodes 삭제
//
static void _init_symtab(void) {
  symtab_node tmp, node;
  for (node = tmp = symtab_head;
       tmp;
       node = tmp) {
    tmp = node->next_node;
    free(node);
  }
}

void _add_symbol(char *symbol, int address) {
  symtab_node new_node = (symtab_node) malloc(sizeof(struct _symtab_node));
  strcpy(new_node->symbol, symbol);
  new_node->address = address;
  new_node->next_node = NULL;

  if (!symtab_head)
    symtab_head = new_node;
  else if (strcmp(symtab_head->symbol, new_node->symbol) >= 1) {
    // new symbol is smaller than head node
    new_node->next_node = symtab_head;
    symtab_head = new_node;
  } else {
    symtab_node cur_node;
    for (cur_node = symtab_head;
         cur_node->next_node;
         cur_node = cur_node->next_node) {
      if (strcmp(cur_node->next_node->symbol, new_node->symbol) >= 1) {
        // new symbol is now smaller than next node
        new_node->next_node = cur_node->next_node;
        cur_node->next_node = new_node;
        return;
      }
    }
    cur_node->next_node = new_node;
  }
}

symtab_node _find_symbol(char *symbol) {
  for (symtab_node node = symtab_head;
       node;
       node = node->next_node) {
    if (!strcmp(symbol, node->symbol)) return node;
  }
  return NULL;
}

/*
 * Line Parsing related variables & function (implementation)
 */
//
// Return false if fail to read line (ex. EOF)
//
bool _parse_asm_line() {
  const char *TOKEN_WORD = " \t\n", *TOKEN_LINE = "\n";
  char line[READ_LINE_LENGTH], *token;

  if (fgets(line, sizeof(line), fp_asm) == NULL) return false;
  if (line[0] == '\n') return false; // 파일 끝으로 간주

  symbol[0] = order[0] = target[0] = '\0'; // initialize token slots

  if (line[0] == '.') {
    // comment line?: YES
    strcpy(symbol, line);
  } else {
    // comment line?: NO
    if (line[0] != ' ') {
      // symbol?: YES --> symbol & order
      token = strtok(line, TOKEN_WORD);
      strcpy(symbol, token);

      token = strtok(NULL, TOKEN_WORD);
      strcpy(order, token);

    } else {
      // symbol?: NO --> order only
      token = strtok(line, TOKEN_WORD);
      strcpy(order, token);
    }

    token = strtok(NULL, TOKEN_LINE);
    if (token != NULL) {
      // target?: YES
      while (*(token) == ' ') token++; // left-trim
      strcpy(target, token);
    }
  }

  return true;
}

void _init_itm_file(char *file) {
  file[strlen(file) - 3] = 'i';
  file[strlen(file) - 2] = 't';
  file[strlen(file) - 1] = 'm';
  fp_itm = fopen(file, "w");
}

/*
* Assemble related variables & function (implementation)
*/
static bool _is_exist(char *token) {

  return (token && token[0] != '\0');

}

int pass1(char *file_fullname) {
  // Pass 1
  // - Assign addresses using PC indicator
  // - Save label-addresses into symtab(Symbol Table) nodes

  // parse the first line
  _parse_asm_line(fp_asm);

  if (_is_exist(order) && !strcmp(order, "START")) {
    if (_is_exist(target)) PC = (int) strtol(target, NULL, 10);
    else PC = 0;

    LOCCTR = 0;
    program_len = PC; // pass1이 끝날 때 재이용하여 총 길이를 계산
    line_no = 0;

    _init_itm_file(file_fullname);
    fprintf(fp_itm, "%-10s %-10s %-10s %-10s %-10s \n", "LINE", "LOC", "SYMBOL", "OPCODE", "OPERAND");
    _write_itm_line();
  } else {
    // Error Handling : NO_START_DIRECTIVE_ON_FIRST_LINE
    printf("There is no START directive on the first line.\n");
    goto ERROR_HANDLING;
  }

  // parse the rest lines until meet 'END' or EOF
  while (1) {
    _parse_asm_line(fp_asm);
    LOCCTR = PC;
    if (!_is_exist(symbol) && !_is_exist(order)) {
      // Error Handling : FINISH_WITHOUT_END_DIRECTIVE
      printf("There is no END directive before file ends.\n");
      goto ERROR_HANDLING;
    }

    line_no += 5;

    if (_is_exist(order) && !strcmp(order, "END")) {
      // last line
      break;
    }

    if (!(_is_exist(symbol) && symbol[0] == '.')) {
      // not comment line
      if (_is_exist(symbol)) {
        // symbol?: YES
        if (_find_symbol(symbol)) {
          // Error Handling : DUPLICATED_SYMBOL
          printf("Line %d : Duplicated symbol declaration of '%s'\n", line_no, symbol);
          goto ERROR_HANDLING;
        }
        _add_symbol(symbol, PC);
      }

      if (!_is_exist(order)) {
        // Error Handling : NO_OPCODE_OR_DIRECTIVE
        printf("Line %d : No opcode or directive\n", line_no);
        goto ERROR_HANDLING;
      }

      if (order[0] == '+') { // extended format (format 4)
        PC++;
        op = opcode_s(order + 1);
      } else {
        op = opcode_s(order);
      }

      if (op) {
        PC += op->format[0] - '0'; // format : '1', '2', '3/4'
      } else if (!strcmp(order, "WORD")) {
        PC += 3;
      } else if (!strcmp(order, "BASE")) {
        // nothing to do in pass1
      } else if (!strcmp(order, "RESW")) {
        PC += 3 * (int) strtol(target, NULL, 10);
      } else if (!strcmp(order, "RESB")) {
        PC += 1 * (int) strtol(target, NULL, 10);
      } else if (!strcmp(order, "BYTE")) {
        int target_cnt = 2; // target example :  X'F1', C'EOF', ...
        if (target[0] == 'C' || target[0] == 'c') {
          // Character (1 byte per 1 char)
          while (*(target + target_cnt++) != '\'') PC += 1;
        } else if (target[0] == 'X' || target[0] == 'x') {
          // Hex (1 byte per 2 char)
          while (*(target + target_cnt) != '\'') {
            target_cnt += 2;
            PC += 1;
          }
        } else {
          // Error Handling : INVALID_BYTE_OPERAND
          printf("Line %d : Invaild BYTE target\n", line_no);
          goto ERROR_HANDLING;
        }
      } else {
        // Error Handling : UNKNOWN_OPCODE_OR_DIRECTIVE
        printf("Line %d : Unknown opcode or directive\n", line_no);
        goto ERROR_HANDLING;
      }
    }
    _write_itm_line();
  }
  _write_itm_line();

  program_len = PC - program_len;

  fclose(fp_itm);
  return 0;

  ERROR_HANDLING:
  fclose(fp_itm);
  return 1;
}

/*
 * Intermediate(itm) File related functions (implementation)
 */
void _init_asm_file(char *file) {
  fp_asm = fopen(file, "r");
}

//
// Write a line to a given file
// only allow char* parameters
//
void _write_itm_line() {
  if (_is_exist(symbol) && symbol[0] == '.') {
    fprintf(fp_itm, "%-10d %-10X %s", line_no, LOCCTR, symbol);
    return;
  }

  if (!_is_exist(symbol)) symbol[0] = '~', symbol[1] = '\0';
  if (!_is_exist(order)) order[0] = '~', order[1] = '\0';
  if (!_is_exist(target)) target[0] = '~', target[1] = '\0';
  fprintf(fp_itm, "%-10d %-10X %-10s %-10s %-10s \n", line_no, LOCCTR, symbol, order, target);
}