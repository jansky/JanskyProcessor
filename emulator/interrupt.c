#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "cpu.h"
#include "error.h"
#include "bios.h"

bool do_cpu_interrupt(CPU *cpu, RAMUNIT *ram, BYTE id)
{
//	printf("Interrupt 0x%x called.\n", id);
	emu_error = 0;

	//Check if BIOS interrupt
	if(id == 0x10)
	{
		return do_bios_interrupt(cpu, ram);
	}
	
	//Store instruction pointer
	//need to store or1
	cpu->bp = cpu->ip;
	
	//Move instruction pointer to value stored in IDT
	cpu->ip = get_dword_at_ram_address(ram, id*4);
	
	if(cpu->ip == 0)
	{
		//printf("interrupt condition line 20\n");
		return false; //No interrupt defined
	}
	
	if(emu_error != 0)
	{
		//printf("interrupt condition line 24\n");
		return false; //Something's really screwed up (or the programmer of the app running is stupid)
	}
	
	while(get_byte_at_ram_address(ram, cpu->ip) != 0x15) //Check for IRET instruction
	{
		if(emu_error != 0)
		{
			//printf("interrupt condition line 32\n");
			return false; //Something's really screwed up (or the programmer of the app running is stupid)
		}
		
		CPURESULT *result = cpu_exec_instruction(cpu, ram, true);
		
		if(result->error != 0)
		{
			//Another interrupt
			if(!do_cpu_interrupt(cpu, ram, result->error))
			{
				//printf("another interrupt called\n");
				return false;
			}
		}
		
		if(cpu->halted && get_byte_at_ram_address(ram, cpu->ip) && emu_error != 0)
		{
			//printf("interrupt condition line 41\n");
			return false;
		}
		
		if(cpu->halted && result->error == 0)
			return true;
		else if(cpu->halted && result->error != 0)
		{
			//printf("interrupt condition line 49\n");
			return false;
		}
	}
	
	//IRET instruction reached, we must return from interrupt
	cpu->ip = cpu->bp;
	
	return true;
}
		
		
	
