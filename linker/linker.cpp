#include "linker.h"
#include <stdlib.h>

std::string JPread_cpp_string_from_binfile(FILE *fp, uint16_t length)
{
	char *str;
	

	if(fp == NULL)
		return NULL;

	if(length == 0)
		return NULL;

	//length includes null terminator
	str = malloc(length);

	if(str == NULL)
		return NULL;

	if(fread(str, 1, length, fp) != length)
		return NULL;

	return new std::string(str);

	

}

int JPlinker_get_from_robj_file(const char *filename, JPLinkerInfo *info)
{
	if(info == NULL)
		return 1;

	FILE *fp = fopen(filename, "rb");

	if(fp == NULL)
		return 2;

	//verify magic number
	uint32_t magicnum = 0;

	if(fread(&magicnum, sizeof(uint32_t), 1, fp) != 1)
		return 3;

	if(magicnum != 0x6a726f66) // "jrof"
		return 4;

	// -- read label defs --

	// get the number of label defs
	uint32_t labeldefs_num = 0;

	if(fread(&labeldefs_num, sizeof(uint32_t), 1, fp) != 1)
		return 3;

	// read the label defs
	for(int i = 0; i < labeldefs_num; i++)
	{
	        JPDef *ldef = new JPDef();

		if(ldef == NULL)
			return 5;

		// get the label def name
		uint16_t namelength = 0;

		if(fread(&namelength, sizeof(uint16_t), 1, fp) != 1)
			return 3;

		std::string labelname = JPread_cpp_string_from_binfile(fp, namelength);

		if(labelname == NULL)
			return 6;

		ldef->name = labelname;

		// get the label def location
		uint32_t labellocation = 0;

		if(fread(&labellocation, sizeof(uint32_t), 1, fp) != 1)
			return 3;

		ldef->location = labellocation;

		// add the new label def to the vector
		info->defs.push_back(ldef);
	}

	return 0;
}
		
	

	
