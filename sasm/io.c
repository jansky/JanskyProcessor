#include "sasm.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int sasm_write_byte(FILE *fp, uint8_t byte)
{
    if(fp == NULL)
        return 0;
    
    uint8_t *byte_ptr = malloc(sizeof(uint8_t));
    
    if(byte_ptr == NULL)
        return 0;
    
    *byte_ptr = byte;
    
    return fwrite(byte_ptr, sizeof(uint8_t), 1, fp);
}

int sasm_write_word(FILE *fp, uint16_t word)
{
    if(fp == NULL)
        return 0;
    
    uint16_t *word_ptr = malloc(sizeof(uint16_t));
    
    if(word_ptr == NULL)
        return 0;
    
    *word_ptr = word;
    
    return fwrite(word_ptr, sizeof(uint16_t), 1, fp);
}

int sasm_write_dword(FILE *fp, uint32_t dword)
{
    if(fp == NULL)
        return 0;
    
    uint32_t *dword_ptr = malloc(sizeof(uint32_t));
    
    if(dword_ptr == NULL)
        return 0;
    
    *dword_ptr = dword;
    
    return fwrite(dword_ptr, sizeof(uint32_t), 1, fp);
}

int sasm_write_string(FILE *fp, char *string)
{
    if(fp == NULL || string == NULL)
        return 0;
        
    return fwrite(string, sizeof(char), strlen(string), fp); // Not null terminated
}

int sasm_write_object_file(FILE *fp, SASMSection *s_root)
{
    if(fp == NULL)
        return SASM_ERROR_IOERROR;
    
    if(s_root == NULL)
        return SASM_ERROR_UNKNOWNERROR;

    /* Close the last section if it hasn't already been closed */

    SASMSection *s_last = sasm_section_get_last_section(s_root);

    if(s_last == NULL)
        return SASM_ERROR_UNKNOWNERROR;
    
    if(!s_last->closed)
    {
        fclose(s_last->code_contents_fp);
    }
    
    /* Write the magic number */
    /* SACO = 4F 43 41 53 (little endian) */

    if(sasm_write_dword(fp, 0x4F434153) != 1)
        return SASM_ERROR_IOERROR;
    
    /* Write the number of sections */
    /* Since we don't know that right now (there could be a failure) we'll write a placeholder and return at the end. */

    long num_sections_location = ftell(fp);

    if(sasm_write_dword(fp, 0x0) != 1)
        return SASM_ERROR_NOERROR;
    
    /* Begin to write each section */

    uint32_t section_count = 0;

    SASMSection *s_next = s_root;

    while(s_next != NULL)
    {
        /* Section Serialization Format

           uint8_t              Name Length
           char*                Name
           uint32_t             Number of Labels
           SASMLocationLabel    Labels
           uint32_t             Number of Locations-to-Fill
           SASMLocationToFill   Locations-to-Fill
           uint32_t             Length of Code Section in Bytes
           char*                Code
        */

        if(s_next->name == NULL)
            return SASM_ERROR_STRINGERROR;
        
        uint8_t name_length = (uint8_t)strlen(s_next->name);

        if(sasm_write_byte(fp, name_length) != 1)
            return SASM_ERROR_IOERROR;
        
        if(fwrite(s_next->name, sizeof(char), name_length, fp) != name_length)
            return SASM_ERROR_IOERROR;
        
        /* We'll do the same placeholder thing for the number of labels and locations-to-fill */

        long num_labels_location = ftell(fp);

        uint32_t label_count = 0;

        if(sasm_write_dword(fp, 0x0) != 1)
            return SASM_ERROR_IOERROR;
        
        SASMLocationLabel *l_next = s_next->l_root;

        while(l_next != NULL)
        {
            if(l_next->name != NULL)
            {
                int error = sasm_location_label_serialize(fp, l_next);

                if(error != SASM_ERROR_NOERROR)
                    return error;
                
                label_count++;
            }

            l_next = l_next->next;
        }

        long num_locations_to_fill_location = ftell(fp);

        uint32_t location_to_fill_count = 0;

        if(sasm_write_dword(fp, 0x0) != 1)
            return SASM_ERROR_IOERROR;
        
        SASMLocationToFill *ltf_next = s_next->ltf_root;

        while(ltf_next != NULL)
        {
            if(ltf_next->name != NULL)
            {
                int error = sasm_location_to_fill_serialize(fp, ltf_next);

                if(error != SASM_ERROR_NOERROR)
                    return error;
                
                location_to_fill_count++;
            }

            ltf_next = ltf_next->next;
        }

        /* Write the compiled code */

        if(sasm_write_dword(fp, (uint32_t)s_next->code_contents_size) != 1)
            return SASM_ERROR_IOERROR;

        if(fwrite(s_next->code_contents, sizeof(char), s_next->code_contents_size, fp) != s_next->code_contents_size)
            return SASM_ERROR_IOERROR;
        
        /* Fill in the number of labels and locations-to-fill */

        long end_location = ftell(fp);

        if(fseek(fp, num_labels_location, SEEK_SET) != 0)
            return SASM_ERROR_IOERROR;
        
        if(sasm_write_dword(fp, label_count) != 1)
            return SASM_ERROR_IOERROR;
        
        if(fseek(fp, num_locations_to_fill_location, SEEK_SET) != 0)
            return SASM_ERROR_IOERROR;
        
        if(sasm_write_dword(fp, location_to_fill_count) != 1)
            return SASM_ERROR_IOERROR;
        
        /* And return to the end of the file */

        if(fseek(fp, end_location, SEEK_SET) != 0)
            return SASM_ERROR_IOERROR;
        
        section_count++;

        s_next = s_next->next;
    }

    /* Write the section count */

    if(fseek(fp, num_sections_location, SEEK_SET) != 0)
        return SASM_ERROR_IOERROR;
    
    if(sasm_write_dword(fp, section_count) != 1)
        return SASM_ERROR_IOERROR;
    
    return SASM_ERROR_NOERROR;
}
    
    
