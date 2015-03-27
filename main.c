#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "types.h"
#include "cpu.h"

int main(int argc, char **argv)
{
	
	if(argc > 1)
	{
		
		if(strcmp(argv[1], "-v") == 0)
		{
			
			printf("Emulator\n\nCopyright (c) Ian Duncan 2015\n\nLicensed under the MIT License\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n");
			return 0;
		}
		
		
		
		
		
		//10*1024 kb memory
		//uint32_t memsize = 10485760;
		uint32_t memsize = 10485760;
		
		
		RAMUNIT *ram = ram_unit_init(memsize);
		CPU *cpu = cpu_init();
		CPURESULT *result;
		
		uint32_t address = 1024;
		int instructions_executed = 0;
		
		cpu->sp = 10485760;
		cpu->sb = 10482560;
		
		cpu->ip = 1024;
		
		
		
		
		/*//PUT
		set_byte_at_ram_address(ram, 0, 0x05);
		
		set_byte_at_ram_address(ram, 1, 0x00);
		
		set_dword_at_ram_address(ram, 2, 0x02);
		
		set_dword_at_ram_address(ram, 6, 255);
		
		
		
		
		//HLT
		set_byte_at_ram_address(ram, 7, 0x00);
		*/
		
		load_program_at_address(ram, 1024, argv[1]);
		
		
		
		
		while(!cpu->halted)
		{
			result = cpu_exec_instruction(cpu, ram);
			
			if(result->error != CPURESULT_SUCCESS)
			{
					if(!do_cpu_interrupt(cpu, ram, (BYTE) result->error))
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
	}
	else
	{
		printf("Usage:\n%s [file to run] [[(optional) RAM dump output]]\n%s -v for version and copyright information\n", argv[0], argv[0]);
		return 1;
	}
	
	return 0;
}
