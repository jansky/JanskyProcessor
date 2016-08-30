#include "sasm.h"
#include <stdlib.h>
#include <stdio.h>

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
    
    
