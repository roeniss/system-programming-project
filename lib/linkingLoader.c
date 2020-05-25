#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linkingLoader.h"

#define MEMORY_SIZE 1048576

extern unsigned char VM[MEMORY_SIZE];
static unsigned int prog_addr = 0x00, prog_end_addr = 0x00;
static int bps[MEMORY_SIZE]; // 0: off, 1: on
static int REGS[10];
static char CC;
static int bp_done = 0; // check if current breakpoint was handled or not yet


static estab_node _estab_head = NULL;

void _init_estab(void);

int _convert_string_to_hex(char *str);

void _add_estab_node(int cs_length, unsigned int address, char *symbol);

void _print_regs(void);

estab_node _find_estab_node(char *symbol);

void _execOP(unsigned int OP, unsigned int N, unsigned int I, unsigned int X, unsigned int B, int TA);

int progaddr(char *addr) {
    // Error handling
    if (!addr) {
        printf("Error: No address value\n");
        return 1;
    }

    // parse hex
    prog_addr = _convert_string_to_hex(addr);
    if (MEMORY_SIZE < prog_addr) {
        printf("Error: Unavailable address value\n");
        return 1;
    }

    return 0;
}

int loader(char *filename1, char *filename2, char *filename3) {
    int file_num = 0;
    FILE *fps[3];
    if (filename1) {
        file_num++;
        fps[0] = fopen(filename1, "r");
    }
    if (filename2) {
        file_num++;
        fps[1] = fopen(filename2, "r");
    }
    if (filename3) {
        file_num++;
        fps[2] = fopen(filename3, "r");
    }

    // Error handling
    if (file_num == 0) {
        printf("Error: At least one file should be specified\n");
        return 1;
    } else if ((file_num > 0 && !fps[0]) || (file_num > 1 && !fps[1]) || (file_num > 2 && !fps[2])) {
        printf("Error: Given file(s) doesn't exist\n");
        return 1;
    }

    // initialize estab
    _init_estab();

    unsigned int PROGADDR = prog_addr, CSADDR = prog_addr;
    char line[300];

    // Pass 1
    for (int i = 0; i < file_num; ++i) {
        FILE *fp = fps[i];
        int CSLTH = 0;

        while (1) {
            fgets(line, sizeof(line), fp);
            if (line[strlen(line)] == '\n') {
                line[strlen(line) - 1] = '\0'; // delete '\n'
            }
            if (line[0] == 'H') {
                char prog_name[7], start_addr[7], end_addr[7];
                memset(prog_name, '\0', sizeof prog_name);
                memset(start_addr, '\0', sizeof start_addr);
                memset(end_addr, '\0', sizeof end_addr);
                sscanf(line + 1, "%6c", prog_name);
                sscanf(line + 1 + 6, "%6c", start_addr);
                sscanf(line + 1 + 6 * 2, "%6c", end_addr);
                CSLTH = _convert_string_to_hex(end_addr) - _convert_string_to_hex(start_addr);
                _add_estab_node(CSLTH, CSADDR, prog_name);
            } else if (line[0] == 'E') {
                CSADDR += CSLTH;
                break;
            } else if (line[0] == 'D') {
                char es_name[7], es_addr[7];
                int es_cnt = ((int) strlen(line) - 1) / 12;
                for (int j = 0; j < es_cnt; ++j) {
                    memset(es_name, '\0', sizeof es_name);
                    memset(es_addr, '\0', sizeof es_addr);
                    sscanf(line + 1 + j * 12, "%6c", es_name);
                    sscanf(line + 1 + 6 + j * 12, "%6c", es_addr);
                    _add_estab_node(0, CSADDR + _convert_string_to_hex(es_addr), es_name);
                }
            } else { // line[0] == ('.' || 'T' || 'M' || 'R')
                continue;
            }
        }
    }

    // print linking result
    printf("control symbol address length\nsection name\n--------------------------------\n");
    estab_node cur_node = _estab_head;
    while (cur_node) {
        if (cur_node->cs_length > 0) {
            // control section
            printf("%-16s%-8X%04X\n", cur_node->symbol, cur_node->address, cur_node->cs_length);
        } else {
            // external symbol
            printf("        %-8s%-8X\n", cur_node->symbol, cur_node->address);
        }
        cur_node = cur_node->next_block;
    }
    printf("-------------------------------\n           total length %04X\n", CSADDR - PROGADDR);

    // Pass 2
    /*PROGADDR = prog_addr, */ CSADDR = prog_addr;
    int EXECADDR = 0;
    estab_node cs_node = _estab_head;

    for (int i = 0; i < file_num; ++i) {
        FILE *fp = fps[i];
        fseek(fp, 0, SEEK_SET);
        int CSLTH = 0;
        while (cs_node->cs_length == 0) cs_node = cs_node->next_block;
        int ref_symbols[10];

        while (1) {
            fgets(line, sizeof(line), fp);
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0'; // delete '\n'
            }
            if (line[0] == 'H') {
                CSLTH = cs_node->cs_length;
                // set 01 reference number (CS addr)
                ref_symbols[1] = (int) CSADDR;
            } else if (line[0] == 'R') {
                char ref_num[3], symbol[7];
                int ref_cnt = ((int) strlen(line) - 1) / 8 + (((int) strlen(line) - 1) % 8 != 0 ? 1 : 0);
                for (int j = 0; j < ref_cnt; ++j) {
                    memset(ref_num, '\0', sizeof ref_num);
                    memset(symbol, '\0', sizeof symbol);
                    sscanf(line + 1 + j * 8, "%2c", ref_num);
                    sscanf(line + 1 + 2 + j * 8, "%6c", symbol);
                    ref_symbols[_convert_string_to_hex(ref_num)] = (int) (_find_estab_node(symbol)->address);
                }
            } else if (line[0] == 'T') {
                char start_addr[7], text_len[3], text_val[3];
                memset(start_addr, '\0', sizeof start_addr);
                memset(text_len, '\0', sizeof text_len);
                sscanf(line + 1, "%6c", start_addr);
                sscanf(line + 1 + 6, "%2c", text_len);
                int target_addr = _convert_string_to_hex(start_addr);
                for (int j = 0; j < _convert_string_to_hex(text_len); ++j) {
                    memset(text_val, '\0', sizeof text_val);
                    sscanf(line + 1 + 6 + 2 + j * 2, "%2c", text_val);
                    VM[CSADDR + target_addr] = (unsigned char) _convert_string_to_hex(text_val);
                    target_addr += 1;
                }
            } else if (line[0] == 'M') {
                char target_addr[7], text_sz[3], ref_num[3];
                memset(target_addr, '\0', sizeof target_addr);
                memset(text_sz, '\0', sizeof text_sz);
                memset(ref_num, '\0', sizeof ref_num);
                sscanf(line + 1, "%6c", target_addr);
                sscanf(line + 1 + 6, "%2c", text_sz);
                sscanf(line + 1 + 6 + 2 + 1, "%2c", ref_num);
                int target_addr_num = _convert_string_to_hex(target_addr);
                int num = VM[CSADDR + target_addr_num], backup = VM[CSADDR + target_addr_num];
                if (_convert_string_to_hex(text_sz) == 5) {
                    backup = (backup / 16) * 16; // first hex
                    num %= 16; // second hex
                }
                num *= 256;
                num += VM[CSADDR + target_addr_num + 1];
                num *= 256;
                num += VM[CSADDR + target_addr_num + 2];
                if (*(line + 1 + 6 + 2) == '+') {
                    num += ref_symbols[_convert_string_to_hex(ref_num)];
                } else { // if '-'
                    num -= ref_symbols[_convert_string_to_hex(ref_num)];
                }
                if (num > 0xFFFFF) num %= 0x100000;
                else if (num < 0) num += 0x100000;
                int first_part = (num / 256) / 256;
                if (_convert_string_to_hex(text_sz) == 5) {
                    first_part += backup;
                }
                VM[CSADDR + target_addr_num] = (unsigned char) first_part;
                VM[CSADDR + target_addr_num + 1] = (unsigned char) (num / 256);
                VM[CSADDR + target_addr_num + 2] = (unsigned char) (num % 256);
            } else if (line[0] == 'E') {
                if (strlen(line) > 1) {
                    char exec_addr[7];
                    memset(exec_addr, '\0', sizeof exec_addr);
                    sscanf(line + 1, "%6c", exec_addr);
                    EXECADDR = _convert_string_to_hex(exec_addr);
                }
                CSADDR += CSLTH;
                break;
            } else {
                continue;
            }
        }
        cs_node = cs_node->next_block;
    }
    prog_addr = EXECADDR;
    prog_end_addr = CSADDR;

    // initialize registers
    memset(REGS, 0, sizeof REGS);
    // update L reg to be program length
    int program_len = 0;
    cur_node = _estab_head;
    while (cur_node) {
        if (cur_node->cs_length) program_len += cur_node->cs_length;
        cur_node = cur_node->next_block;
    }
    REGS[2] = program_len;
    // update PC reg to be starting program address
    REGS[8] = (int) prog_addr;

    return 0;
}

int bp(char *addr) {
    // Error handling
    if (!addr) {
        printf("         breakpoint\n         ----------\n");
        for (int i = 0; i < MEMORY_SIZE; ++i) {
            if (bps[i] == 1) {
                printf("         %X\n", i);
            }
        }
        return 0;
    }

    // check 'clear' parameter
    if (strcmp(addr, "clear") == 0) {
        for (int i = 0; i < MEMORY_SIZE; ++i) bps[i] = 0;
        printf("         [" "\x1b[32m" "ok" "\x1b[0m" "] clear all breakpoints\n");
        return 0;
    }

    // parse hex
    int bp_addr = _convert_string_to_hex(addr);
    if (bp_addr < 0 || MEMORY_SIZE < bp_addr) {
        printf("Error: Unavailable breakpoint address value\n");
        return 1;
    }

    bps[bp_addr] = 1;
    printf("         [" "\x1b[32m" "ok" "\x1b[0m" "] create breakpoint %s\n", addr);

    return 0;
}

int run(void) {
    unsigned int init_prog_addr = prog_addr;
    unsigned int OP, N, I, X, B, P, E, R1, R2;
    int DISP;
    while (1) {
        if (prog_addr >= prog_end_addr) {
            _print_regs();
            printf("         End Program\n");
            prog_addr = init_prog_addr;
            return 0;
        }
        if (!bp_done && bps[prog_addr]) {
            // stop by breakpoint
            _print_regs();
            printf("         Stop at checkpoint[%X]\n", prog_addr);
            bp_done = 1;
            return 0;
        }
        OP = VM[prog_addr] & 0b11111100;
        N = (VM[prog_addr] & 0b10) >> 1;
        I = (VM[prog_addr] & 0b1);


        if ((OP == 160) || (OP == 180) || (OP == 184)) {
            // format 2
            R1 = VM[++prog_addr] >> 4;
            R2 = VM[prog_addr] & 0b1111;
            prog_addr++;
            _execOP(OP, N, I, R1, R2, 0);
        } else {
            X = (VM[++prog_addr] & 0b10000000) >> 7;
            // format 3
            if (N == 0 && I == 0) {
                DISP = (VM[prog_addr] & 0b111) << 8;
                DISP += VM[++prog_addr] << 4;
                DISP += VM[++prog_addr];
                prog_addr++;
                if (X) DISP += REGS[1];
                _execOP(OP, N, I, X, 0, DISP);
            } else {
                B = (VM[prog_addr] & 0b1000000) >> 6;
                P = (VM[prog_addr] & 0b100000) >> 5;
                E = (VM[prog_addr] & 0b10000) >> 4;
                DISP = (VM[prog_addr] & 0b1111) << 8;
                DISP += VM[++prog_addr];
                prog_addr++;
                if (B) DISP += REGS[3];
                else if (P) {
                    if (DISP >= 2048) DISP -= 4096;
                    DISP += (int) prog_addr;
                }
                if (X) DISP += REGS[1];
                if (E) {
                    DISP <<= 8;
                    DISP += VM[prog_addr];
                    prog_addr++;
                }
                _execOP(OP, N, I, X, B, DISP);
            }
        }
        bp_done = 0;
    }
}

int _convert_string_to_hex(char *str) {
    return (int) strtoul(str, NULL, 16);
}

void _init_estab(void) {
    estab_node next = NULL, cur = _estab_head;
    for (; cur || next; cur = next) {
        next = cur->next_block;
        free(cur);
    }
    _estab_head = NULL;
}

void _add_estab_node(int cs_length, unsigned int address, char *symbol) {
    estab_node new_node = (estab_node) malloc(sizeof(struct _estab_node));
    strcpy(new_node->symbol, symbol);
    new_node->address = address;
    new_node->cs_length = cs_length;
    new_node->next_block = NULL;

    // trim symbol name
    int i = (int) strlen(new_node->symbol) - 1;
    while (new_node->symbol[i] != '\0') {
        if (new_node->symbol[i] == ' ') new_node->symbol[i] = '\0';
        i--;
    }


    if (_estab_head) {
        estab_node cur_node = _estab_head;
        while (cur_node->next_block) cur_node = cur_node->next_block;
        cur_node->next_block = new_node;
    } else {
        _estab_head = new_node;
    }
}

estab_node _find_estab_node(char *symbol) {
    char symbolCopy[20];
    strcpy(symbolCopy, symbol);
    // trim symbol name
    int i = (int) strlen(symbolCopy) - 1;
    while (symbolCopy[i] != '\0') {
        if (symbolCopy[i] == ' ') symbolCopy[i] = '\0';
        i--;
    }
    estab_node cur_node = _estab_head;
    while (cur_node && strcmp(cur_node->symbol, symbolCopy) != 0) {
        cur_node = cur_node->next_block;
    }
    return cur_node;
}

void _print_regs(void) {
    printf("A : %06X  X : %06X\n", REGS[0], REGS[1]);
    printf("L : %06X PC : %06X\n", REGS[2], prog_addr);
    printf("B : %06X  S : %06X\n", REGS[3], REGS[4]);
    printf("T : %06X\n", REGS[5]);
}

void _execOP(unsigned int OP, unsigned int N, unsigned int I, unsigned int X, unsigned int B, int TA) {
    // X, B could be respectively R1, R2
    unsigned int r1 = X, r2 = B;
    int value = 0;

    switch (OP) {
        case 20: // STL
            VM[TA] = REGS[2] >> 16;
            VM[TA + 1] = (REGS[2] & 0b11111111000000000) >> 8;
            VM[TA + 2] = REGS[2] & 0b11111111;
            break;
        case 16: // STX
            VM[TA] = REGS[1] >> 16;
            VM[TA + 1] = (REGS[1] & 0b11111111000000000) >> 8;
            VM[TA + 2] = REGS[1] & 0b11111111;
            break;
        case 12: // STA
            VM[TA] = REGS[0] >> 16;
            VM[TA + 1] = (REGS[0] & 0b1111111100000000) >> 8;
            VM[TA + 2] = REGS[0] & 0b11111111;
            break;
        case 116: // LDT
            if (N == 1 && I == 1) {
                REGS[5] = VM[TA] << 16;
                REGS[5] = VM[TA + 1] << 8;
                REGS[5] = VM[TA + 2];
            } else if (N == 0 && I == 1) {
                REGS[5] = TA;
            }
            break;
        case 0: // LDA
            if (N == 1 && I == 1) {
                REGS[0] = VM[TA] << 16;
                REGS[0] += VM[TA + 1] << 8;
                REGS[0] += VM[TA + 2];
            } else if (N == 0 && I == 1) {
                REGS[0] = TA;
            }
            break;
        case 104: // LDB
            if (N == 1 && I == 1) {
                REGS[3] = VM[TA] << 16;
                REGS[3] = VM[TA + 1] << 8;
                REGS[3] = VM[TA + 2];
            } else if (N == 0 && I == 1) {
                REGS[3] = TA;
            }

            break;
        case 216: // RD
            // "input device로부터 아무것도 받지 못했다고 가정한다"
            REGS[0] &= 0b111111111111111100000000;
            break;
        case 220: // WD
            // " 다음 instruction으로 넘어간다."
            break;
        case 224: // TD
            // "다음 instruction으로 넘어가되 CC(condition code)는 '<'로 변경되었다고 가정한다"
            CC = '<';
            break;
        case 84: // STCH
            VM[TA] = REGS[0] & 0b11111111;
            break;
        case 80: // LDCH
            REGS[0] &= 0b111111111111111100000000;
            REGS[0] += VM[TA];
            break;
        case 76: // RSUB
            prog_addr = REGS[2];
            break;
        case 40: // COMP
            if (N == 1 && I == 1) {
                value = (VM[TA] << 16) + (VM[TA + 1] << 8) + VM[TA + 2];
            } else if (N == 0 && I == 1) {
                value = TA;
            }
            if (REGS[0] > value) CC = '>';
            else if (REGS[0] == value) CC = '=';
            else CC = '<';
            break;
        case 180: // CLEAR
            REGS[r1] = 0;
            break;
        case 160: // COMPR
            if (REGS[r1] > REGS[r2]) CC = '>';
            else if (REGS[r1] == REGS[r2]) CC = '=';
            else CC = '<';
            break;
        case 184: // TIXR
            REGS[1]++;
            if (REGS[1] > REGS[r1]) CC = '>';
            else if (REGS[1] == REGS[r1]) CC = '=';
            else CC = '<';
            break;
        case 60: // J
            if (N == 1 && I == 1) {
                prog_addr = TA;
            } else if (N == 1 && I == 0) {
                prog_addr = (VM[TA] << 16) + (VM[TA + 1] << 8) + VM[TA + 2];
            }
            break;
        case 48: // JEQ
            if (CC == '=') prog_addr = TA;
            break;
        case 52: // JGT
            if (CC == '>') prog_addr = TA;
            break;
        case 56: // JLT
            if (CC == '<') prog_addr = TA;
            break;
        case 72: // JSUB
            REGS[2] = (int) prog_addr;
            prog_addr = TA;
            break;
        default:
            printf("\n!?\n\n\n");


    }

}























