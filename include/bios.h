#pragma once
#include <stdio.h>
#include <stdint.h>
#include "cpu.h"
#include "memory.h"
#include "interop.h"


typedef struct IOUNIT
{
	FILE *source;
	uint64_t size;

	struct IOUNIT *next;
} IOUNIT;

	

bool do_bios_interrupt(CPU *cpu, RAMUNIT *ram);

bool output_string_to_screen(RAMUNIT *ram, DWORD location);
bool output_number_to_screen(DWORD number, int base);
bool output_byte_as_string_to_screen(CPU *cpu, RAMUNIT *ram, bool ar1ispointer);

bool input_string(CPU *cpu, RAMUNIT *ram, DWORD location, DWORD maxsize);
bool input_dword(CPU *cpu, RAMUNIT *ram, DWORD location);
bool input_byte(CPU *cpu, RAMUNIT *ram, DWORD location);

INTEROP_INFO *interop_info_create(char *root, bool disk_enabled);
bool interop_query(INTEROP_INFO *iinfo, int query);
bool interop_disk_read_file_into_ram(CPU *cpu, RAMUNIT *ram, DWORD location, char *filepath, INTEROP_INFO *iifno);
