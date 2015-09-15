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

	int i = 0;
	
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

	

	

	for(i = 0; i <= strlen(buffer); i++)
	{
		set_byte_at_ram_address(ram, location+i, buffer[i]);

		if(emu_error != 0)
			return false;
	}

	//terminate the string
	set_byte_at_ram_address(ram, location+i, 0);

	if(emu_error != 0)
		return false;

	return true;
}

uint32_t input_number(CPU *cpu)
{
	emu_error = 0;
	cpu->ar5 = 0;
	uint32_t n = 0;

	char *nraw = malloc(200);

	if(nraw == NULL)
	{
		emu_error = EMUERR_ALLOCERROR;
		return n;
	}

	if(!fgets(nraw, 199, stdin))
	{
		//non-fatal error
		if(cpu != NULL)
			cpu->ar5 = 0x01;
		else
			emu_error = EMUERR_UNKNOWN;
		
		return n;
	}

	//remove trailing newline
	strtok(nraw, "\n");

	if(sscanf(nraw, "%d", &n) != 1)
	{
		//non-fatal error
		if(cpu != NULL)
			cpu->ar5 = 0x02;
		else
			emu_error = EMUERR_INVALIDINPUT;
		
		return n;
	}
	

	return n;
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

	input = input_number(cpu);

	if(emu_error != 0)
		return false;

	if(cpu->ar5 != 0)
	{
		//set non-fatal error
		cpu->ar5++;
		return true;
	}
		       

	

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

	input = input_number(cpu);

	if(emu_error != 0)
		return false;

	if(cpu->ar5 != 0)
	{
		//set non-fatal error
		cpu->ar5++;
		return true;
	}
	
	set_byte_at_ram_address(ram, location, (uint8_t)input);

	if(emu_error != 0)
		return false;

	return true;
}

	
		

	

	

	

	
