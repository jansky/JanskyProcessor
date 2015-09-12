#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "memory.h"
#include "types.h"
#include "bios.h"
#include "error.h"
#include "cpu.h"
#include "debug.h"

void run_emulator(uint32_t memsize, uint32_t stacksize, uint32_t loadat, char *programfile, bool writedump, char *dumpfile, bool regdump, char *root, bool debug);

void do_about();



int main(int argc, char **argv)
{
	int c;
	static int mem_dump = true;
	static int reg_dump = true;
	static int about = false;
	static int debug = false;

	uint32_t memsize = 10485760;
	uint32_t stacksize = 0x100;
	uint32_t loadat = 0x400;

	char *program = NULL;
	char *dmp_file = "mem.dmp";
	char *root = NULL;
	

	while(1)
	{
		static struct option long_options[] =
		{
			{"no-reg-dump", no_argument, &reg_dump, false},
			{"no-mem-dump", no_argument, &mem_dump, false},
			{"version", no_argument, &about, true},
			{"debug", no_argument, &debug, true},
			{"program", required_argument, 0, 'p'},
			{"memsize", required_argument, 0, 'm'},
			{"stacksize", required_argument, 0, 's'},
			{"loadat", required_argument, 0, 'l'},
			{"dumpfile", required_argument, 0, 'f'},
			{"rootdir", required_argument, 0, 'r'},
			{0,0,0,0}
		};

		int option_index = 0;

		c = getopt_long(argc, argv, "p:m:s:l:f:r:", long_options, &option_index);

		if(c == -1)
			break;

		switch(c)
		{
		case 0:
			break;
		case 'p':
			program = optarg;
			break;
		case 'f':
		{
			if(optarg != NULL)
				dmp_file = optarg;
			break;
		}
		case 'r':
		{
			if(optarg != NULL)
				root = optarg;
			break;
		}
		case 'm':
		{
			if(optarg != NULL)
			{
				if(sscanf(optarg, "%x", &memsize) != 1)
				{
					fprintf(stderr, "Error: --memsize expects hexadecimal value.\n");
					exit(1);
				}

				if(memsize < 0x100000)
				{
					fprintf(stderr, "Error: memory size must be at least 0x100000.\n");
					exit(1);
				}
			}

			break;
		}
		case 's':
		{
			if(optarg != NULL)
			{
				if(sscanf(optarg, "%x", &stacksize) != 1)
				{
					fprintf(stderr, "Error: --stacksize expects hexadecimal value.\n");
					exit(1);
				}

				if(memsize < 0x96)
				{
					fprintf(stderr, "Error: stack size must be at least 0x96.\n");
					exit(1);
				}
			}

			break;
		}
		case 'l':
		{
			if(optarg != NULL)
			{
				if(sscanf(optarg, "%x", &loadat) != 1)
				{
					fprintf(stderr, "Error: --loadat expects hexadecimal value.\n");
					exit(1);
				}

				
			}

			break;
		}
		case '?':
			break;
		default:
			abort();
			break;
		}
	}

	if(about)
	{
		do_about();
		exit(0);
	}
	

	if(program == NULL)
	{
		fprintf(stderr, "Error: Program filename must be specified.\n");
		exit(1);
	}

	run_emulator(memsize, stacksize, loadat, program, mem_dump, dmp_file, reg_dump, root, debug);

	
	
		

				

		

	
	return 0;
}

void run_emulator(uint32_t memsize, uint32_t stacksize, uint32_t loadat, char *programfile, bool writedump, char *dumpfile, bool regdump, char *root, bool debug)
{
	
		
		
	RAMUNIT *ram = ram_unit_init(memsize);
	CPU *cpu = cpu_init();
	CPURESULT *result;
	INTEROP_INFO *iinfo;

	breakpoint *b_root = NULL;
		
	
	int instructions_executed = 0;

	if(ram == NULL || cpu == NULL)
	{
		fprintf(stderr, "Error: Initialization failed.\n");
		exit(2);
	}
		
	cpu->sp = memsize - 1;
	cpu->sb = ((memsize - 1) - (stacksize*4));

	if(loadat >= (memsize - 1))
	{
		fprintf(stderr, "Error: Program load position out of bounds.\n");
		exit(3);
	}

	if(cpu->sb <= loadat)
	{
		fprintf(stderr, "Error: Stack base cannot be lower than program load position.\n");
		exit(4);
	}

	
		
	cpu->ip = 0;
	cpu->or1 = loadat;

	//setup interop
	if(root == NULL)
	{
		//no disk interop
		iinfo = interop_info_create(NULL, false);

		if(iinfo == NULL)
		{
			fprintf(stderr, "Error: Unable to allocate memory for interop info struct.\n");

			exit(6);
		}
	}
	else
	{
		//disk interop enabled
		iinfo = interop_info_create(root, true);

		if(iinfo == NULL)
		{
			fprintf(stderr, "Error: Unable to allocate memory for interop info struct.\n");

			exit(6);
		}

	}

	cpu->iinfo = iinfo;

	
		
		
		
		
		
	//This should fix stack problems.
	//stack_push(cpu, ram, 0);

	emu_error = 0;
		
	if(!load_program_at_address(ram, loadat, programfile))
	{
		fprintf(stderr, "Error: Could not load specified program into memory.\n");

		exit(9);
	}

	//if debug is enabled, allow the user a chance to set breakpoints and do other things
	if(debug)
		b_root = debug_do_interface(b_root, cpu, ram);

	

	
	

	
		
		
		
		
	while(!cpu->halted)
	{

		//see if we're on a breakpoint
		if(debug && b_root != NULL)
		{
			//printf("\nbreakpoint possible\n");
			if(debug_is_on_breakpoint(b_root, cpu))
			{
				b_root = debug_do_interface(b_root, cpu, ram);
			}
		}
		
		result = cpu_exec_instruction(cpu, ram, false);
			
		if(result->error != CPURESULT_SUCCESS)
		{
			//printf("Error result: 0x%x", result->error);
			if(!do_cpu_interrupt(cpu, ram, (BYTE) result->error))
			{
				if(debug)
				{
					printf("Last Intruction: %x\tResult: %x\n\n", result->instruction, result->error);
					b_root = debug_do_interface(b_root, cpu, ram);
				}
				else
				{
					
					cpu->halted = true;
				}
			}
		}
			
		
			
		instructions_executed++;
			
		if(!cpu->halted)
			free(result);
	}

	if(regdump)
	{		
		print_end_dump(cpu, result, ram);
		
		printf("Instructions executed: %d\n", instructions_executed);
	}
		
	//Write RAM dump
	
		
	if(writedump)
	{
		if(!write_ram_contents_to_file(ram, dumpfile))
		{
			fprintf(stderr, "Unable to write RAM dump to \'%s\'.\n", dumpfile);
			exit(5);
		}
		else
		{
			fprintf(stderr, "Wrote RAM dump to \'%s\'.\n", dumpfile);
			
		}
	}
		
	//Be kind.
	free(ram);
	free(cpu);
	free(result);
}

void do_about()
{
	
	printf("Emulator\n\nCopyright (c) Ian Duncan 2015\n\nLicensed under the MIT License\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n");
		
	
}
