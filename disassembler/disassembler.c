#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "types.h"

void output_location_info(FILE *ip, FILE *op);
void output_condition(FILE *ip, FILE *op);
BYTE get_byte(FILE *ip);
DWORD get_dword(FILE *ip);

int main(int argc, char **argv)
{

	FILE *ip;
	FILE *op;
	BYTE instruction;

	if(argc == 2 && strcmp(argv[1], "-v") == 0)
	{
			
		printf("Disassembler\n\nCopyright (c) Ian Duncan 2015\n\nLicensed under the MIT License\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n");
		return 0;
	}
	

	if(argc < 3)
	{
		printf("Disassembler\n\n");
		printf("Usage: %s [Program file] [Output file]\n", argv[0]);
		printf("Usage: To output to screen set the output file to be STDOUT\n");
		printf("Usage: %s -v for version and copyright information\n", argv[0]);
		exit(1);
	}
	
	

	ip = fopen(argv[1], "rb");

	if(ip == NULL)
	{
		printf("Error: Could not open program file.\n");
		exit(1);
	}

	if(strcmp(argv[2], "STDOUT") == 0)
	{
		op = stdout;
	}
	else
	{
		op = fopen(argv[2], "w");
	}

	if(op == NULL)
	{
		printf("Error: Could not open output file.\n");
		exit(2);
	}

	while(fread(&instruction, sizeof(BYTE), 1, ip) == 1)
	{
		fprintf(op, "; location 0x%x\n", (uint32_t)ftell(ip)-1);
		
		switch(instruction)
		{
		case 0x00:
		{
			fprintf(op, "hlt");
			break;

		}
		case 0x03:
		{
			fprintf(op, "int ");
			fprintf(op, "0x%x", get_byte(ip));
			break;

		}
		case 0x05:
		{
			fprintf(op, "put ");
			output_location_info(ip, op);
			fprintf(op, " 0x%x", get_dword(ip));
			break;
		}
		case 0x06:
		{
			fprintf(op, "cpy ");
			output_location_info(ip, op);
			fprintf(op, " ");
			output_location_info(ip, op);
			break;
		}
		case 0x07:
		{
			fprintf(op, "add ");
			output_location_info(ip, op);
			fprintf(op, " ");
			output_location_info(ip, op);
			break;
		}
		case 0x08:
		{
			fprintf(op, "sub ");
			output_location_info(ip, op);
			fprintf(op, " ");
			output_location_info(ip, op);
			break;
		}
		case 0x09:
		{
			fprintf(op, "mul ");
			output_location_info(ip, op);
			fprintf(op, " ");
			output_location_info(ip, op);
			break;
		}
		case 0x0A:
		{
			fprintf(op, "div ");
			output_location_info(ip, op);
			fprintf(op, " ");
			output_location_info(ip, op);
			break;
		}
		case 0x0B:
		{
			fprintf(op, "push ");
			output_location_info(ip, op);
			break;
		}
		case 0x0C:
		{
			fprintf(op, "pop ");
			output_location_info(ip, op);
			break;
		}
		case 0x0F:
		{
			fprintf(op, "jmp ");
			output_location_info(ip, op);
			break;
		}
		case 0x10:
		{
			fprintf(op, "cmp ");
			output_location_info(ip, op);
			fprintf(op, " ");
			output_location_info(ip, op);
			break;
		}
		case 0x11:
		{
			fprintf(op, "ret");
			break;
		}
		case 0x12:
		{
			fprintf(op, "jmpc ");
			output_condition(ip, op);
			fprintf(op, " ");
			output_location_info(ip, op);
			break;
		}
		case 0x13:
		{
			fprintf(op, "jmpr ");
			output_location_info(ip, op);
			fprintf(op, " ");
			output_location_info(ip, op);
			break;
		}
		case 0x14:
		{
			fprintf(op, "jmprc ");
			output_condition(ip, op);
			fprintf(op, " ");
			output_location_info(ip, op);
			fprintf(op, " ");
			output_location_info(ip, op);
			break;
		}
		case 0x15:
		{
			fprintf(op, "iret");
			break;
		}
		case 0x16:
		{
			fprintf(op, "uip ");
			output_location_info(ip, op);
			fprintf(op, " ");
			output_location_info(ip, op);
			break;
		}
		case 0x2e:
		{

			//Check for .dta mark
			if(get_byte(ip) == 0x64 && get_byte(ip) == 0x74 && get_byte(ip) == 0x61)
			{
				fprintf(op, "; .dta section begins at position 0x%lx.\n", ftell(ip));
				fclose(ip);
				fclose(op);
				exit(0);
			}
			else
			{
				printf("Error near position 0x%lx: 0x%x is not a valid instruction.\n", ftell(ip), instruction);
				exit(5);
			}
		}
		case 0xFF:
		{
			fprintf(op, "tst");
			break;
		}
		default:
		{
			printf("Error near position 0x%lx: 0x%x is not a valid instruction.\n", ftell(ip), instruction);
			exit(5);
			break;
		}
		}

		fprintf(op, "\n");

	}



	if(!feof(ip))
	{
		printf("Error near position 0x%lx: There was an IO error.\n", ftell(ip));
		exit(3);
	}

	fclose(ip);
	fclose(op);

	return 0;
}

void output_location_info(FILE *ip, FILE *op)
{

	BYTE loc_id;
	DWORD location;

	if(fread(&loc_id, sizeof(BYTE), 1, ip) != 1)
	{
		printf("Error near position 0x%lx: Reached EOF or had IO error while reading location information.\n", ftell(ip));
		exit(4);
	}

	if(fread(&location, sizeof(DWORD), 1, ip) != 1)
	{
		printf("Error near position 0x%lx: Reached EOF or had IO error while reading location information.\n", ftell(ip));
		exit(4);
	}

	

	if(loc_id > 0x04)
	{
		loc_id -= 0x05;
		fprintf(op, "b");
	}

	switch(loc_id)
	{
	case 0x00:
	{
		fprintf(op, "r ");

		switch(location)
		{
					
		case 0x00:
			fprintf(op, "ar1");
			break;
		case 0x01:
			fprintf(op, "ar2");
			break;
		case 0x02:
			fprintf(op, "ar3");
			break;
		case 0x03:
			fprintf(op, "ar4");
			break;
		case 0x04:
			fprintf(op, "ar5");
			break;
		case 0x05:
			fprintf(op, "ip");
			break;
		case 0x06:
			fprintf(op, "bp");
			break;
		case 0x07:
			fprintf(op, "sp");
			break;
		case 0x08:
			fprintf(op, "sb");
			break;
			//FPU not implemented yet
		case 0x0A:
			fprintf(op, "cr1");
			break;
		case 0x0B:
			fprintf(op, "pr1");
			break;
		case 0x0C:
			fprintf(op, "pr1");
			break;
		case 0x0F:
			fprintf(op, "flr1");
			break;
		default:
		{
			printf("Error near position 0x%lx: Register ID 0x%x invalid.\n", ftell(ip), location);
			exit(5);
		}
		}
		break;
	}
	case 0x01:
	{
		fprintf(op, "m ");

		fprintf(op, "0x%x", location);
		break;
	}
	case 0x02:
		fprintf(op, "a ");
		fprintf(op, "0x%x", location);
		break;
	case 0x03:
		fprintf(op, "pr ");
		switch(location)
		{
					
		case 0x00:
			fprintf(op, "ar1");
			break;
		case 0x01:
			fprintf(op, "ar2");
			break;
		case 0x02:
			fprintf(op, "ar3");
			break;
		case 0x03:
			fprintf(op, "ar4");
			break;
		case 0x04:
			fprintf(op, "ar5");
			break;
		case 0x05:
			fprintf(op, "ip");
			break;
		case 0x06:
			fprintf(op, "bp");
			break;
		case 0x07:
			fprintf(op, "sp");
			break;
		case 0x08:
			fprintf(op, "sb");
			break;
			//FPU not implemented yet
		case 0x0A:
			fprintf(op, "cr1");
			break;
		case 0x0B:
			fprintf(op, "pr1");
			break;
		case 0x0C:
			fprintf(op, "pr1");
			break;
		case 0x0F:
			fprintf(op, "flr1");
			break;
		default:
		{
			printf("Error near position 0x%lx: Register ID 0x%x invalid.\n", ftell(ip), location);
			exit(5);
			break;
		}
		}
		break;
	case 0x04:
		fprintf(op, "pm ");
		fprintf(op, "0x%x", location);
		break;
	default:
	{
		printf("Error near position 0x%lx: Location type 0x%x invalid.\n", ftell(ip), loc_id);
		exit(5);
		break;
	}
	}

}

BYTE get_byte(FILE *ip)
{
	BYTE b;

	if(fread(&b, sizeof(BYTE), 1, ip) != 1)
	{
		printf("Error near position 0x%lx: Reached unexpected EOF or had IO error.\n", ftell(ip));
		exit(4);
	}

	return b;

}

DWORD get_dword(FILE *ip)
{
	DWORD d;

	if(fread(&d, sizeof(DWORD), 1, ip) != 1)
	{
		printf("Error near position 0x%lx: Reached unexpected EOF or had IO error.\n", ftell(ip));
		exit(4);
	}



	return d;

}

void output_condition(FILE *ip, FILE *op)
{
	BYTE condition = get_byte(ip);

	switch(condition)
	{
	case 0x00:
	{
		fprintf(op, "ne");
		break;
	}
	case 0x01:
	{
		fprintf(op, "eq");
		break;
	}
	case 0x02:
	{
		fprintf(op, "gt");
		break;
	}
	case 0x03:
	{
		fprintf(op, "lt");
		break;
	}
	default:
	{
		printf("Error near position 0x%lx: Condition type 0x%x invalid.\n", ftell(ip), condition);
		exit(5);
		break;
	}
	}
}






