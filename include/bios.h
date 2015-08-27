#pragma once
#include <stdio.h>
#include <stdint.h>

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
