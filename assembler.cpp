#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <map>
#include <stdexcept>
#include <stdint.h>
#include <stdlib.h>

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
void write_bytes_to_file(std::ofstream output, std::vector<Byte> bytes);
std::vector<Byte> assembler_label_postprocessor(std::vector<Byte> bytes, std::map<std::string,std::vector<Byte>> labels, std::map<uint32_t,std::string> jumps);


void assembler_print_error_and_exit(std::string error, ErrorInformation e, int exitcode)
{
	std::cout << "Error " << e.filename << ":" << e.line_num << ": " << error << std::endl;
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
	else if(line_tokens[0] == "put")
	{
		if(line_tokens.size() < 4)
			assembler_print_error_and_exit("Operation 'put' requires 4 operands.", e, 1);
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
			assembler_print_error_and_exit("Operation 'cpy' requires 5 operands.", e, 1);
		else
		{
			bytes.push_back(0x06);
			
			std::vector<Byte> loc1_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e);
			std::vector<Byte> loc2_bytes = assembler_assemble_location(line_tokens[3], line_tokens[4], e);
			
			bytes.insert(bytes.end(), loc1_bytes.begin(), loc1_bytes.end());
			bytes.insert(bytes.end(), loc2_bytes.begin(), loc2_bytes.end());
		}
	}
	else if(line_tokens[0] == "add")
	{
		if(line_tokens.size() < 5)
			assembler_print_error_and_exit("Operation 'add' requires 5 operands.", e, 1);
		else
		{
			bytes.push_back(0x07);
			
			std::vector<Byte> loc1_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e);
			std::vector<Byte> loc2_bytes = assembler_assemble_location(line_tokens[3], line_tokens[4], e);
			
			bytes.insert(bytes.end(), loc1_bytes.begin(), loc1_bytes.end());
			bytes.insert(bytes.end(), loc2_bytes.begin(), loc2_bytes.end());
		}
	}
	else if(line_tokens[0] == "sub")
	{
		if(line_tokens.size() < 5)
			assembler_print_error_and_exit("Operation 'sub' requires 5 operands.", e, 1);
		else
		{
			bytes.push_back(0x08);
			
			std::vector<Byte> loc1_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e);
			std::vector<Byte> loc2_bytes = assembler_assemble_location(line_tokens[3], line_tokens[4], e);
			
			bytes.insert(bytes.end(), loc1_bytes.begin(), loc1_bytes.end());
			bytes.insert(bytes.end(), loc2_bytes.begin(), loc2_bytes.end());
		}
	}
	else if(line_tokens[0] == "mul")
	{
		if(line_tokens.size() < 5)
			assembler_print_error_and_exit("Operation 'mul' requires 5 operands.", e, 1);
		else
		{
			bytes.push_back(0x09);
			
			std::vector<Byte> loc1_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e);
			std::vector<Byte> loc2_bytes = assembler_assemble_location(line_tokens[3], line_tokens[4], e);
			
			bytes.insert(bytes.end(), loc1_bytes.begin(), loc1_bytes.end());
			bytes.insert(bytes.end(), loc2_bytes.begin(), loc2_bytes.end());
		}
	}
	else if(line_tokens[0] == "div")
	{
		if(line_tokens.size() < 5)
			assembler_print_error_and_exit("Operation 'div' requires 5 operands.", e, 1);
		else
		{
			bytes.push_back(0x0A);
			
			std::vector<Byte> loc1_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e);
			std::vector<Byte> loc2_bytes = assembler_assemble_location(line_tokens[3], line_tokens[4], e);
			
			bytes.insert(bytes.end(), loc1_bytes.begin(), loc1_bytes.end());
			bytes.insert(bytes.end(), loc2_bytes.begin(), loc2_bytes.end());
		}
	}
	else if(line_tokens[0] == "push")
	{
		if(line_tokens.size() < 3)
			assembler_print_error_and_exit("Operaiton 'push' requires two operands.", e, 1);
		else
		{
			bytes.push_back(0x0B);
			
			std::vector<Byte> loc_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e);
			
			bytes.insert(bytes.end(), loc_bytes.begin(), loc_bytes.end());
		}
	}
	else if(line_tokens[0] == "pop")
	{
		if(line_tokens.size() < 3)
			assembler_print_error_and_exit("Operaiton 'pop' requires two operands.", e, 1);
		else
		{
			bytes.push_back(0x0C);
			
			std::vector<Byte> loc_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e);
			
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
			else
			{
				std::vector<Byte> loc_bytes = assembler_assemble_location(line_tokens[1], line_tokens[2], e);
			
				bytes.insert(bytes.end(), loc_bytes.begin(), loc_bytes.end());
			}
			
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
			
			lblbytes.push_back(0x02);
			
			for(int i = 0; i < 4; i++)
			{
				lblbytes.push_back(b[i]);
			}
			
			labels[line_tokens[1]] = lblbytes;
		}
	}
	else if(line_tokens[0] == "ret")
	{
		bytes.push_back(0x11);
	}
	else if(line_tokens[0] == "tst")
	{
		bytes.push_back(0xFF);
	}
	else
	{
		//Instruction not defined
		std::cout << "Error " << e.filename << ":" << e.line_num << ": Instruction \'" << line_tokens[0] << "\' not defined." << std::endl;
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
		std::cout << "Error " << e.filename << ":" << e.line_num << ": Integer required, not string." << std::endl;
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
	else
	{
		assembler_print_error_and_exit("\'" + loc_id + "\' is not a valid location type.", e, 1);
	}
	
	return bytes;
}
		
		
		
	
	

void write_bytes_to_file(char *filename, std::vector<Byte> bytes)
{
	FILE *fp;
	
	fp = fopen(filename, "wb");
	
	if(fp == NULL)
	{
		std::cout << "Error: Unable to open \"" << filename << "\' for writing." << std::endl;
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
			std::cout << "Error: \'" << j.second << "\' is an undefined jump location." << std::endl;
			exit(1);
		}
		else
		{
			
			//Insert jump location
			bytes[j.first] = labels[j.second][0];
			bytes[j.first+1] = labels[j.second][1];
			bytes[j.first+2] = labels[j.second][2];
			bytes[j.first+3] = labels[j.second][3];
			
			
		}
	}
	
	return bytes;
}
			

int main(int argc, char **argv)
{
	if(argc < 3)
	{
		std::cout << "Error: No output and or input files specified." << std::endl;
		std::cout << "Info: Run assembler as follows: " << argv[0] << " [input file] [ouptut file]" << std::endl;
		exit(1);
	}
	
	std::string input_file = std::string(argv[1]);
	
	
	std::ifstream input_file_stream;
	
	
	input_file_stream.open(argv[1], std::ifstream::in);
	
	
	if(!input_file_stream.good())
	{
		std::cout << "Error: Unable to open \'" << input_file << "\' for reading." << std::endl;
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
						ParserReturn p = assembler_parse_line_tokens(line_tokens, e, (uint32_t)bytes.size());
										
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
	






