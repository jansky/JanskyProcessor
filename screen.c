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

bool output_number_to_screen(DWORD number, int base)
{
	switch(base)
	{
	case 8:
	{
		printf("0o%o", number);
	        break;
	}
	case 10:
	{
		printf("%d", number);
		break;
	}
	default:
	{
		printf("0x%x", number);
		break;
	}

	}

	return true;
}

bool output_byte_as_string_to_screen(CPU *cpu, RAMUNIT *ram, bool ar1ispointer)
{
	if(!ar1ispointer)
	{
		printf("%c", (BYTE)cpu->ar1);
		return true;
	}
	else
	{
		
		emu_error = 0;
		uint32_t location = cpu->ar1;
	
		if(location >= ram->bytesize)
		{
			emu_error = EMUERR_OUTOFRANGE;
			return false;
		}
		
		BYTE c;
		c = get_byte_at_ram_address(ram, location);

		if(emu_error != 0)
			return false;

		printf("%c", c);
		return true;
	}
}
