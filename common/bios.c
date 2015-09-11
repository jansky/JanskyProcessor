#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "cpu.h"
#include "error.h"
#include "bios.h"

bool do_bios_interrupt(CPU *cpu, RAMUNIT *ram)
{
	//get the operation category
	switch(cpu->pr1)
	{
		//version information and metadata
	case 0x00:
	{
		//get suboperation
		switch(cpu->pr2)
		{
			//get BIOS version
		case 0x01:
		{
			cpu->ar1 = 0x03;

			break;
		}
		//get total memory in the system
		case 0x02:
		{
			cpu->ar1 = ram->bytesize;
			break;
		}
				
		default:
		{
			return false;
		}
		}

		break;
	}
	//screen operations
	case 0x01:
	{
		//get suboperation
		switch(cpu->pr2)
		{
			//write string to screen
		case 0x01:
		{
			if(!output_string_to_screen(ram, cpu->ar1))
				return false;

			break;
		}
		//write number in hex to screen
		case 0x02:
		{
			output_number_to_screen(cpu->ar1, 16);
			break;
		}
		//write number in octal to screen
		case 0x03:
		{
			output_number_to_screen(cpu->ar1, 8);
			break;
		}
		//write number in decimal to screen
		case 0x04:
		{
			output_number_to_screen(cpu->ar1, 10);
			break;
		}
		//write byte in ar1 as string to screen
		case 0x05:
		{
			return output_byte_as_string_to_screen(cpu, ram, false);
			break;
		}
		//write byte at pointer in ar1 as string to screen
		case 0x06:
		{
			return output_byte_as_string_to_screen(cpu, ram, true);
			break;
		}
		default:
		{
			return false;
			break;
		}
		}

		break;
	}
	//input operations
	case 0x02:
	{
		switch(cpu->pr2)
		{
	        //input string
		case 0x01:
		{
			return input_string(cpu, ram, cpu->ar1, cpu->ar2);
			break;
		}
		//input dword
		case 0x02:
		{
			return input_dword(cpu, ram, cpu->ar1);
			break;
		}
		//input byte
		case 0x03:
		{
			return input_byte(cpu, ram, cpu->ar1);
			break;
		}
		default:
		{
			return false;
			break;
		}
		}

		break;
		
	}
	//interop opterations
	case 0x03:
	{
		switch(cpu->pr2)
		{
	        //query interop
		case 0x01:
		{
			cpu->ar1 = interop_query(cpu->iinfo, cpu->ar1);
			break;
		}
		//read entire file into memory
		case 0x02:
		{
			if(cpu->ar2 >= (cpu->sb))
			{
				//set non-fatal error
				printf("\nstack error trigger\n");
				cpu->ar5 = 0x01;
				return true;
			}

			size_t filepath_loc;

			filepath_loc = (size_t)ram->data;
			filepath_loc += (size_t)cpu->ar2;

			//printf("\n0x%x - pointer location\n", filepath_loc);
					
			return interop_disk_read_file_into_ram(cpu, ram, cpu->ar1, (char*)(filepath_loc), cpu->iinfo);

			break;
		}
		//read entire file into memory (safe)
		case 0x03:
		{
			if(cpu->ar2 >= (cpu->sb))
			{
				//set non-fatal error
				printf("\nstack error trigger\n");
				cpu->ar5 = 0x01;
				return true;
			}

			size_t filepath_loc;

			filepath_loc = (size_t)ram->data;
			filepath_loc += (size_t)cpu->ar2;

			//printf("\n0x%x - pointer location\n", filepath_loc);
					
			return interop_disk_read_file_into_ram_safe(cpu, ram, cpu->ar1, cpu->ar3, (char*)(filepath_loc),  cpu->iinfo);

			break;
		}
		default:
		{
			return false;
			break;
		}
		}

		break;
	}		
	default:
	{
		return false;
		break;
	}
	}

	return true;
}
