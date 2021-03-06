#pragma once
#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

static int linker_error = 0;

/* Linker Function Error Return Codes */
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

typedef struct LinkerLocationLabel
{
	char *name;
	uint32_t location;

	int section_id;

	char global;
	
	struct LinkerLocationLabel *next;
} LinkerLocationLabel;

typedef struct LinkerLocationToFill
{
	char *name;
	uint32_t location;

	struct LinkerLocationToFill *next;
} LinkerLocationToFill;

typedef struct LinkerSection
{
	char *name;
	char *type;

	FILE *fp;

	long code_begin_location;
	uint32_t code_section_length;

	LinkerLocationLabel *l_root;

	LinkerLocationToFill *ltf_root;

	uint32_t location_offset;

	struct LinkerSection *next;
} LinkerSection;

LinkerLocationLabel *linker_location_label_create(char *name, uint32_t location, int section_id, char global);

size_t linker_location_label_max(LinkerLocationLabel *l_root);

int linker_location_label_add(LinkerLocationLabel *l_root, LinkerLocationLabel *l_new);

LinkerLocationLabel *linker_location_labels_deserialize(FILE *fp, int section_id, LinkerLocationLabel *global_root);

LinkerLocationLabel *linker_location_label_get_by_name(LinkerLocationLabel *l_root, char *name);

LinkerLocationToFill *linker_location_to_fill_create(char *name, uint32_t location);

int linker_location_to_fill_add(LinkerLocationToFill *ltf_root, LinkerLocationToFill *ltf_new);

LinkerLocationToFill *linker_locations_to_fill_deserialize(FILE *fp);

LinkerSection *linker_section_create(char *name, char *type, FILE *fp, long code_begin_location, uint32_t code_section_length, LinkerLocationLabel *l_root, LinkerLocationToFill *ltf_root);

int linker_section_add(LinkerSection *s_root, LinkerSection *s_new);

int linker_section_get_count(LinkerSection *s_root);

LinkerSection *linker_section_get_by_id(LinkerSection *s_root, int id);

bool linker_section_id_in_array(int *section_order, int num_sections_to_write, int id);

char *linker_get_filename_from_fullname(char *fullname);

int *linker_section_get_file_global_sections(LinkerSection *s_root, char *fullname, int *num_file_global_sections);

int linker_read_sections_from_file(char *filename, LinkerSection *s_root, LinkerLocationLabel *global_root);

int *linker_get_section_write_order(LinkerSection *s_root, char **types, int num_types, int *num_sections_to_write);

int linker_read_code_from_file(FILE *input, FILE *output, long code_location, uint32_t code_length);

int linker_write_first_pass(LinkerSection *s_root, int *section_order, int num_sections_to_write, FILE *output_fp);

int linker_write_second_pass(LinkerSection *s_root, FILE *output_fp, LinkerLocationLabel *global_root, int *section_order, int num_sections_to_write);

int char_occurence_count(char *str, char c);

void linker_fatal_error();








