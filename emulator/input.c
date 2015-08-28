#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "cpu.h"
#include "error.h"
#include "bios.h"

bool input_string(CPU *cpu, RAMUNIT *ram, DWORD location, DWORD maxsize)
{
	
	emu_error = 0;
	cpu->ar5 = 0;
	
	char *buffer;
	char format[20];

	if(maxsize > 500)
	{
		//set non-fatal error
		cpu->ar5 = 0x01;

		return true;
	}

	if(maxsize + 1 >= ram->bytesize)
	{
		//set non-fatal error
		cpu->ar5 = 0x02;
	}

	buffer = malloc(maxsize+1);

	if(buffer == NULL)
	{
		//set non-fatal error
		cpu->ar5 = 0x03;

		return true;
	}

	//sprintf(format, "%%%d[0-9a-zA-Z ]", maxsize);

	//scanf(format, buffer);

	if(!fgets(buffer, maxsize, stdin))
	{
		//set non-fatal error
		cpu->ar5 = 0x04;

		return true;
	}

	strtok(buffer, "\n");

	

	for(int i = 0; i <= strlen(buffer); i++)
	{
		set_byte_at_ram_address(ram, location+i, buffer[i]);

		if(emu_error != 0)
			return false;
	}

	return true;
}

bool input_dword(CPU *cpu, RAMUNIT *ram, DWORD location)
{
	emu_error = 0;
	cpu->ar5 = 0;

	if(location + 3 >= ram->bytesize)
	{
		//set non-fatal error
		cpu->ar5 = 0x01;

		return true;
	}

	uint32_t input = 0;

	scanf("%d", &input);

	set_dword_at_ram_address(ram, location, input);

	if(emu_error != 0)
		return false;

	return true;
}

bool input_byte(CPU *cpu, RAMUNIT *ram, DWORD location)
{
	emu_error = 0;
	cpu->ar5 = 0;
	
	if(location >= ram->bytesize)
	{
		//set non-fatal error
		cpu->ar5 = 0x01;

		return true;
	}

	uint32_t input = 0;

	scanf("%d", &input);
	
	set_byte_at_ram_address(ram, location, (uint8_t)input);

	if(emu_error != 0)
		return false;

	return true;
}

	
		

	

	

	

	
