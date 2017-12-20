#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <map>
#include <stdexcept>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t Byte;

typedef struct ErrorInformation
{
	std::string filename;
	int line_num;
} ErrorInformation;

typedef struct ParserReturn
{
	std::vector<Byte> bytes;
	std::map<uint32_t,std::string> jumps;
	std::map<std::string,std::vector<Byte>> labels;
} ParserReturn;


void assembler_print_error_and_exit(std::string error, ErrorInformation e, int exitcode);
std::vector<std::string> assembler_string_tokenize(const std::string &source, const char *delimiter, bool keepEmpty);
std::vector<std::string> assembler_parse_line(std::string line);
ParserReturn assembler_parse_line_tokens(std::vector<std::string> line_tokens, ErrorInformation e, uint32_t offset);
std::vector<Byte> assembler_uint32_string_to_byte_array(std::string num, ErrorInformation e);
std::vector<Byte> assembler_assemble_location(std::string loc_id, std::string location, ErrorInformation e);
std::vector<Byte> assembler_assemble_location(std::string loc_id, std::string location, ErrorInformation e, std::map<uint32_t,std::string> jumps, uint32_t address);
void write_bytes_to_file(std::ofstream output, std::vector<Byte> bytes);
std::vector<Byte> assembler_label_postprocessor(std::vector<Byte> bytes, std::map<std::string,std::vector<Byte>> labels, std::map<uint32_t,std::string> jumps);


void assembler_print_error_and_exit(std::string error, ErrorInformation e, int exitcode)
{
	std::cerr << "Error " << e.filename << ":" << e.line_num << ": " << error << std::endl;
	exit(exitcode);
}

std::vector<std::string> assembler_string_tokenize(const std::string &source, const char *delimiter, bool keepEmpty)
{
    std::vector<std::string> results;

    size_t prev = 0;
    size_t next = 0;

    while ((next = source.find_first_of(delimiter, prev)) != std::string::npos)
    {
        if (keepEmpty || (next - prev != 0))
        {
            results.push_back(source.substr(prev, next - prev));
        }
        prev = next + 1;
    }

    if (prev < source.size())
    {
        results.push_back(source.substr(prev));
    }

    return results;
}

std::vector<std::string> assembler_parse_line(std::string line)
{
	return assembler_string_tokenize(line, " ", false);
}

ParserReturn assembler_parse_line_tokens(std::vector<std::string> line_tokens, ErrorInformation e, uint32_t offset)
{
	ParserReturn p;
	std::vector<Byte> bytes;
	std::map<std::string,std::vector<Byte>> labels;
	std::map<uint32_t,std::string> jumps;


	
	if(line_tokens[0] == "hlt")
	{
		bytes.push_back(0x00);
	}
	else if(line_tokens[0] == "int")
	{
		if(line_tokens.size() < 2)
			assembler_print_error_and_exit("Operation 'int' requires 1 operand.", e, 1);
		else
		{
			bytes.push_back(0x03);
			
			bytes.push_back(assembler_uint32_string_to_byte_array(line_tokens[1], e)[0]);
		}
	}
	else if(line_tokens[0] == "put")
	{
		if(line_tokens.size() < 4)
			assembler_print_error_and_exit("Operation 'put' requires 3 operands.", e, 1);
		else
		{
			bytes.push_back(0x05);
			
			std::vector<Byte> loc_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e);
			std::vector<Byte> data_bytes = assembler_uint32_string_to_byte_array(line_tokens[3], e);
			
			bytes.insert(bytes.end(), loc_bytes.begin(), loc_bytes.end());
			bytes.insert(bytes.end(), data_bytes.begin(), data_bytes.end());
		}
	}
	else if(line_tokens[0] == "cpy")
	{
		if(line_tokens.size() < 5)
			assembler_print_error_and_exit("Operation 'cpy' requires 4 operands.", e, 1);
		else
		{
			bytes.push_back(0x06);
			
			if(line_tokens[1] == "l")
			{
				jumps[(uint32_t)bytes.size() + offset] = line_tokens[2];
				
				bytes.push_back(0x02);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
			}
			else if(line_tokens[1] == "lo")
			{
				//std::cerr << "lo location" << std::endl;
				jumps[(uint32_t)bytes.size() + offset] = line_tokens[2];
				
				bytes.push_back(0x0B);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
			}
			else
			{
				std::vector<Byte> loc_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e);
			
				bytes.insert(bytes.end(), loc_bytes.begin(), loc_bytes.end());
			}

			std::vector<Byte> loc2_bytes = assembler_assemble_location(line_tokens[3], line_tokens[4], e, jumps, (uint32_t)bytes.size()+offset);
			
			//bytes.insert(bytes.end(), loc1_bytes.begin(), loc1_bytes.end());
			bytes.insert(bytes.end(), loc2_bytes.begin(), loc2_bytes.end());
		}
	}
	else if(line_tokens[0] == "add")
	{
		if(line_tokens.size() < 5)
			assembler_print_error_and_exit("Operation 'add' requires 4 operands.", e, 1);
		else
		{
			bytes.push_back(0x07);
			
			std::vector<Byte> loc1_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e,jumps, (uint32_t)bytes.size()+offset);
			std::vector<Byte> loc2_bytes = assembler_assemble_location(line_tokens[3], line_tokens[4], e,jumps, (uint32_t)bytes.size()+offset);
			
			bytes.insert(bytes.end(), loc1_bytes.begin(), loc1_bytes.end());
			bytes.insert(bytes.end(), loc2_bytes.begin(), loc2_bytes.end());
		}
	}
	else if(line_tokens[0] == "sub")
	{
		if(line_tokens.size() < 5)
			assembler_print_error_and_exit("Operation 'sub' requires 4 operands.", e, 1);
		else
		{
			bytes.push_back(0x08);
			
			std::vector<Byte> loc1_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e,jumps, (uint32_t)bytes.size()+offset);
			std::vector<Byte> loc2_bytes = assembler_assemble_location(line_tokens[3], line_tokens[4], e,jumps, (uint32_t)bytes.size()+offset);
			
			bytes.insert(bytes.end(), loc1_bytes.begin(), loc1_bytes.end());
			bytes.insert(bytes.end(), loc2_bytes.begin(), loc2_bytes.end());
		}
	}
	else if(line_tokens[0] == "mul")
	{
		if(line_tokens.size() < 5)
			assembler_print_error_and_exit("Operation 'mul' requires 4 operands.", e, 1);
		else
		{
			bytes.push_back(0x09);
			
			std::vector<Byte> loc1_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e,jumps, (uint32_t)bytes.size()+offset);
			std::vector<Byte> loc2_bytes = assembler_assemble_location(line_tokens[3], line_tokens[4], e,jumps, (uint32_t)bytes.size()+offset);
			
			bytes.insert(bytes.end(), loc1_bytes.begin(), loc1_bytes.end());
			bytes.insert(bytes.end(), loc2_bytes.begin(), loc2_bytes.end());
		}
	}
	else if(line_tokens[0] == "div")
	{
		if(line_tokens.size() < 5)
			assembler_print_error_and_exit("Operation 'div' requires 4 operands.", e, 1);
		else
		{
			bytes.push_back(0x0A);
			
			std::vector<Byte> loc1_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e,jumps, (uint32_t)bytes.size()+offset);
			std::vector<Byte> loc2_bytes = assembler_assemble_location(line_tokens[3], line_tokens[4], e,jumps, (uint32_t)bytes.size()+offset);
			
			bytes.insert(bytes.end(), loc1_bytes.begin(), loc1_bytes.end());
			bytes.insert(bytes.end(), loc2_bytes.begin(), loc2_bytes.end());
		}
	}
	else if(line_tokens[0] == "push")
	{
		if(line_tokens.size() < 3)
			assembler_print_error_and_exit("Operaiton 'push' requires 2 operands.", e, 1);
		else
		{
			bytes.push_back(0x0B);
			
			std::vector<Byte> loc_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e,jumps, (uint32_t)bytes.size()+offset);
			
			bytes.insert(bytes.end(), loc_bytes.begin(), loc_bytes.end());
		}
	}
	else if(line_tokens[0] == "pop")
	{
		if(line_tokens.size() < 3)
			assembler_print_error_and_exit("Operaiton 'pop' requires 2 operands.", e, 1);
		else
		{
			bytes.push_back(0x0C);
			
			std::vector<Byte> loc_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e,jumps, (uint32_t)bytes.size()+offset);
			
			bytes.insert(bytes.end(), loc_bytes.begin(), loc_bytes.end());
		}
	}
	else if(line_tokens[0] == "jmp")
	{
		if(line_tokens.size() < 3)
			assembler_print_error_and_exit("Operation 'jmp' requires 2 operands.", e, 1);
		else
		{
			bytes.push_back(0x0F);
		
			
			if(line_tokens[1] == "l")
			{
				jumps[(uint32_t)bytes.size() + offset] = line_tokens[2];
				
				bytes.push_back(0x02);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
			}
			else if(line_tokens[1] == "lo")
			{
				jumps[(uint32_t)bytes.size() + offset] = line_tokens[2];
				
				bytes.push_back(0x0B);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
			}
			else
			{
				std::vector<Byte> loc_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e,jumps, (uint32_t)bytes.size()+offset);
			
				bytes.insert(bytes.end(), loc_bytes.begin(), loc_bytes.end());
			}
			
		}
	}
	else if(line_tokens[0] == "cmp")
	{
		if(line_tokens.size() < 5)
			assembler_print_error_and_exit("Operation 'cmp' requires 4 operands.", e, 1);
		else
		{
			bytes.push_back(0x10);
			
			std::vector<Byte> loc1_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e,jumps, (uint32_t)bytes.size()+offset);
			std::vector<Byte> loc2_bytes = assembler_assemble_location(line_tokens[3], line_tokens[4], e,jumps, (uint32_t)bytes.size()+offset);
			
			bytes.insert(bytes.end(), loc1_bytes.begin(), loc1_bytes.end());
			bytes.insert(bytes.end(), loc2_bytes.begin(), loc2_bytes.end());
		}
	}
	//Non instruction
	else if(line_tokens[0] == "lbl")
	{
		if(line_tokens.size() < 2)
			assembler_print_error_and_exit("Macro 'lbl' requires 1 operand.", e, 1);
		else
		{
			
			Byte b[4];
	
			*reinterpret_cast<uint32_t*>(b) = (uint32_t)bytes.size() + offset;
			 
			std::vector<Byte> lblbytes;
			
			//lblbytes.push_back(0x02);
			
			for(int i = 0; i < 4; i++)
			{
				lblbytes.push_back(b[i]);
			}
			
			labels[line_tokens[1]] = lblbytes;

			std::cout << line_tokens[1] << " ";

			printf("0x%x\n", (uint32_t)bytes.size() + offset);
		}
	}
	else if(line_tokens[0] == "ret")
	{
		bytes.push_back(0x11);
	}
	else if(line_tokens[0] == "jmpc")
	{
		if(line_tokens.size() < 4)
			assembler_print_error_and_exit("Operation 'jmpc' requires 3 operands.", e, 1);
		else
		{
			bytes.push_back(0x12);
			
			if(line_tokens[1] == "eq")
				bytes.push_back(0x01);
			else if(line_tokens[1] == "ne")
				bytes.push_back(0x00);
			else if(line_tokens[1] == "gt")
				bytes.push_back(0x02);
			else if(line_tokens[1] == "lt")
				bytes.push_back(0x03);
			else
				assembler_print_error_and_exit("\'" + line_tokens[1] + "\' is an unrecognized condition.", e, 1);
		
			
			if(line_tokens[2] == "l")
			{
				jumps[(uint32_t)bytes.size() + offset] = line_tokens[3];
				
				bytes.push_back(0x02);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
			}
			else if(line_tokens[2] == "lo")
			{
				jumps[(uint32_t)bytes.size() + offset] = line_tokens[2];
				
				bytes.push_back(0x0B);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
			}
			else
			{
				std::vector<Byte> loc_bytes = assembler_assemble_location(line_tokens[2], line_tokens[3], e);
			
				bytes.insert(bytes.end(), loc_bytes.begin(), loc_bytes.end());
			}
			
		}
	}
	else if(line_tokens[0] == "jmpr")
	{
		if(line_tokens.size() < 5)
			assembler_print_error_and_exit("Operation 'jmpr' requires 4 operands.", e, 1);
		else
		{
			bytes.push_back(0x13);
		
			
			if(line_tokens[1] == "l")
			{
				jumps[(uint32_t)bytes.size() + offset] = line_tokens[2];
				
				bytes.push_back(0x02);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
			}
			else if(line_tokens[1] == "lo")
			{
				jumps[(uint32_t)bytes.size() + offset] = line_tokens[2];
				
				bytes.push_back(0x0B);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
			}
			else
			{
				std::vector<Byte> loc_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e);
			
				bytes.insert(bytes.end(), loc_bytes.begin(), loc_bytes.end());
			}
			
			if(line_tokens[3] == "l")
			{
				jumps[(uint32_t)bytes.size() + offset] = line_tokens[4];
				
				bytes.push_back(0x02);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
			}
			else if(line_tokens[3] == "lo")
			{
				jumps[(uint32_t)bytes.size() + offset] = line_tokens[2];
				
				bytes.push_back(0x0B);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
			}
			else
			{
				std::vector<Byte> loc_bytes = assembler_assemble_location(line_tokens[3], line_tokens[4], e);
			
				bytes.insert(bytes.end(), loc_bytes.begin(), loc_bytes.end());
			}
			
		}
	}
	else if(line_tokens[0] == "jmprc")
	{
		if(line_tokens.size() < 6)
			assembler_print_error_and_exit("Operation 'jmprc' requires 5 operands.", e, 1);
		else
		{
			bytes.push_back(0x14);
			
			if(line_tokens[1] == "eq")
				bytes.push_back(0x01);
			else if(line_tokens[1] == "ne")
				bytes.push_back(0x00);
			else if(line_tokens[1] == "gt")
				bytes.push_back(0x02);
			else if(line_tokens[1] == "lt")
				bytes.push_back(0x03);
			else
				assembler_print_error_and_exit("\'" + line_tokens[1] + "\' is an unrecognized condition.", e, 1);
		
			
			if(line_tokens[2] == "l")
			{
				jumps[(uint32_t)bytes.size() + offset] = line_tokens[3];
				
				bytes.push_back(0x02);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
			}
			else if(line_tokens[2] == "lo")
			{
				jumps[(uint32_t)bytes.size() + offset] = line_tokens[2];
				
				bytes.push_back(0x0B);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
			}
			else
			{
				std::vector<Byte> loc_bytes = assembler_assemble_location(line_tokens[2], line_tokens[3], e);
			
				bytes.insert(bytes.end(), loc_bytes.begin(), loc_bytes.end());
			}
			
			if(line_tokens[4] == "l")
			{
				jumps[(uint32_t)bytes.size() + offset] = line_tokens[5];
				
				bytes.push_back(0x02);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
			}
			else if(line_tokens[4] == "lo")
			{
				jumps[(uint32_t)bytes.size() + offset] = line_tokens[2];
				
				bytes.push_back(0x0B);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
			}
			else
			{
				std::vector<Byte> loc_bytes = assembler_assemble_location(line_tokens[4], line_tokens[5], e);
			
				bytes.insert(bytes.end(), loc_bytes.begin(), loc_bytes.end());
			}
			
		}
	}
	else if(line_tokens[0] == "rawb")
	{
		if(line_tokens.size() < 2)
			assembler_print_error_and_exit("Macro 'rawb' requires 1 operand.", e, 1);
		else
		{
			
			
			bytes.push_back(assembler_uint32_string_to_byte_array(line_tokens[1], e)[0]);
		}
	}
	else if(line_tokens[0] == "rawd")
	{
		if(line_tokens.size() < 2)
			assembler_print_error_and_exit("Macro 'rawd' requires 1 operand.", e, 1);
		else
		{
			std::vector<Byte> num_bytes = assembler_uint32_string_to_byte_array(line_tokens[1], e);
			
			bytes.insert(bytes.end(), num_bytes.begin(), num_bytes.end());
		}
	}
	else if(line_tokens[0] == "room")
	{
		if(line_tokens.size() < 2)
			assembler_print_error_and_exit("Macro 'room' requires 1 operand.", e, 1);
		else
		{
			
			for(int i = 0; i < (assembler_uint32_string_to_byte_array(line_tokens[1], e)[0]); i++)
			{
				bytes.push_back(0x00);
			}
		}
	}
	else if(line_tokens[0] == "raws")
	{
		if(line_tokens.size() < 2)
			assembler_print_error_and_exit("Macro 'raws' requires 1 operand.", e, 1);
		else
		{
			
			for(int z = 1; z < line_tokens.size(); z++)
			{
				for(int y = 0; y < line_tokens[z].length(); y++)
				{
					bytes.push_back((uint8_t)line_tokens[z][y]);
				}

				if(z != (line_tokens.size() - 1))
				{
					bytes.push_back((uint8_t)' ');
				}
			}

		}	
	}
	else if(line_tokens[0] == "iret")
	{
		bytes.push_back(0x15);
	}
	else if(line_tokens[0] == "uip")
	{
		if(line_tokens.size() < 5)
			assembler_print_error_and_exit("Operation 'uip' requires 4 operands.", e, 1);
		else
		{
			bytes.push_back(0x16);
			
			if(line_tokens[1] == "l")
			{
				jumps[(uint32_t)bytes.size() + offset] = line_tokens[2];
				
				bytes.push_back(0x02);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
			}
			else if(line_tokens[1] == "lo")
			{
				
				jumps[(uint32_t)bytes.size() + offset] = line_tokens[2];
				
				bytes.push_back(0x0B);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
			}
			else
			{
				std::vector<Byte> loc_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e);
			
				bytes.insert(bytes.end(), loc_bytes.begin(), loc_bytes.end());
			}

			std::vector<Byte> loc2_bytes = assembler_assemble_location(line_tokens[3], line_tokens[4], e, jumps, (uint32_t)bytes.size()+offset);
			
			//bytes.insert(bytes.end(), loc1_bytes.begin(), loc1_bytes.end());
			bytes.insert(bytes.end(), loc2_bytes.begin(), loc2_bytes.end());
		}
	}
	else if(line_tokens[0] == "tst")
	{
		bytes.push_back(0xFF);
	}
	else
	{
		//Instruction not defined
		std::cerr << "Error " << e.filename << ":" << e.line_num << ": Instruction \'" << line_tokens[0] << "\' not defined." << std::endl;
		exit(1);
	}
	
	p.bytes = bytes;
	p.jumps = jumps;
	p.labels = labels;
	
	return p;
}

std::vector<Byte> assembler_uint32_string_to_byte_array(std::string num, ErrorInformation e)
{
	uint32_t n;
	
	try
	{
		n = (uint32_t)std::stoul(num, nullptr, 0);
	}
	catch(...)
	{
		std::cerr << "Error " << e.filename << ":" << e.line_num << ": Integer required, not string." << std::endl;
		exit(1);
	}
	
	Byte b[4];
	
	*reinterpret_cast<uint32_t*>(b) = n;
	 
	std::vector<Byte> bytes;
	
	for(int i = 0; i < 4; i++)
	{
		bytes.push_back(b[i]);
	}
	
	return bytes;
}

std::vector<Byte> assembler_assemble_location(std::string loc_id, std::string location, ErrorInformation e)
{
	std::vector<Byte> bytes;
	
	if(loc_id == "r")
	{
		bytes.push_back(0x00);
		
		if(location == "ar1")
			bytes.push_back(0x00);
		else if(location == "ar2")
			bytes.push_back(0x01);
		else if(location == "ar3")
			bytes.push_back(0x02);
		else if(location == "ar4")
			bytes.push_back(0x03);
		else if(location == "ar5")
			bytes.push_back(0x04);
		else if(location == "ip")
			bytes.push_back(0x05);
		else if(location == "bp")
			bytes.push_back(0x06);
		else if(location == "sp")
			bytes.push_back(0x07);
		else if(location == "sb")
			bytes.push_back(0x08);
		else if(location == "cr1")
			bytes.push_back(0x0A);
		else if(location == "pr1")
			bytes.push_back(0x0B);
		else if(location == "pr2")
			bytes.push_back(0x0C);
		//No FPU yet
		else if(location == "flr1")
			bytes.push_back(0x0F);
		else if(location == "nr1")
			bytes.push_back(0x10);
		else if(location == "nr2")
			bytes.push_back(0x11);
		else if(location == "nr3")
			bytes.push_back(0x12);
		else if(location == "nr4")
			bytes.push_back(0x13);
		else if(location == "nr5")
			bytes.push_back(0x14);
		else if(location == "nr6")
			bytes.push_back(0x15);
		else if(location == "nr7")
			bytes.push_back(0x16);
		else if(location == "or1")
			bytes.push_back(0x17);
		else
			assembler_print_error_and_exit("\'" + location + "\' is not a valid register location.", e, 1);
		
		bytes.push_back(0x00);
		bytes.push_back(0x00);
		bytes.push_back(0x00);
	}
	else if(loc_id == "m")
	{
		bytes.push_back(0x01);
		
		std::vector<Byte> intbytes = assembler_uint32_string_to_byte_array(location, e);
		
		bytes.insert(bytes.end(), intbytes.begin(), intbytes.end());
	}
	else if(loc_id == "a")
	{
		bytes.push_back(0x02);
		
		std::vector<Byte> intbytes = assembler_uint32_string_to_byte_array(location, e);
		
		bytes.insert(bytes.end(), intbytes.begin(), intbytes.end());
	}
	else if(loc_id == "pr")
	{
		bytes.push_back(0x03);
		
		if(location == "ar1")
			bytes.push_back(0x00);
		else if(location == "ar2")
			bytes.push_back(0x01);
		else if(location == "ar3")
			bytes.push_back(0x02);
		else if(location == "ar4")
			bytes.push_back(0x03);
		else if(location == "ar5")
			bytes.push_back(0x04);
		//No FPU yet
		else if(location == "cr1")
			bytes.push_back(0x0A);
		else if(location == "pr1")
			bytes.push_back(0x0B);
		else if(location == "pr2")
			bytes.push_back(0x0C);
		//No FPU yet
		else if(location == "flr1")
			bytes.push_back(0x0F);
		else if(location == "nr1")
			bytes.push_back(0x10);
		else if(location == "nr2")
			bytes.push_back(0x11);
		else if(location == "nr3")
			bytes.push_back(0x12);
		else if(location == "nr4")
			bytes.push_back(0x13);
		else if(location == "nr5")
			bytes.push_back(0x14);
		else if(location == "nr6")
			bytes.push_back(0x15);
		else if(location == "nr7")
			bytes.push_back(0x16);
		else if(location == "or1")
			bytes.push_back(0x17);
		else
			assembler_print_error_and_exit("\'" + location + "\' is not a valid register location.", e, 1);
		
		bytes.push_back(0x00);
		bytes.push_back(0x00);
		bytes.push_back(0x00);
	}
	else if(loc_id == "pm")
	{
		bytes.push_back(0x04);
		
		
		
		std::vector<Byte> intbytes = assembler_uint32_string_to_byte_array(location, e);
		
		bytes.insert(bytes.end(), intbytes.begin(), intbytes.end());
	}
	else if(loc_id == "br")
	{
		bytes.push_back(0x05);
		
		if(location == "ar1")
			bytes.push_back(0x00);
		else if(location == "ar2")
			bytes.push_back(0x01);
		else if(location == "ar3")
			bytes.push_back(0x02);
		else if(location == "ar4")
			bytes.push_back(0x03);
		else if(location == "ar5")
			bytes.push_back(0x04);
		else if(location == "ip")
			bytes.push_back(0x05);
		else if(location == "bp")
			bytes.push_back(0x06);
		else if(location == "sp")
			bytes.push_back(0x07);
		else if(location == "sb")
			bytes.push_back(0x08);
		else if(location == "cr1")
			bytes.push_back(0x0A);
		else if(location == "pr1")
			bytes.push_back(0x0B);
		else if(location == "pr2")
			bytes.push_back(0x0C);
		//No FPU yet
		else if(location == "flr1")
			bytes.push_back(0x0F);
		else if(location == "nr1")
			bytes.push_back(0x10);
		else if(location == "nr2")
			bytes.push_back(0x11);
		else if(location == "nr3")
			bytes.push_back(0x12);
		else if(location == "nr4")
			bytes.push_back(0x13);
		else if(location == "nr5")
			bytes.push_back(0x14);
		else if(location == "nr6")
			bytes.push_back(0x15);
		else if(location == "nr7")
			bytes.push_back(0x16);
		else if(location == "or1")
			bytes.push_back(0x17);
		else
			assembler_print_error_and_exit("\'" + location + "\' is not a valid register location.", e, 1);
		
		bytes.push_back(0x00);
		bytes.push_back(0x00);
		bytes.push_back(0x00);
	}
	else if(loc_id == "bm")
	{
		bytes.push_back(0x06);
		
		std::vector<Byte> intbytes = assembler_uint32_string_to_byte_array(location, e);
		
		bytes.insert(bytes.end(), intbytes.begin(), intbytes.end());
	}
	else if(loc_id == "ba")
	{
		bytes.push_back(0x07);
		
		std::vector<Byte> intbytes = assembler_uint32_string_to_byte_array(location, e);
		
		bytes.insert(bytes.end(), intbytes.begin(), intbytes.end());
	}
	else if(loc_id == "bpr")
	{
		bytes.push_back(0x08);
		
		if(location == "ar1")
			bytes.push_back(0x00);
		else if(location == "ar2")
			bytes.push_back(0x01);
		else if(location == "ar3")
			bytes.push_back(0x02);
		else if(location == "ar4")
			bytes.push_back(0x03);
		else if(location == "ar5")
			bytes.push_back(0x04);
		//No FPU yet
		else if(location == "cr1")
			bytes.push_back(0x0A);
		else if(location == "pr1")
			bytes.push_back(0x0B);
		else if(location == "pr2")
			bytes.push_back(0x0C);
		//No FPU yet
		else if(location == "flr1")
			bytes.push_back(0x0F);
		else if(location == "nr1")
			bytes.push_back(0x10);
		else if(location == "nr2")
			bytes.push_back(0x11);
		else if(location == "nr3")
			bytes.push_back(0x12);
		else if(location == "nr4")
			bytes.push_back(0x13);
		else if(location == "nr5")
			bytes.push_back(0x14);
		else if(location == "nr6")
			bytes.push_back(0x15);
		else if(location == "nr7")
			bytes.push_back(0x16);
		else if(location == "or1")
			bytes.push_back(0x17);
		else
			assembler_print_error_and_exit("\'" + location + "\' is not a valid register location.", e, 1);
		
		bytes.push_back(0x00);
		bytes.push_back(0x00);
		bytes.push_back(0x00);
	}
	else if(loc_id == "bpm")
	{
		bytes.push_back(0x09);
		
		
		
		std::vector<Byte> intbytes = assembler_uint32_string_to_byte_array(location, e);
		
		bytes.insert(bytes.end(), intbytes.begin(), intbytes.end());
	}
	else
	{
		assembler_print_error_and_exit("\'" + loc_id + "\' is not a valid location type.", e, 1);
	}
	
	return bytes;
}

std::vector<Byte> assembler_assemble_location(std::string loc_id, std::string location, ErrorInformation e, std::map<uint32_t,std::string> jumps, uint32_t address)
{
	
	if(loc_id == "l")
	{
		std::vector<Byte> bytes;
				jumps[address] = location;
				
				bytes.push_back(0x02);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
		
		return bytes;
		
	}
	if(loc_id == "lo")
	{
		std::vector<Byte> bytes;
				jumps[address] = location;
				
				bytes.push_back(0x0B);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
				bytes.push_back(0x00);
		
		return bytes;
		
	}
	else
	{
		return assembler_assemble_location(loc_id, location, e);
	}
}
	
	
		
		
		
	
	

void write_bytes_to_file(char *filename, std::vector<Byte> bytes)
{
	FILE *fp;
	
	fp = fopen(filename, "wb");
	
	if(fp == NULL)
	{
		std::cerr << "Error: Unable to open \"" << filename << "\' for writing." << std::endl;
		exit(1);
	}
	else
	{
		for(int i = 0; i < bytes.size(); i++)
		{
			fwrite(&bytes[i], 1, 1, fp);
		}
	}
	fclose(fp);
}

std::vector<Byte> assembler_label_postprocessor(std::vector<Byte> bytes, std::map<std::string,std::vector<Byte>> labels, std::map<uint32_t,std::string> jumps)
{
	//Loop through jumps referencing labels
	for(const auto &j : jumps)
	{
		if(labels.find(j.second) == labels.end())
		{
			std::cerr << "Error: \'" << j.second << "\' is an undefined jump location." << std::endl;
			exit(1);
		}
		else
		{
			
			//Insert jump location
			bytes[j.first+1] = labels[j.second][0];
			bytes[j.first+2] = labels[j.second][1];
			bytes[j.first+3] = labels[j.second][2];
			bytes[j.first+4] = labels[j.second][3];
			
			
		}
	}
	
	return bytes;
}
			

int main(int argc, char **argv)
{
	if(argc  > 1 && std::string(argv[1]) == "-v")
	{
	
			
		printf("Assembler\n\nCopyright (c) Ian Duncan 2015\n\nLicensed under the MIT License\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n");
		return 0;
		
	}
	
	if(argc < 3)
	{
		std::cerr << "Error: No output and or input files specified." << std::endl;
		std::cerr << "Info: Run assembler as follows: " << argv[0] << " [input file] [ouptut file]" << std::endl;
		std::cerr << "or: " << argv[0] << " -v for version and copyright information" << std::endl;
		exit(1);
	}
	
	
	
	std::string input_file = std::string(argv[1]);
	
	
	std::ifstream input_file_stream;
	
	
	input_file_stream.open(argv[1], std::ifstream::in);
	
	
	if(!input_file_stream.good())
	{
		std::cerr << "Error: Unable to open \'" << input_file << "\' for reading." << std::endl;
		exit(1);
	}
	else
	{
		std::vector<Byte> bytes;
		std::map<uint32_t,std::string> jumps;
		std::map<std::string,std::vector<Byte>> labels;
		int i = 1;
		
		for( std::string line; getline( input_file_stream, line ); i++)
		{
			if(line.length() != 0)
			{
				if(line.at(0) == ';')
					; //It's a comment, ignore
				else
				{
					ErrorInformation e = { input_file, i };
					
					std::vector<std::string> line_tokens = assembler_parse_line(line);
					
					if(line_tokens.size() < 1)
						; //Ignore line, it's empty
					else
					{
						uint32_t offset = (uint32_t)bytes.size();

						

						

						

						ParserReturn p = assembler_parse_line_tokens(line_tokens, e, offset);
										
						bytes.insert(bytes.end(), p.bytes.begin(), p.bytes.end());
						
						//Concatenate maps
						for(const auto &j : p.jumps)
						{
							jumps[j.first] = j.second;
						}
						
						for(const auto &l : p.labels)
						{
							labels[l.first] = l.second;
						}
						
						
					}
				}
			}
			
			
				
		}
		
		bytes = assembler_label_postprocessor(bytes, labels, jumps);
		
		
		
		write_bytes_to_file(argv[2], bytes);
		
		
		
		
		
	}
	
	input_file_stream.close();
		
		
		
		
	
	
	
	
	
	
	
	
	
	
	
	return 0;
}
	





