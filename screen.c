#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "cpu.h"
#include "error.h"
#include "bios.h"

bool output_string_to_screen(RAMUNIT *ram, DWORD location)
{
	emu_error = 0;
	uint32_t i = location;

	if(location >= ram->bytesize)
	{
		emu_error = EMUERR_OUTOFRANGE;
		return false;
	}

	BYTE c;

	c = get_byte_at_ram_address(ram, i);

	if(emu_error != 0)
		return false;

	i++;



	while(c != 0)
	{

		printf("%c", c);

		c = get_byte_at_ram_address(ram, i);

		if(emu_error != 0)
			return false;

		i++;

	}
}