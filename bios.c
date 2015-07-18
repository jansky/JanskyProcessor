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