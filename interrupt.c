#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "cpu.h"
#include "error.h"

bool do_cpu_interrupt(CPU *cpu, RAMUNIT *ram, BYTE id)
{
	emu_error = 0;
	
	//Store instruction pointer
	cpu->bp = cpu->ip;
	
	//Move instruction pointer to value stored in IDT
	cpu->ip = get_dword_at_ram_address(ram, id*4);
	
	if(cpu->ip == 0)
		return false; //No interrupt defined
	
	if(emu_error != 0)
		return false; //Something's really screwed up (or the programmer of the app running is stupid)
	
	while(get_byte_at_ram_address(ram, cpu->ip) != 0x15) //Check for IRET instruction
	{
		if(emu_error != 0)
			return false; //Something's really screwed up (or the programmer of the app running is stupid)
		
		CPURESULT *result = cpu_exec_instruction(cpu, ram);
		
		if(result != 0)
		{
			//Another interrupt
			if(!do_cpu_interrupt(cpu, ram, result->error))
				return false;
		}
		
		if(cpu->halted && get_byte_at_ram_address(ram, cpu->ip) && emu_error != 0)
			return true;
		else
			return false;
	}
	
	//IRET instruction reached, we must return from interrupt
	cpu->ip = cpu->bp;
	
	return true;
}
		
		
	
