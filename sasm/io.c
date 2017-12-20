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

int sasm_write_object_file(FILE *fp, SASMLocationLabel *ll_root, SASMLocationToFill *ltf_root, char *code, size_t code_length)
{
    if(fp == NULL)
        return SASM_ERROR_IOERROR;
    
    if(ll_root == NULL || ltf_root == NULL)
        return SASM_ERROR_LABELERROR;
    
    if(code == NULL)
        return SASM_ERROR_STRINGERROR;
    
    /* Write the magic number */
    /* SACO = 4F 43 41 53 (little endian) */

    if(sasm_write_dword(fp, 0x4F434153) != 1)
        return SASM_ERROR_IOERROR;
    
    /* Labels

       Section Format:

       uint32_t                 Number of Labels
       SASMLocationLabel        ...

    */

    uint32_t label_count = 0;

    SASMLocationLabel *l_next = ll_root;

    while(l_next != NULL)
    {
        if(l_next->name != NULL)
            label_count++;
        
        l_next = l_next->next;
    }

    if(sasm_write_dword(fp, label_count) != 1)
        return SASM_ERROR_IOERROR;
    
    l_next = ll_root;

    while(l_next != NULL)
    {
        if(l_next->name != NULL)
        {
            int error = sasm_location_label_serialize(fp, l_next);

            if(error != SASM_ERROR_NOERROR)
                return error;
        }

        l_next = l_next->next;
    }

    /* Locations-To-Fill

    Section Format:

       uint32_t                 Number of Locations-to-Fill
       SASMLocationToFill       ...

    */

    uint32_t ltf_count = 0;

    SASMLocationToFill *ltf_next = ltf_root;

    while(ltf_next != NULL)
    {
        if(ltf_next->name != NULL)
            ltf_count++;
        
        ltf_next = ltf_next->next;
    }

    if(sasm_write_dword(fp, ltf_count) != 1)
        return SASM_ERROR_IOERROR;
    
    ltf_next = ltf_root;

    while(ltf_next != NULL)
    {
        if(ltf_next->name != NULL)
        {
            int error = sasm_location_to_fill_serialize(fp, ltf_next);

            if(error != SASM_ERROR_NOERROR)
                return error;
        }

        ltf_next = ltf_next->next;
    }

    /* Compiled Code */

    if(fwrite(code, sizeof(char), code_length, fp) != code_length)
        return SASM_ERROR_IOERROR;
    
    return SASM_ERROR_NOERROR;
}
    
    
