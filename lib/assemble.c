#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "optab.h" // OPTAB을 참조할 때 사용
#include "symtab.h" // SYMTAB을 참조할 때 사용

#define READ_LINE_LENGTH 255 // asm 파일, itm 파일을 한 줄씩 읽는 array를 선언할 때 사
#define MAX_MODIFICATION_RECORD 100 // Modification record를 담는 array를 선언할 때 사용

static FILE *fp_asm, *fp_itm, *fp_lst, *fp_obj; // 4개의 파일을 각각의 포인터로 관리
static char line[READ_LINE_LENGTH], *symbol, *order, *operand; // 파일 한 줄을 읽는 포인터와 세 칼럼을 보관하는 포인터
static int line_no, loc, PC, program_length; // 좌측 라인 넘버, LOC, PC, (pass1에서 계산되는) 전체 프로그램 길이
static int regB = -1; // B register 값. BASE 지시어가 없으면 사용할 수 없다.
static int r1, r2; // format 2 --> 두 레지스터 값을 찾기 위해 사용
static char *first_operand, *second_operand; // format 3/4 --> operand 1 [,2] 을 보관하는 포인터
static optab_node op; // OPTAB에서 mnemonic에 맞는 opcode 구조체를 가져올 때 사용
static symtab_node sym; // SYMTAB에서 symbol에 맞는 symbol 구조체를 가져올 때 사용
static char *placeHolder = " "; // 파일 출력 시 빈 칸이 '(null)' 텍스트가 뜨는 것을 방지
static int m_record_node[MAX_MODIFICATION_RECORD]; // Modification Record를 보관
static int m_record_cnt = 0; // Modification Record의 갯수를 저장

/**
 * 공통 요소
 */
static void _open_file_with_ext(FILE **fp, char *name, const char ext[4], char mode[2]); // 확장자만 바꿔서 파일 오픈
static void _delete_file_with_ext(char *name, const char ext[4]); // 확장자만 바꿔서 파일 삭제 (에러 발생 시 사용)

/**
 * Pass 1 관련
 */
static int _parse_asm_line(); // asm 파일 1 줄을 읽는다. 성공 시 return 0
static int _pass1(); // 성공 시 return 0

/**
 * Pass 2 관련
 */
static int _parse_itm_line(); // itm(intermediate) 파일 1 줄을 읽는다. 성공 시 return 0
static int _pass2(); // 성공 시 return 0
static int _convertStrToRegId(const char *target); // register 이름을 그에 맞는 int로 변환한다.
static int _charToHex(char target); // '0' ~ '9', 'A' ~ 'F'의 char를 int로 변환한다
static void _find_r1r2(); // format 2 --> operand에서 r1, r2다 (int) 를 찾아낸다
static void _split_operands(); // format 3/4 --> operand에서 first, second operand (char*) 를 찾아낸다

int assemble(char *file) {
  char original_file[255];
  if (!file) {
    // Error Handling : NO_FILE_NAME
    printf("There is no file name\n");
    goto ERROR_HANDLING;
  }

  strcpy(original_file, file);

  fp_asm = fopen(file, "r");
  if (!fp_asm) {
    // Error Handling : FILE_NOT_FOUND
    printf("There is no such file\n");
    goto ERROR_HANDLING;
  }
  _open_file_with_ext(&fp_itm, file, "itm", "w");
  _init_symtab();

  int error = _pass1();
  if (error)
    goto ERROR_HANDLING;

  fclose(fp_itm);
  fp_itm = NULL;
  fclose(fp_asm);
  fp_asm = NULL;

  _open_file_with_ext(&fp_itm, file, "itm", "r");
  _open_file_with_ext(&fp_obj, file, "obj", "w");
  _open_file_with_ext(&fp_lst, file, "lst", "w");

  error = _pass2();
  if (error)
    goto ERROR_HANDLING;

  // close all files
  if (fp_itm) {
    fclose(fp_itm);
    fp_itm = NULL;
  }
  if (fp_obj) {
    fclose(fp_obj);
    fp_obj = NULL;
  }
  if (fp_lst) {
    fclose(fp_lst);
    fp_lst = NULL;
  }

  printf("\x1b[32m" "Successfully " "\x1b[0m" "assemble %s.\n", original_file);

  return 0;

  ERROR_HANDLING:

  // close all files
  if (fp_asm) {
    fclose(fp_asm);
    fp_asm = NULL;
  }
  if (fp_itm) {
    fclose(fp_itm);
    fp_itm = NULL;
  }
  if (fp_obj) {
    fclose(fp_obj);
    fp_obj = NULL;
  }
  if (fp_lst) {
    fclose(fp_lst);
    fp_lst = NULL;
  }

  // remove every generated files
  _delete_file_with_ext(file, "itm");
  _delete_file_with_ext(file, "obj");
  _delete_file_with_ext(file, "lst");
  return 1;
}

static void _open_file_with_ext(FILE **fp, char *name, const char ext[4], char mode[2]) {
  for (int i = 1; i <= 3; ++i) name[strlen(name) - i] = ext[3 - i];
  *fp = fopen(name, mode);
}

void _delete_file_with_ext(char *name, const char ext[4]) {
  for (int i = 1; i <= 3; ++i) name[strlen(name) - i] = ext[3 - i];
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

  int error = _parse_asm_line();
  if (error) {
    // Error Handling : FAIL_TO_READ_LINE
    printf("Line %d : Fail to read line\n", line_no);
    return 1;
  }

  // fprintf(fp_itm, "%-10s %-10s %-10s %-10s %-10s %-10s \n", "LINE", "loc", "PC", "SYMBOL", "OPCODE", "OPERAND");

  // read first line
  if (!(order && !strcmp(order, "START"))) {
    fseek(fp_asm, 0, SEEK_SET);
    symbol = "A"; // default program name
    order = "START";
    operand = "0";
  }
  if (operand) loc = (int) strtol(operand, NULL, 10);
  program_length = loc;
  PC = loc;
  fprintf(fp_itm, "%-10d %-10d %-10s %-10s %-10X \n", line_no, loc, symbol, order, (int) strtol(operand, NULL, 16));

  // parse the rest lines until meet 'END' or EOF
  while (1) {
    line_no += 5;
    loc = PC;
    error = _parse_asm_line();
    if (error) {
      // Error Handling : FAIL_TO_READ_LINE
      printf("Line %d : Fail to read line\n", line_no);
      return 1;
    }

    if (order && !strcmp(order, "END"))
      break;

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

  if (fgets(line, sizeof(line), fp_itm) == NULL) return 1;
  else if (line[0] == '\n') return 1; // considered as EOF

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
    return 0;
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

  return 0;
}

/**
 * write object file and lsting file
 */
int _pass2() {
  unsigned char objCode[255] = {0,};
  int objCodeIdxTo = 0, objCodeIdxFrom, disp, textRecordStart = 0;
  bool forceLineBreak = false, bit_ni = false;
  int error = _parse_itm_line();
  if (error) {
    // Error Handling : FAIL_TO_READ_LINE
    printf("Line %d : Fail to read line\n", line_no);
    return 1;
  }

  symbol[6] = '\0';
  fprintf(fp_obj, "H%-6s%06X%06X\n", symbol, (int) strtol(operand, NULL, 16), program_length);
  // fprintf(fp_lst, "%-4s     %-4s       %-22s    %s\n", "Line", "Loc", "Source Statement", "Object code");
  fprintf(fp_lst, "%4d     %04X     %-8s %-8s %-8s  \n", line_no, loc, symbol, order, operand);

  textRecordStart = PC = loc;

  while (1) {
    loc = PC;
    bit_ni = false;

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
    error = _parse_itm_line();
    if (error) {
      // Error Handling : FAIL_TO_READ_LINE
      printf("Line %d : Fail to read line\n", line_no);
      return 1;
    }

    if (!strcmp(order, "END"))
      break;

    if (!(symbol && symbol[0] == '.')) {
      // not comment line
      if (order[0] == '+') op = get_optab_node(order + 1);
      else op = get_optab_node(order);
      if (op) {
        // opcode?: YES
        objCode[objCodeIdxTo] += (unsigned char) op->code;
        if (operand && operand[0] == '#') {
          // immediate addressing (n bit = 0, i bit = 1)
          objCode[objCodeIdxTo++] += 1;
          operand++;
          bit_ni = true;
        } else if (operand && operand[0] == '@') {
          // indirect addressing (n bit 1, i bit = 0)
          objCode[objCodeIdxTo++] += 2;
          operand++;
          bit_ni = true;
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
          objCode[objCodeIdxTo++] = (unsigned char) ((r1 << 4) + r2);
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
              printf("Line %d : Fail to read operand\n", line_no);
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
              objCode[objCodeIdxTo++] = (unsigned char) (disp >> 8);
              objCode[objCodeIdxTo++] = (unsigned char) (disp & 0xFF);
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
            if (second_operand && _convertStrToRegId(second_operand) == _convertStrToRegId("X")) {
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
          objCode[objCodeIdxTo++] = (unsigned char) operand[0];
          objCode[objCodeIdxTo++] = (unsigned char) operand[1];
          objCode[objCodeIdxTo++] = (unsigned char) operand[2];
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
              objCode[objCodeIdxTo++] = (unsigned char) *(operand + target_cnt);
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
              objCode[objCodeIdxTo++] = (unsigned char) ((byte << 4) + byte2);
              PC++;
            }
          }
        }
      }
      // common write to listing file
      WRITE_LISTING_LINE:
      if (bit_ni) operand--;
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

int _convertStrToRegId(const char *target) {
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
