#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "memory.h"
#include "types.h"
#include "error.h"

RAMUNIT *read_ram_contents_from_file(char *filename)
{
	FILE *fp;
	uint32_t size;
	RAMUNIT *ram;
	
	fp = fopen(filename, "rb");
	
	if(fp == NULL)
		return NULL;
		
	if(fread(&size, sizeof(uint32_t), 1, fp) < 1)
		return NULL;
	
	ram = ram_unit_init(size);
	
	if(ram == NULL)
		return NULL;
	
	if(fread(ram->data, sizeof(BYTE), size, fp) < size)
		return NULL;
	
	return ram;
}

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		printf("RAM Dump Analyzer\n\n");
		printf("Usage: %s [RAM Dump file]\n", argv[0]);
		printf("Usage: %s -v for version and copyright information\n", argv[0]);
		exit(1);
	}
	
	if(strcmp(argv[1], "-v") == 0)
	{
			
		printf("Memory Analyzer\n\nCopyright (c) Ian Duncan 2015\n\nLicensed under the MIT License\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n");
		return 0;
	}
	
	RAMUNIT *ram = read_ram_contents_from_file(argv[1]);
	
	if(ram == NULL)
	{
		printf("Error reading RAM Dump.\n");
		exit(2);
	}
	
	uint32_t address = 0;
	char read_size = 0;
	
	printf("%% ");
	scanf("%c %x", &read_size, &address);
	
	while(read_size != 'q')
	{
		emu_error = 0;
		
		switch(read_size)
		{
			
			case 'b':
			{
				BYTE b = get_byte_at_ram_address(ram, address);
				
				if(emu_error != 0)
					printf("Error: Unable to retrieve byte from that address.\n");
				else
					printf("BYTE 0x%x: 0x%x, %d\n", address, b, b);
				
				break;
			}
			case 'w':
			{
				WORD c = get_word_at_ram_address(ram, address);
				
				if(emu_error != 0)
					printf("Error: Unable to retrieve byte from that address.\n");
				else
					printf("WORD 0x%x: 0x%x, %d\n", address, c, c);
				
				break;
			}
			case 'd':
			{
				DWORD d = get_dword_at_ram_address(ram, address);
				
				if(emu_error != 0)
					printf("Error: Unable to retrieve byte from that address.\n");
				else
					printf("DWORD 0x%x: 0x%x, %d\n", address, d, d);
				
				break;
			}
			default:
			{
				printf("Error: Data type unrecognized.\n");
				break;
			}
		}
		
		printf("%% ");
		scanf(" %c %x", &read_size, &address);
	}
	
	end:
	
	return 0;
}
			
	
	
	
	
