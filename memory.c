#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "error.h"

RAMUNIT *ram_unit_init(uint32_t bytesize)
{
	
		emu_error = 0;
		
		//Are you stupid?
		if(bytesize == 0)
		{
			emu_error = EMUERR_ALLOCERROR;
			return NULL;
		}
		
		RAMUNIT *ram = malloc(sizeof(RAMUNIT));
		
		//Check if memory was allocated
		if(ram == NULL)
		{
			emu_error = EMUERR_ALLOCERROR;
			return NULL;
		}
		
		//Allocate memory requested
		ram->data = malloc(sizeof(unsigned char)*bytesize);
		ram->bytesize = bytesize;
		
		//Check if memory was allocated
		if(ram->data == NULL)
		{
			emu_error = EMUERR_ALLOCERROR;
			return NULL;
		}
		
		printf("Allocated %d bytes of RAM.\n", bytesize);
			
		return ram;
}

BYTE get_byte_at_ram_address(RAMUNIT *ram, uint32_t address)
{
	emu_error = 0;
	
	if(address >= ram->bytesize)
	{
		emu_error = EMUERR_OUTOFRANGE;
		return 0;
	}
	
	return ram->data[address];
}

WORD get_word_at_ram_address(RAMUNIT *ram, uint32_t address)
{
	emu_error = 0;
	
	//A WORD is 2 bytes, not 1
	if(address + 1 >= ram->bytesize)
	{
		emu_error = EMUERR_OUTOFRANGE;
		return 0;
	}
	
	char w[2];
	
	w[0] = ram->data[address];
	w[1] = ram->data[address + 1];
	
	return *((WORD*) w);
}

DWORD get_dword_at_ram_address(RAMUNIT *ram, uint32_t address)
{
	emu_error = 0;
	
	//A DWORD is 4 bytes, not 1
	if(address + 3 >= ram->bytesize)
	{
		emu_error = EMUERR_OUTOFRANGE;
		return 0;
	}
	
	char w[4];
	
	w[0] = ram->data[address];
	w[1] = ram->data[address + 1];
	w[2] = ram->data[address + 2];
	w[3] = ram->data[address + 3];
	
	return *((DWORD*) w);
}

void set_byte_at_ram_address(RAMUNIT *ram, uint32_t address, BYTE data)
{
	emu_error = 0;
	
	if(address > ram->bytesize)
	{
		emu_error = EMUERR_ALLOCERROR;
		return;
	}
	
	ram->data[address] = data;
}

void set_word_at_ram_address(RAMUNIT *ram, uint32_t address, WORD data)
{
	emu_error = 0;
	unsigned char *data_array = malloc(sizeof(unsigned char)*2);
	
	if(data_array == NULL)
	{
		emu_error = EMUERR_ALLOCERROR;
		return;
	}
	
	//WORD is 2 bytes, not 1
	if(address + 1 > ram->bytesize)
	{
		emu_error = EMUERR_ALLOCERROR;
		return;
	}
	
	data_array = (unsigned char*) &data;
	
	memcpy(ram->data + address, data_array, 2);
}

void set_dword_at_ram_address(RAMUNIT *ram, uint32_t address, DWORD data)
{
	emu_error = 0;
	unsigned char *data_array = malloc(sizeof(unsigned char)*4);
	
	if(data_array == NULL)
	{
		emu_error = EMUERR_ALLOCERROR;
		return;
	}
	
	//WORD is 4 bytes, not 1
	if(address + 3 > ram->bytesize)
	{
		emu_error = EMUERR_ALLOCERROR;
		return;
	}
	
	data_array = (unsigned char*) &data;
	
	memcpy(ram->data + address, data_array, 4);
}

void load_program_at_address(RAMUNIT *ram, uint32_t address, char *filename)
{
	unsigned char *buffer;
	FILE *fp;
	int filesize;
	
	fp = fopen(filename, "r");
	
	if(fp == NULL)
	{
		emu_error = EMUERR_ALLOCERROR;
		return;
	}
	
	//Get the length of the file
	fseek(fp, 0L, SEEK_END);
	filesize = ftell(fp);
	
	if(address + filesize  > ram->bytesize)
	{
		emu_error = EMUERR_ALLOCERROR;
		return;
	}
	
	buffer = malloc(sizeof(unsigned char)*filesize);
	
	if(buffer == NULL)
	{
		emu_error = EMUERR_ALLOCERROR;
		return;
	}
	
	fseek(fp, 0L, SEEK_SET);
	
	fread(buffer, sizeof(unsigned char), filesize, fp);
	
	memcpy(ram->data + address, buffer, sizeof(unsigned char)*filesize);
}

bool write_ram_contents_to_file(RAMUNIT *ram, char *filename)
{
	FILE *fp;
	
	fp = fopen(filename, "wb");
	
	if(fp == NULL)
		return false;
	
	if(fwrite(&ram->bytesize, sizeof(uint32_t), 1, fp) < 1)
	{
		fclose(fp);
		return false;
	}
	
	if(fwrite(ram->data, sizeof(BYTE), ram->bytesize, fp) < ram->bytesize)
	{
		fclose(fp);
		return false;
	}
	
	fclose(fp);
	
	return true;
}
	
	
	
	
		
	
	
	
	
	


	
	
		
		
		
		
