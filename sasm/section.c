#include "sasm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SASMSection *sasm_section_create(char *name)
{
    if(name == NULL)
        return NULL;
    
    SASMSection *s_new = malloc(sizeof(SASMSection));

    if(s_new == NULL)
        return NULL;
    
    s_new->name = strdup(name);

    if(s_new->name == NULL)
        return NULL;
    
    s_new->code_contents_fp = open_memstream(&(s_new->code_contents), &(s_new->code_contents_size));

    if(s_new->code_contents_fp == NULL)
        return NULL;
    
    s_new->l_root = sasm_location_label_create(NULL, 0, 0);

    s_new->ltf_root = sasm_location_to_fill_create(NULL, 0);

    if(s_new->l_root == NULL || s_new->ltf_root == NULL)
        return NULL;

    s_new->closed = false;
    
    s_new->next = NULL;
    
    return s_new;
}

int sasm_section_add(SASMSection *s_root, SASMSection *s_new)
{
    if(s_root == NULL || s_new == NULL)
    {
        return SASM_ERROR_LABELERROR;
    }

    SASMSection *next = s_root;

    while(next != NULL)
    {
        if(next->next == NULL)
        {
            next->next = s_new;
            break;
        }

        next = next->next;
    }

    return SASM_ERROR_NOERROR;
}

SASMSection *sasm_section_get_last_section(SASMSection *s_root)
{
    SASMSection *next = s_root;

    while(next != NULL)
    {
        if(next->next == NULL)
            return next;
        
        next = next->next;
    }

    return NULL;
}