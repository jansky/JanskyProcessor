#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define SASM_MAX_LINE_LENGTH 769 //768 characters + 1 for newline

static int sasm_error = 0;

/* Assembler Function Error Return Codes */
#define SASM_ERROR_NOERROR 0
#define SASM_ERROR_SYNTAXINVALID 1
#define SASM_ERROR_IOERROR 2
#define SASM_ERROR_STRINGERROR 3
#define SASM_ERROR_LABELERROR 4
#define SASM_ERROR_LABELINVALID 5
#define SASM_ERROR_NUMBERERROR 6
#define SASM_ERROR_REGISTERINVALID 7
#define SASM_ERROR_UNKNOWNERROR 8
#define SASM_ERROR_USAGEINVALID 9

typedef struct SASMLocationLabel
{
    char *name;
    uint32_t location;
    
    struct SASMLocationLabel *next;
} SASMLocationLabel;

typedef struct SASMLocationToFill
{
    char *name;
    uint32_t location;
    
    struct SASMLocationToFill *next;
} SASMLocationToFill;

/* Location Label Functions */

SASMLocationLabel *sasm_location_label_create(char *name, uint32_t location);
int sasm_location_label_get_max(SASMLocationLabel *root);
SASMLocationLabel *sasm_location_label_get(SASMLocationLabel *root, int id);
bool sasm_location_label_add(SASMLocationLabel *root, SASMLocationLabel *new);
SASMLocationLabel *sasm_location_label_get_by_name(SASMLocationLabel *root, char *name);

SASMLocationToFill *sasm_location_to_fill_create(char *name, uint32_t location);
int sasm_location_to_fill_get_max(SASMLocationToFill *root);
SASMLocationToFill *sasm_location_to_fill_get(SASMLocationToFill *root, int id);
bool sasm_location_to_fill_add(SASMLocationToFill *root, SASMLocationToFill *new);

int sasm_fill_in_labels(FILE *fp, SASMLocationLabel *ll_root, SASMLocationToFill *ltf_root);

/* Assembler Functions */

int sasm_is_string_empty(const char *s);

uint32_t sasm_parse_number(char *number);

int sasm_write_byte(FILE *fp, uint8_t byte);

int sasm_write_dword(FILE *fp, uint32_t dword);

int sasm_write_string(FILE *fp, char *string);

int sasm_write_get_location(FILE *fp, char *loctype, char *locarg, SASMLocationToFill *root);
int sasm_write_put_location(FILE *fp, char *loctype, char *locarg, SASMLocationToFill *root);

int sasm_assemble_line(char *line, FILE *fp, SASMLocationLabel *ll_root, SASMLocationToFill *ltf_root);

uint32_t get_register_id_from_string(char *reg);

int sasm_write_one_location(FILE *fp, SASMLocationToFill *ltf_root, bool location_get);
int sasm_write_two_locations(FILE *fp, SASMLocationToFill *ltf_root, bool location1_get, bool location2_get);

