#pragma once
#include "types.h"
#include <stdbool.h>

typedef struct RAMUNIT
{
	unsigned char *data;
	uint32_t bytesize;
} RAMUNIT;

RAMUNIT *ram_unit_init(uint32_t bytesize);

BYTE get_byte_at_ram_address(RAMUNIT *ram, uint32_t address);
WORD get_word_at_ram_address(RAMUNIT *ram, uint32_t address);
DWORD get_dword_at_ram_address(RAMUNIT *ram, uint32_t address);

void set_byte_at_ram_address(RAMUNIT *ram, uint32_t address, BYTE data);
void set_word_at_ram_address(RAMUNIT *ram, uint32_t address, WORD data);
void set_dword_at_ram_address(RAMUNIT *ram, uint32_t address, DWORD data);

bool load_program_at_address(RAMUNIT *ram, uint32_t address, char *filename);

bool write_ram_contents_to_file(RAMUNIT *ram, char *filename);

/*
 * WARNING
 * 
 * No attempts are made at checking endianness.
 * It is expected that you run this on a little-endian machine
 */


