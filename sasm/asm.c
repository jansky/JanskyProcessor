#include "sasm.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

int sasm_is_string_empty(const char *s)
{
  while (*s != '\0') {
    if (!isspace((unsigned char)*s))
      return 0;
    s++;
  }
  return 1;
}

uint32_t sasm_parse_number(char *number)
{
    // Get the kind of number we're parsing
    uint32_t result = 0;
    
    if(sscanf(number, "%i", &result) != 1)
        sasm_error = SASM_ERROR_NUMBERERROR;
        
    return result;
}

int sasm_write_one_location(FILE *fp, SASMLocationToFill *ltf_root, bool location_get)
{
    char *loctype1;
    char *locarg1;
            
    // Location 1
    loctype1 = strtok(NULL, " ");
            
    if(loctype1 == NULL)
        return SASM_ERROR_SYNTAXINVALID;
            
    locarg1 = strtok(NULL, " ");
            
    if(locarg1 == NULL)
        return SASM_ERROR_SYNTAXINVALID;
                
    if(location_get)
        return sasm_write_get_location(fp, loctype1, locarg1, ltf_root);
    else
        return sasm_write_put_location(fp, loctype1, locarg1, ltf_root);
}

int sasm_write_two_locations(FILE *fp, SASMLocationToFill *ltf_root, bool location1_get, bool location2_get)
{
    int result1;
    int result2;
            
    result1 = sasm_write_one_location(fp, ltf_root, location1_get);
    
    if(result1 != SASM_ERROR_NOERROR)
        return result1;
        
    result2 = sasm_write_one_location(fp, ltf_root, location2_get);
    
    if(result2 != SASM_ERROR_NOERROR)
        return result2;
        
    return SASM_ERROR_NOERROR;
}

/*int sasm_assemble_line(char *line, FILE *fp, SASMLocationLabel *ll_root, SASMLocationToFill *ltf_root)*/
int sasm_assemble_line(char *line, SASMSection *s_root)
{
    // Perform some checks to ensure everything is okay
    
    if(line == NULL)
        return SASM_ERROR_STRINGERROR;

    // Check if line is a comment
    
    if(sasm_is_string_empty(line))
        return SASM_ERROR_NOERROR;
    
    if(line[0] == ';')
        return SASM_ERROR_NOERROR; // We're done parsing this line

    /* Set up the environment from the current section, which is always the last section in the list. */

    SASMSection *s_current = sasm_section_get_last_section(s_root);

    if(s_current == NULL)
        return SASM_ERROR_UNKNOWNERROR;
    
    if(s_current->code_contents_fp == NULL || s_current->l_root == NULL || s_current->ltf_root == NULL)
        return SASM_ERROR_UNKNOWNERROR;
    
    FILE *fp = s_current->code_contents_fp;
    SASMLocationLabel *ll_root = s_current->l_root;
    SASMLocationToFill *ltf_root = s_current->ltf_root;

    // Check for instructions with no arguments
    
    if(strcmp(line, "hlt") == 0)
    {
        if(sasm_write_byte(fp,0x00) != 1) return SASM_ERROR_IOERROR;
        return SASM_ERROR_NOERROR;
    }
    
    if(strcmp(line, "ret") == 0)
    {
        if(sasm_write_byte(fp,0x11) != 1) return SASM_ERROR_IOERROR;
        return SASM_ERROR_NOERROR;
    }
    
    if(strcmp(line, "iret") == 0)
    {
        if(sasm_write_byte(fp,0x15) != 1) return SASM_ERROR_IOERROR;
        return SASM_ERROR_NOERROR;
    }
    
    if(strcmp(line, "tst") == 0)
    {
        if(sasm_write_byte(fp,0xFF) != 1) return SASM_ERROR_IOERROR;
        return SASM_ERROR_NOERROR;
    }
    
    // Now check for instructions that require arguments
    char *instruction = strtok(line, " ");
    
    if(instruction != NULL)
    {
        if(strcmp(instruction, "int") == 0)
        {
            if(sasm_write_byte(fp,0x03) != 1) return SASM_ERROR_IOERROR;
            
            // Write interrupt number
            char *interrupt = strtok(NULL, " ");
            
            if(interrupt == NULL)
                return SASM_ERROR_SYNTAXINVALID;
                
            uint8_t interrupt_number = (uint8_t)sasm_parse_number(interrupt);
            
            if(sasm_error != 0)
                return SASM_ERROR_NUMBERERROR;
                
            if(sasm_write_byte(fp,interrupt_number) != 1) return SASM_ERROR_IOERROR;
            
            return SASM_ERROR_NOERROR; // Operation was successul
        }
        else if(strcmp(instruction, "put") == 0)
        {
            if(sasm_write_byte(fp,0x05) != 1) return SASM_ERROR_IOERROR;
            
            //Write the location of where to put the data
            int result = sasm_write_one_location(fp, ltf_root, false);
            
            if(result != SASM_ERROR_NOERROR)
                return result;
                
            //Write the dword to put
            char *value = strtok(NULL, " ");
            
            if(value == NULL)
                return SASM_ERROR_SYNTAXINVALID;
                
            uint32_t value_number = sasm_parse_number(value);
            
            if(sasm_error != 0)
                return SASM_ERROR_NUMBERERROR;
                
            if(sasm_write_dword(fp,value_number) != 1) return SASM_ERROR_IOERROR;
            
            return SASM_ERROR_NOERROR; // Operation was successful
        }
        else if(strcmp(instruction, "cpy") == 0)
        {
            if(sasm_write_byte(fp,0x06) != 1) return SASM_ERROR_IOERROR;
            
            //Write the locations of where to copy the data
            return sasm_write_two_locations(fp, ltf_root, true, false);          
            
        }
        else if(strcmp(instruction, "add") == 0)
        {
            if(sasm_write_byte(fp,0x07) != 1) return SASM_ERROR_IOERROR;
            
            //Write the locations of where to copy the data
            return sasm_write_two_locations(fp, ltf_root, false, true);
        }
        else if(strcmp(instruction, "sub") == 0)
        {
            if(sasm_write_byte(fp,0x08) != 1) return SASM_ERROR_IOERROR;
            
            //Write the locations of where to copy the data
            return sasm_write_two_locations(fp, ltf_root, false, true);
        }
        else if(strcmp(instruction, "mul") == 0)
        {
            if(sasm_write_byte(fp,0x09) != 1) return SASM_ERROR_IOERROR;
            
            //Write the locations of where to copy the data
            return sasm_write_two_locations(fp, ltf_root, false, true);
        }
        else if(strcmp(instruction, "div") == 0)
        {
            if(sasm_write_byte(fp,0x0A) != 1) return SASM_ERROR_IOERROR;
            
            //Write the locations of where to copy the data
            return sasm_write_two_locations(fp, ltf_root, false, true);
        }
        else if(strcmp(instruction, "push") == 0)
        {
            if(sasm_write_byte(fp,0x0B) != 1) return SASM_ERROR_IOERROR;
            
            //Write the locations of where to copy the data
            return sasm_write_one_location(fp, ltf_root, true);
        }
        else if(strcmp(instruction, "push") == 0)
        {
            if(sasm_write_byte(fp,0x0B) != 1) return SASM_ERROR_IOERROR;
            
            //Write the locations of where to copy the data
            return sasm_write_one_location(fp, ltf_root, true);
        }
        else if(strcmp(instruction, "pop") == 0)
        {
            if(sasm_write_byte(fp,0x0C) != 1) return SASM_ERROR_IOERROR;
            
            //Write the locations of where to copy the data
            return sasm_write_one_location(fp, ltf_root, false);
        }
        else if(strcmp(instruction, "jmp") == 0)
        {
            if(sasm_write_byte(fp,0x0F) != 1) return SASM_ERROR_IOERROR;
            
            //Write the locations of where to copy the data
            return sasm_write_one_location(fp, ltf_root, true);
        }
        else if(strcmp(instruction, "cmp") == 0)
        {
            if(sasm_write_byte(fp,0x10) != 1) return SASM_ERROR_IOERROR;
            
            //Write the locations of where to copy the data
            return sasm_write_two_locations(fp, ltf_root, true, true);
        }
        else if(strcmp(instruction, "jmpc") == 0)
        {
            if(sasm_write_byte(fp,0x12) != 1) return SASM_ERROR_IOERROR;
            
            //Write condition byte
            char *condition = strtok(NULL, " ");
            
            if(condition == NULL)
                return SASM_ERROR_STRINGERROR;
                
            if(strcmp(condition, "eq") == 0)
            {
                if(sasm_write_byte(fp,0x01) != 1) return SASM_ERROR_IOERROR;
            }
            else if(strcmp(condition, "ne") == 0)
            {
                if(sasm_write_byte(fp,0x00) != 1) return SASM_ERROR_IOERROR;
            }
            else if(strcmp(condition, "gt") == 0)
            {
                if(sasm_write_byte(fp,0x02) != 1) return SASM_ERROR_IOERROR;
            }
            else if(strcmp(condition, "lt") == 0)
            {
                if(sasm_write_byte(fp,0x03) != 1) return SASM_ERROR_IOERROR;
            }
            else
            {
                return SASM_ERROR_SYNTAXINVALID;
            }
            
            
            //Write the locations of where to copy the data
            return sasm_write_one_location(fp, ltf_root, true);
        }
        else if(strcmp(instruction, "jmpr") == 0)
        {
            if(sasm_write_byte(fp,0x13) != 1) return SASM_ERROR_IOERROR;
            
            return sasm_write_two_locations(fp, ltf_root, true, true);
                      
        }
        else if(strcmp(instruction, "jmprc") == 0)
        {
            if(sasm_write_byte(fp,0x14) != 1) return SASM_ERROR_IOERROR;
            
            //Write condition byte
            char *condition = strtok(NULL, " ");
            
            if(condition == NULL)
                return SASM_ERROR_STRINGERROR;
                
            if(strcmp(condition, "eq") == 0)
            {
                if(sasm_write_byte(fp,0x01) != 1) return SASM_ERROR_IOERROR;
            }
            else if(strcmp(condition, "ne") == 0)
            {
                if(sasm_write_byte(fp,0x00) != 1) return SASM_ERROR_IOERROR;
            }
            else if(strcmp(condition, "gt") == 0)
            {
                if(sasm_write_byte(fp,0x02) != 1) return SASM_ERROR_IOERROR;
            }
            else if(strcmp(condition, "lt") == 0)
            {
                if(sasm_write_byte(fp,0x03) != 1) return SASM_ERROR_IOERROR;
            }
            else
            {
                return SASM_ERROR_SYNTAXINVALID;
            }
            
            
            //Write the locations of where to copy the data
            return sasm_write_two_locations(fp, ltf_root, true, true);
        }
        else if(strcmp(instruction, "uip") == 0)
        {
            if(sasm_write_byte(fp,0x16) != 1) return SASM_ERROR_IOERROR;
            
            return sasm_write_two_locations(fp, ltf_root, true, true);
        }
        // Now for some assembler helper functions
        else if(strcmp(instruction, "sec") == 0)
        {
            char *section_name = strtok(NULL, " ");

            if(section_name == NULL)
                return SASM_ERROR_SYNTAXINVALID;
            
            /* Close the current section */

            fclose(s_current->code_contents_fp);

            s_current->closed = true;

            /* Start a new section */

            SASMSection *s_new = sasm_section_create(section_name);

            if(s_new == NULL)
                return SASM_ERROR_UNKNOWNERROR;
            
            if(sasm_section_add(s_root, s_new) != SASM_ERROR_NOERROR)
                return SASM_ERROR_UNKNOWNERROR;
            
            return SASM_ERROR_NOERROR; // Operation was successful
        }
        else if(strcmp(instruction, "lbl") == 0)
        {
            char *label_name = strtok(NULL, " ");
            
            if(label_name == NULL)
                return SASM_ERROR_SYNTAXINVALID;
                
            // Create a new SASMLocationLabel
            
            long location = ftell(fp); // Get label location
        
            if(location == -1)
                return SASM_ERROR_IOERROR;
            
            SASMLocationLabel *new = sasm_location_label_create(label_name, (uint32_t)location, 0);
            
            if(!sasm_location_label_add(ll_root, new)) return SASM_ERROR_LABELERROR;
            
            // Output label location
            printf("%s: %s %x\n", s_current->name, label_name, (uint32_t)location);
            
            return SASM_ERROR_NOERROR; // Operation was successful
        }
        else if(strcmp(instruction, "glbl") == 0)
        {
            char *label_name = strtok(NULL, " ");
            
            if(label_name == NULL)
                return SASM_ERROR_SYNTAXINVALID;
                
            // Create a new SASMLocationLabel
            
            long location = ftell(fp); // Get label location
        
            if(location == -1)
                return SASM_ERROR_IOERROR;
            
            SASMLocationLabel *new = sasm_location_label_create(label_name, (uint32_t)location, 1);
            
            if(!sasm_location_label_add(ll_root, new)) return SASM_ERROR_LABELERROR;
            
            // Output label location
            printf("(global) %s: %s %x\n", s_current->name, label_name, (uint32_t)location);
            
            return SASM_ERROR_NOERROR; // Operation was successful
        }
        else if(strcmp(instruction, "flbl") == 0)
        {
            char *label_name = strtok(NULL, " ");
            
            if(label_name == NULL)
                return SASM_ERROR_SYNTAXINVALID;
                
            // Create a new SASMLocationLabel
            
            long location = ftell(fp); // Get label location
        
            if(location == -1)
                return SASM_ERROR_IOERROR;
            
            SASMLocationLabel *new = sasm_location_label_create(label_name, (uint32_t)location, 2);
            
            if(!sasm_location_label_add(ll_root, new)) return SASM_ERROR_LABELERROR;
            
            // Output label location
            printf("(file global) %s: %s %x\n", s_current->name, label_name, (uint32_t)location);
            
            return SASM_ERROR_NOERROR; // Operation was successful
        }
        else if(strcmp(instruction, "room") == 0)
        {
        	char *num_bytes = strtok(NULL, " ");
        	
        	if(num_bytes == NULL)
        		return SASM_ERROR_SYNTAXINVALID;
        		
        	uint32_t num_bytes_number = sasm_parse_number(num_bytes);
            
            if(sasm_error != 0)
                return SASM_ERROR_NUMBERERROR;
            
            for(uint32_t i = 0; i < num_bytes_number;i++)
                if(sasm_write_byte(fp,0x00) != 1) return SASM_ERROR_IOERROR;
            
            return SASM_ERROR_NOERROR; // Operation was successful
        }
        else if(strcmp(instruction, "rawb") == 0)
        {
        	char *byte = strtok(NULL, " ");
        	
        	if(byte == NULL)
        		return SASM_ERROR_SYNTAXINVALID;
        		
        	uint32_t byte_number = sasm_parse_number(byte);
            
            if(sasm_error != 0)
                return SASM_ERROR_NUMBERERROR;
                
            if(byte_number > 255)
                return SASM_ERROR_SYNTAXINVALID;
            
          
            if(sasm_write_byte(fp,(uint8_t)byte_number) != 1) return SASM_ERROR_IOERROR;
            
            return SASM_ERROR_NOERROR; // Operation was successful
        }
        else if(strcmp(instruction, "rawd") == 0)
        {
        	char *dword = strtok(NULL, " ");
        	
        	if(dword == NULL)
        		return SASM_ERROR_SYNTAXINVALID;
        		
        	uint32_t dword_number = sasm_parse_number(dword);
            
            if(sasm_error != 0)
                return SASM_ERROR_NUMBERERROR;
                
            if(sasm_write_dword(fp,dword_number) != 1) return SASM_ERROR_IOERROR;
            
            return SASM_ERROR_NOERROR; // Operation was successful
        }
        else if(strcmp(instruction, "raws") == 0)
        {
            char *string = strtok(NULL, "");
            
            if(string == NULL)
                return SASM_ERROR_SYNTAXINVALID;
                
            if(sasm_write_string(fp, string) != strlen(string)) return SASM_ERROR_IOERROR;
            
            return SASM_ERROR_NOERROR; // Operation was successful
        }
        else
        {
            return SASM_ERROR_SYNTAXINVALID; // Instruction not valid
        }
    }
    else
    {
        return SASM_ERROR_STRINGERROR;
    }
            
            
    
    return SASM_ERROR_UNKNOWNERROR; // We shouldn't end up here
}
    
    
    
    
    
