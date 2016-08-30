#pragma once
#include <vector>
#include <map>
#include <string>
#include <stdint.h>


typedef std::map<uint32_t, std::string> JPLabels;

typedef struct JPDef
{
	std::string name;

	uint32_t location;
} JPDef;
	
typedef struct JPLinkerInfo
{
	std::vector<JPDef*> defs;
	JPLabels labels;

	std::vector<uint8_t> obj_bytes;

	uint32_t size;
} JPLinkerInfo;

std::string JPread_cpp_string_from_binfile(FILE *fp, uint16_t length);
int JPlinker_get_from_robj_file(const char *filename, JPLinkerInfo *info);


