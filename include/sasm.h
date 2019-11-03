#pragma once
#define _POSIX_C_SOURCE 200809L
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

    char global;
    
    struct SASMLocationLabel *next;
} SASMLocationLabel;

typedef struct SASMLocationToFill
{
    char *name;
    uint32_t location;
    
    struct SASMLocationToFill *next;
} SASMLocationToFill;

typedef struct SASMSection
{
    char *name;

    SASMLocationLabel *l_root;

    SASMLocationToFill *ltf_root;

    char *code_contents;
    size_t code_contents_size;

    FILE *code_contents_fp;

    bool closed;

    struct SASMSection *next;
} SASMSection;

/* Location Label Functions */

SASMLocationLabel *sasm_location_label_create(char *name, uint32_t location, char global);
int sasm_location_label_get_max(SASMLocationLabel *root);
SASMLocationLabel *sasm_location_label_get(SASMLocationLabel *root, int id);
bool sasm_location_label_add(SASMLocationLabel *root, SASMLocationLabel *new);
SASMLocationLabel *sasm_location_label_get_by_name(SASMLocationLabel *root, char *name);

SASMLocationToFill *sasm_location_to_fill_create(char *name, uint32_t location);
int sasm_location_to_fill_get_max(SASMLocationToFill *root);
SASMLocationToFill *sasm_location_to_fill_get(SASMLocationToFill *root, int id);
bool sasm_location_to_fill_add(SASMLocationToFill *root, SASMLocationToFill *new);

SASMSection *sasm_section_create(char *name);

int sasm_section_add(SASMSection *s_root, SASMSection *s_new);

SASMSection *sasm_section_get_last_section(SASMSection *s_root);

int sasm_fill_in_labels(FILE *fp, SASMLocationLabel *ll_root, SASMLocationToFill *ltf_root);

int sasm_location_label_serialize(FILE *fp, SASMLocationLabel *ll);
int sasm_location_to_fill_serialize(FILE *fp, SASMLocationToFill *ltf);


/* Assembler Functions */

int sasm_is_string_empty(const char *s);

uint32_t sasm_parse_number(char *number);

int sasm_write_byte(FILE *fp, uint8_t byte);

int sasm_write_word(FILE *fp, uint16_t word);

int sasm_write_dword(FILE *fp, uint32_t dword);

int sasm_write_string(FILE *fp, char *string);

int sasm_write_object_file(FILE *fp, SASMSection *s_root);

int sasm_write_get_location(FILE *fp, char *loctype, char *locarg, SASMLocationToFill *root);
int sasm_write_put_location(FILE *fp, char *loctype, char *locarg, SASMLocationToFill *root);

int sasm_assemble_line(char *line, SASMSection *s_root);

uint32_t get_register_id_from_string(char *reg);

int sasm_write_one_location(FILE *fp, SASMLocationToFill *ltf_root, bool location_get);
int sasm_write_two_locations(FILE *fp, SASMLocationToFill *ltf_root, bool location1_get, bool location2_get);

