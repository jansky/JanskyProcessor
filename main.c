#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "types.h"
#include "cpu.h"

int main(int argc, char **argv)
{
	//1024 kb memory
	//uint32_t memsize = 10485760;
	uint32_t memsize = 4096;
	
	
	RAMUNIT *ram = ram_unit_init(memsize);
	CPU *cpu = cpu_init();
	CPURESULT *result;
	
	uint32_t address = 0;
	int instructions_executed = 0;
	
	cpu->sp = 4095;
	cpu->sb = 3895;
	
	
	/*//PUT
	set_byte_at_ram_address(ram, 0, 0x05);
	
	set_byte_at_ram_address(ram, 1, 0x00);
	
	set_dword_at_ram_address(ram, 2, 0x02);
	
	set_dword_at_ram_address(ram, 6, 255);
	
	
	
	
	//HLT
	set_byte_at_ram_address(ram, 7, 0x00);
	*/
	
	load_program_at_address(ram, 0, argv[1]);
	
	
	
	
	while(!cpu->halted)
	{
		result = cpu_exec_instruction(cpu, ram);
		
		if(result->error != CPURESULT_SUCCESS)
		{
				//We don't have interrupts, so exit.
				cpu->halted = true;
		}
		
		address = result->address;
		
		instructions_executed++;
		
		if(!cpu->halted)
			free(result);
	}
	
	print_end_dump(cpu, result, ram);
	
	printf("Instructions executed: %d\n", instructions_executed);
	
	//Write RAM dump
	char *filename = "mem.dmp";
	
	if(argc > 2)
		filename = argv[2];
	
	if(!write_ram_contents_to_file(ram, filename))
		printf("Unable to write RAM dump to \'%s\'.\n", filename);
	else
		printf("Wrote RAM dump to \'%s\'.\n", filename);
	
	//Be kind.
	free(ram);
	free(cpu);
	free(result);
	
	return 0;
}
