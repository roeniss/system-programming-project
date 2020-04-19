#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "optab.h"
#include "symtab.h"

#define READ_LINE_LENGTH 255
#define MAX_MODIFICATION_RECORD 100

static FILE *fp_asm, *fp_itm, *fp_lst, *fp_obj;
static char *symbol, *order, *operand, line[READ_LINE_LENGTH];
static int line_no, loc, PC, program_length;
static int regB = -1; // B register
static int r1, r2; // pass2: format 2 --> register 1, 2
static char *first_operand, *second_operand; // pass2: format 3/4 --> operand 1 [,2]
static optab_node op;
static symtab_node sym;
static char *placeHolder = " "; // for fprintf

// Modification Record
static int m_record_node[MAX_MODIFICATION_RECORD];
static int m_record_cnt = 0;

/**
 * 공통 요소
 */
static void _open_file_with_ext(FILE **fp, char *name, char ext[4], char mode[2]);
static void _delete_file_with_ext(char *name, char ext[4]);

/**
 * Pass 1 관련
 */
static int _parse_asm_line();
static int _pass1();

/**
 * Pass 2 관련
 */
static int _parse_itm_line();
static int _pass2();
static int _convertStrToRegId(char *target);
static int _charToHex(char target);
static void _find_r1r2();
void _split_operands();

int assemble(char *file) {
  char original_file[255];
  strcpy(original_file, file);

  fp_asm = fopen(file, "r");
  if (!fp_asm) {
    // Error Handling : FILE_NOT_FOUND
    printf("There is no such file\n");
    goto ERROR_HANDLING;
  }
  _open_file_with_ext(&fp_itm, file, "itm", "w");
  _init_symtab();

  int error = _pass1(file);
  if (error) goto ERROR_HANDLING;

  fclose(fp_itm);
  fclose(fp_asm);

  _open_file_with_ext(&fp_itm, file, "itm", "r");
  _open_file_with_ext(&fp_obj, file, "obj", "w");
  _open_file_with_ext(&fp_lst, file, "lst", "w");

  error = _pass2(file);
  if (error) goto ERROR_HANDLING;

  // close all files
  if (fp_itm) fclose(fp_itm);
  if (fp_obj) fclose(fp_obj);
  if (fp_lst) fclose(fp_lst);

  printf("\x1b[32m" "Successfully " "\x1b[0m" "assemble %s.\n", original_file);

  return 0;

  ERROR_HANDLING:

  // close all files
  if (fp_asm) fclose(fp_asm);
  if (fp_itm) fclose(fp_itm);
  if (fp_obj) fclose(fp_obj);
  if (fp_lst) fclose(fp_lst);

  // remove every generated files
  _delete_file_with_ext(file, "itm");
  _delete_file_with_ext(file, "obj");
  _delete_file_with_ext(file, "lst");
  return 1;
}

static void _open_file_with_ext(FILE **fp, char *name, char ext[4], char mode[2]) {
  for (int i = 1; i <= 3; ++i)
    name[strlen(name) - i] = ext[3 - i];
  *fp = fopen(name, mode);
}

void _delete_file_with_ext(char *name, char ext[4]) {
  for (int i = 1; i <= 3; ++i)
    name[strlen(name) - i] = ext[3 - i];
  remove(name);
}

int _parse_asm_line() {
  const char *WORD = " \t", *LINE = "\0";

  if (fgets(line, sizeof(line), fp_asm) == NULL) return 1;
  else if (line[0] == '\n') return 1; // consider as EOF

  line[strlen(line) - 1] = '\0'; // delete '\n'

  // Step 1 : Get symbol (1st slot)
  if (line[0] == '.') {
    // comment line
    symbol = line;
    return 0;
  } else if (line[0] == ' ') {
    // no symbol
    symbol = NULL;
  } else {
    // symbol
    symbol = strtok(line, WORD);
  }

  // Step 2 : Get order (opcode or directive) (2nd slot)
  if (symbol) order = strtok(NULL, WORD);
  else order = strtok(line, WORD);

  // Step 3 : Get operand (3rd slot)
  operand = strtok(NULL, LINE);
  while (operand && *operand == ' ' && *(operand + 1) != '\0') operand += 1;
  if (operand && operand[0] == ' ') operand = NULL;

  return 0;
}

/**
 * Make symbol table with adequate Loc, write intermediate file
 */
int _pass1() {
  line_no = 5, loc = 0, PC = 0;

  bool error = _parse_asm_line();
  if (error) return 1;
  //fprintf(fp_itm, "%-10s %-10s %-10s %-10s %-10s %-10s \n", "LINE", "loc", "PC", "SYMBOL", "OPCODE", "OPERAND");

  if (order && !strcmp(order, "START")) {
    if (operand) loc = strtol(operand, NULL, 10);
    program_length = loc;
  } else {
    fseek(fp_asm, 0, SEEK_SET);
    symbol = "A"; // default program name
    order = "START";
    operand = "0";
  }

  PC = loc;
  fprintf(fp_itm, "%-10d %-10d %-10s %-10s %-10X \n", line_no, loc, symbol, order, (int) strtol(operand, NULL, 16));

  // parse the rest lines until meet 'END' or EOF
  while (1) {
    line_no += 5;
    loc = PC;
    error = _parse_asm_line();
    if (error) {
      // Error Handling : FAIL_TO_READ_LINE
      printf("Line %d : Fail to read line", line_no);
      return 1;
    }

    if (order && !strcmp(order, "END")) break;

    if (!(symbol && symbol[0] == '.')) {
      // not comment line
      if (symbol) {
        // symbol?: YES
        if (find_symbol(symbol)) {
          // Error Handling : DUPLICATED_SYMBOL
          printf("Line %d : Duplicated symbol declaration of '%s'\n", line_no, symbol);
          return 1;
        }
        _add_symbol(symbol, PC);
      }
      // ~ symbol?: YES

      if (!order) {
        // Error Handling : NO_OPCODE_OR_DIRECTIVE
        printf("Line %d : No opcode or directive\n", line_no);
        return 1;
      }

      // find opcode
      if (order[0] == '+') { // extended format (format 4)
        PC++;
        op = get_optab_node(order + 1);
      } else {
        op = get_optab_node(order);
      }

      if (op) { // opcode in OPTAB
        PC += op->format[0] - '0'; // format : '1', '2', '3/4'
      } else if (!strcmp(order, "WORD")) {
        PC += 3;
      } else if (!strcmp(order, "BASE")) {
        // nothing to do
      } else if (!strcmp(order, "RESW")) {
        PC += 3 * (int) strtol(operand, NULL, 10);
      } else if (!strcmp(order, "RESB")) {
        PC += 1 * (int) strtol(operand, NULL, 10);
      } else if (!strcmp(order, "BYTE")) {
        // order example : X'F1', C'EOF', X'FF1'
        int target_cnt = 2;
        if (operand[0] == 'C' || operand[0] == 'c') {
          // Character (1 byte per 1 char)
          while (*(operand + target_cnt) != '\'') {
            target_cnt++;
            PC++;
          }
        } else if (operand[0] == 'X' || operand[0] == 'x') {
          // Half-byte (1 byte per 2 char)
          // Hex 개수를 짝수로 맞춰주는 작업 진행 (ex) X'FF1' -> X'0FF1'
          int cnt = 0;
          while (*(operand + target_cnt) != '\'') {
            cnt++;
            target_cnt++;
          }
          target_cnt--; // 마지막 hex를 보고 있음
          if (cnt % 2 == 1) {
            cnt++;
            while (*(operand + target_cnt) != '\'') {
              operand[target_cnt + 1] = operand[target_cnt];
              target_cnt--;
            }
            operand[target_cnt + 1] = '0';
          }
          PC += cnt / 2;
        } else {
          // Error Handling : INVALID_BYTE_OPERAND
          printf("Line %d : Invalid operand for BYTE directive\n", line_no);
          return 1;
        }
      } else {
        // Error Handling : UNKNOWN_INSTRUCTION
        printf("Line %d : Unknown instruction (opcode or directive)\n", line_no);
        return 1;
      }

      if (!symbol) symbol = "~";
      if (!order) order = "~";
      if (!operand) operand = "~";
      fprintf(fp_itm, "%-10d %-10d %-10s %-10s %-10s \n", line_no, loc, symbol, order, operand);
    } else {
      // comment line
      fprintf(fp_itm, "%-10d %-10d %s\n", line_no, loc, symbol);
    }
  }

  // "END" directives
  if (!symbol) symbol = "~";
  if (!order) order = "~";
  if (!operand) operand = "~";
  fprintf(fp_itm, "%-10d %-10d %-10s %-10s %-10s \n", line_no, loc, symbol, order, operand);

  // save program length
  program_length = PC - program_length;

  return 0;
}

int _parse_itm_line() {
  char *WORD = " \t", *LINE = "\n", *token;

  if (fgets(line, sizeof(line), fp_itm) == NULL) return false;
  else if (line[0] == '\n') return false; // considered as EOF

  // Step 1 : Get line number
  line_no = (int) strtol(strtok(line, WORD), NULL, 10);

  // Step 2 : Get LOC
  token = strtok(NULL, WORD);
  loc = (int) strtol(token, NULL, 10);

  // Step 3 : Get comment line
  char *rest = token + strlen(token) + 1;
  while (*rest == ' ') rest++;
  if (rest[0] == '.') {
    symbol = rest;
    rest[strlen(symbol) - 1] = '\0';
    return true;
  } else {
    symbol = strtok(NULL, WORD);
    if (symbol[0] == '~') symbol[0] = '\0';
  }

  // Step 4 : Get order (opcode or directive)
  order = strtok(NULL, WORD);

  // Step 5 : Get operand
  operand = strtok(NULL, LINE);
  while (*operand == ' ') operand++;
  if (operand[0] == '~') operand = NULL;

  return true;
}

/**
 * write object file and lsting file
 */
int _pass2() {
  unsigned char objCode[255] = {0,};
  int objCodeIdxTo = 0, objCodeIdxFrom, disp, textRecordStart = 0;
  bool forceLineBreak = false;
  _parse_itm_line();

  symbol[6] = '\0';
  fprintf(fp_obj, "H%-6s%06X%06X\n", symbol, (int) strtol(operand, NULL, 16), program_length);
  fprintf(fp_lst, "%-4s     %-4s       %-22s    %s\n", "Line", "Loc", "Source Statement", "Object code");
  fprintf(fp_lst, "%4d     %04X     %-8s %-8s %-8s  \n", line_no, loc, symbol, order, operand);

  textRecordStart = PC = loc;

  while (1) {
    loc = PC;

    // write object file if needed
    if (forceLineBreak || objCodeIdxTo >= 28) {
      if (objCodeIdxTo > 0) {
        fprintf(fp_obj, "T%06X%02X", textRecordStart, objCodeIdxTo);
        for (int i = 0; i < objCodeIdxTo; ++i) {
          fprintf(fp_obj, "%02X", objCode[i]);
          objCode[i] = '\0';
        }
        fprintf(fp_obj, "\n");
      }
      textRecordStart = loc;
      objCodeIdxTo = 0;
      forceLineBreak = false;
    }

    objCodeIdxFrom = objCodeIdxTo;

    // read new line
    _parse_itm_line();
    if (!strcmp(order, "END")) break;

    if (!(symbol && symbol[0] == '.')) {
      // not comment line
      if (order[0] == '+') op = get_optab_node(order + 1);
      else op = get_optab_node(order);
      if (op) {
        // opcode?: YES
        objCode[objCodeIdxTo] = op->code;
        if (operand && operand[0] == '#') {
          // immediate addressing (n bit = 0, i bit = 1)
          objCode[objCodeIdxTo++] += 1;
          operand++;
        } else if (operand && operand[0] == '@') {
          // indirect addressing (n bit 1, i bit = 0)
          objCode[objCodeIdxTo++] += 2;
          operand++;
        } else if ((op->format)[0] == '3') {
          // simple addressing (n bit = 1, i bit = 1)
          objCode[objCodeIdxTo++] += 3;
        } else {
          objCodeIdxTo++;
        }

        if ((op->format)[0] == '1') {
          // format 1 : 8 bit
          PC++;
          goto WRITE_LISTING_LINE;
        } else if ((op->format)[0] == '2') {
          // format 2 : 16 bit
          _find_r1r2();
          objCode[objCodeIdxTo++] = (r1 << 4) + r2;
          PC += 2;
          goto WRITE_LISTING_LINE;
        } else {
          // format 3/4
          _split_operands();
          // get disp
          sym = find_symbol(first_operand);
          if (!sym) {
            // not symbol (absolute value & immediate addressing)
            if (first_operand && ('0' > first_operand[0] || first_operand[0] > '9')) {
              printf("Line %d : Fail to read operand", line_no);
              return 1;
            }
            if (operand) disp = (int) strtol(operand, NULL, 10);
            else {
              disp = 0;
              operand = placeHolder;
            }
            if (order[0] != '+') {
              // format 3
              PC += 3;
              objCode[objCodeIdxTo++] = disp >> 8;
              objCode[objCodeIdxTo++] = disp & 0xFF;
              goto WRITE_LISTING_LINE;
            } else {
              // format 4
              PC += 4;
              objCode[objCodeIdxTo] += 1 << 4;
              objCode[objCodeIdxTo++] += (disp >> 16) & 0xF;
              objCode[objCodeIdxTo++] += (disp >> 8) & 0xFF;
              objCode[objCodeIdxTo++] += disp & 0xFF;
              goto WRITE_LISTING_LINE;
            }
          }
          disp = sym->address;

          if (order[0] != '+') {
            PC = loc + 3;
            // format 3 : 24 bit
            if (second_operand && _convertStrToRegId(second_operand) == 1) {
              // indexed addressing
              objCode[objCodeIdxTo] += (8 << 4);
            }
            if (-2048 <= (disp - PC) && (disp - PC) < 2048) {
              // PC-relative addressing
              int TA = disp - PC;
              objCode[objCodeIdxTo] += (2 << 4);
              objCode[objCodeIdxTo++] += (TA >> 8) & 0xF;
              objCode[objCodeIdxTo++] += (TA & 0xFF);
            } else if (-1 < regB && 0 <= (disp - regB) && (disp - regB) < 4096) {
              // Base-relative addressing
              int TA = disp - regB;
              objCode[objCodeIdxTo] += (4 << 4);
              objCode[objCodeIdxTo++] += (TA >> 8);
              objCode[objCodeIdxTo++] += (TA & 0xFF);
            }
          } else {
            // format 4 : 32 bit
            PC += 4;
            objCode[objCodeIdxTo] += (1 << 4);
            objCode[objCodeIdxTo++] += disp >> 16 & 0xF;
            objCode[objCodeIdxTo++] += (disp >> 8) & 0xFF;
            objCode[objCodeIdxTo++] += disp & 0xFF;

            // add modification record line
            m_record_node[m_record_cnt++] = PC - 3;
          }
        }
      } else {
        // opcode?: NO (directive: Yes)
        operand = strtok(operand, " ");
        if (!strcmp(order, "WORD")) {
          objCode[objCodeIdxTo++] = operand[0];
          objCode[objCodeIdxTo++] = operand[1];
          objCode[objCodeIdxTo++] = operand[2];
          PC += 3;
        } else if (!strcmp(order, "BASE")) {
          sym = find_symbol(operand);
          regB = sym->address;
        } else if (!strcmp(order, "RESB")) {
          int amount = (int) strtol(operand, NULL, 10);
          PC += amount;
          forceLineBreak = true;
        } else if (!strcmp(order, "RESW")) {
          int amount = (int) strtol(operand, NULL, 10);
          PC += amount * 3;
          forceLineBreak = true;
        } else if (!strcmp(order, "BYTE")) {
          int target_cnt = 2;
          if (operand[0] == 'C' || operand[0] == 'c') {
            // Character (1 byte per 1 char)
            while (*(operand + target_cnt) != '\'') {
              objCode[objCodeIdxTo++] = *(operand + target_cnt);
              target_cnt++;
              PC++;
            }
          } else if (operand[0] == 'X' || operand[0] == 'x') {
            // Half-byte (1 byte per 2 char)
            // Hex 개수를 짝수로 맞춰주는 작업 진행 (ex) X'FF1' -> X'0FF1'
            int cnt = 0;
            while (*(operand + target_cnt) != '\'') {
              cnt++;
              target_cnt++;
            }
            target_cnt--; // 마지막 hex를 보고 있음
            if (cnt % 2 == 1) {
              while (*(operand + target_cnt) != '\'') {
                operand[target_cnt + 1] = operand[target_cnt];
                target_cnt--;
              }
              target_cnt++;
              operand[target_cnt] = '0';
            }
            target_cnt = 2;
            while (*(operand + target_cnt) != '\'') {
              int byte = _charToHex(*(operand + target_cnt++));
              int byte2 = _charToHex(*(operand + target_cnt++));
              objCode[objCodeIdxTo++] = (byte << 4) + byte2;
              PC++;
            }
          }
        }
      }
      // common write to listing file
      WRITE_LISTING_LINE:
      fprintf(fp_lst, "%4d     %04X     %-8s %-8s %-8s  ", line_no, loc, symbol, order, operand);
      for (int i = objCodeIdxFrom; i < objCodeIdxTo; ++i) fprintf(fp_lst, "%02X", objCode[i]);
      fprintf(fp_lst, "\n");
    } else {
      // comment?: YES
      fprintf(fp_lst, "%4d              %s\n", line_no, symbol);
    }
  }

  // write rest lines to listing file & object file
  fprintf(fp_lst, "%4d                       %-8s %s\n", line_no, order, operand);

  if (objCodeIdxTo) {
    fprintf(fp_obj, "T%06X%02X", textRecordStart, objCodeIdxTo);
    for (int i = 0; i < objCodeIdxTo; ++i) fprintf(fp_obj, "%02X", objCode[i]);
    fprintf(fp_obj, "\n");
  }

  for (int i = 0; i < m_record_cnt; ++i)
    fprintf(fp_obj, "M%06X05\n", m_record_node[i]);

  int entry_address = operand ? find_symbol(strtok(operand, " "))->address : 0;
  fprintf(fp_obj, "E%06X", entry_address);
  return 0;
}

int _convertStrToRegId(char *target) {
  if (!target) return 0;
  if (target[0] == 'A') return 0;
  else if (target[0] == 'X') return 1;
  else if (target[0] == 'L') return 2;
  else if (target[0] == 'B') return 3;
  else if (target[0] == 'S') return 4;
  else if (target[0] == 'T') return 5;
  else if (target[0] == 'F') return 6;
  else if (target[0] == 'P' && target[1] == 'C') return 8;
  else if (target[0] == 'S' && target[1] == 'W') return 9;
  else return 0;
}

int _charToHex(char target) {
  if ('0' <= target && target <= '9') return target - '0';
  else if ('A' <= target && target <= 'F') return target - 55;
  else return 0;
}

void _find_r1r2() {
  if (!operand) {
    r1 = r2 = 0;
  } else {
    if (strpbrk(",", operand)) {
      r1 = _convertStrToRegId(strtok(operand, ","));
      r2 = _convertStrToRegId(strtok(NULL, " "));
    } else {
      r1 = _convertStrToRegId(strtok(operand, " "));
      r2 = 0;
    }
  }
}

void _split_operands() {
  if (!operand) {
    first_operand = second_operand = NULL;
  } else {
    if (strpbrk(",", operand)) {
      first_operand = strtok(operand, ",");
      second_operand = strtok(NULL, " ");
    } else {
      first_operand = strtok(operand, " ");
      second_operand = NULL;
    }
  }
}
