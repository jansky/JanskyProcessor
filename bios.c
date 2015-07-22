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
					cpu->ar1 = 0x01;

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
