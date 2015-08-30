#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include "cpu.h"
#include "error.h"
#include "bios.h"

INTEROP_INFO *interop_info_create(char *root, bool disk_enabled)
{

	emu_error = 0;
	
	INTEROP_INFO *iinfo;

	iinfo = malloc(sizeof(INTEROP_INFO));

	if(iinfo == NULL)
	{
		emu_error = EMUERR_ALLOCERROR;
		return NULL;
	}

	iinfo->root = root;
	iinfo->disk_enabled = disk_enabled;

        return iinfo;

}

bool interop_query(INTEROP_INFO *iinfo, int query)
{
	if(iinfo == NULL)
		return false;

	switch(query)
	{
	case INTEROP_QUERY_DISK:
		return iinfo->disk_enabled;
	default:
		return false;
	}

	return false;
}

bool interop_disk_read_file_into_ram(CPU *cpu, RAMUNIT *ram, DWORD location, char *filepath, INTEROP_INFO *iinfo)
{
	char *fullpath;
	FILE *fp;

	struct stat st;

	uint8_t buffer;
	uint32_t i = 0;

	cpu->ar5 = 0x00;
	
	if(iinfo == NULL)
		return false;
	
	if(!iinfo->disk_enabled)
	{
		//set non-fatal error
		cpu->ar5 = 0x02;
		return true;
	}

	if(!iinfo->root || !filepath)
		return false;

	//we need an extra / character
	fullpath = malloc(strlen(iinfo->root)+strlen(filepath)+3);

	

	if(fullpath == NULL)
		return false;

	sprintf(fullpath, "%s/%s", iinfo->root, filepath);

	fp = fopen(fullpath, "rb");

	if(fp == NULL)
	{
		//set non-fatal error
		cpu->ar5 = 0x03;
		return true;
	}

	stat(fullpath, &st);

	int size = st.st_size;
	
	if(size < 1)
	{
		//set non-fatal error
		cpu->ar5 = 0x04;
		fclose(fp);
		return true;
	}

	if(location + size >= (ram->bytesize - 1))
	{
		// set non-fatal error
		cpu->ar5 = 0x05;
		fclose(fp);
		return true;
	}

	while(fread(&buffer, 1, 1, fp) == 1)
	{
		emu_error = 0;

		set_byte_at_ram_address(ram, location+i, buffer);

		if(emu_error != 0)
		{
			//set non-fatal error
			cpu->ar5 = 0x06;
			fclose(fp);
			return true;
		}

		i++;
	}

	if(ferror(fp))
	{
		//set non-fatal error
		cpu->ar5 = 0x07;
		fclose(fp);
		return true;
	}

	fclose(fp);

	return true;
}
			
		

	

	

	


	
		
		
