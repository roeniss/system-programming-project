#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linkingLoader.h"
#include "vm.h"

#define MEMORY_SIZE 1048576

static int prog_addr = 0x00;
extern unsigned char VM[MEMORY_SIZE];

static estab_node _estab_head = NULL;

void _init_estab(void);

int _convert_string_to_hex(char *str);

void _add_estab_node(int cs_length, int address, char *symbol);

estab_node _find_estab_node(char *symbol);

int progaddr(char *addr) {
    // Error handling
    if (!addr) {
        printf("Error: No address value\n");
        return 1;
    }

    // parse hex
    prog_addr = _convert_string_to_hex(addr);
    if (prog_addr < 0 || MEMORY_SIZE < prog_addr) {
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

    int PROGADDR = prog_addr, CSADDR = prog_addr;
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
    PROGADDR = prog_addr, CSADDR = prog_addr;
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
                ref_symbols[1] = CSADDR;
            } else if (line[0] == 'R') {
                char ref_num[3], symbol[7];
                int ref_cnt = ((int) strlen(line) - 1) / 8 + (((int) strlen(line) - 1) % 8 != 0 ? 1 : 0);
                for (int j = 0; j < ref_cnt; ++j) {
                    memset(ref_num, '\0', sizeof ref_num);
                    memset(symbol, '\0', sizeof symbol);
                    sscanf(line + 1 + j * 8, "%2c", ref_num);
                    sscanf(line + 1 + 2 + j * 8, "%6c", symbol);
                    ref_symbols[_convert_string_to_hex(ref_num)] = _find_estab_node(symbol)->address;
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
                    VM[CSADDR + target_addr] = (char) _convert_string_to_hex(text_val);
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
                VM[CSADDR + target_addr_num] = (char) first_part;
                VM[CSADDR + target_addr_num + 1] = (char) (num / 256);
                VM[CSADDR + target_addr_num + 2] = (char) (num % 256);
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

    return 0;
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

void _add_estab_node(int cs_length, int address, char *symbol) {
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






























