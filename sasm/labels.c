#include "sasm.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

SASMLocationLabel *sasm_location_label_create(char *name, uint32_t location, char global)
{

    SASMLocationLabel *new = malloc(sizeof(SASMLocationLabel));
    
    if(new == NULL)
        return NULL;
        
    // Set parameters
    if(name != NULL) // The string needs to be copied or it gets corrupted due to strtok things
    {
        char *new_name = malloc(strlen(name) + 1);
        
        if(new_name == NULL)
            return NULL;
        
        new->name = strcpy(new_name, name);
    }
    else
    {
        new->name = NULL;
    }

    new->global = global;

    new->location = location;

    new->next = NULL;
    
    return new;
}

SASMLocationLabel *sasm_location_label_get_by_name(SASMLocationLabel *root, char *name)
{
    SASMLocationLabel *next = root;
    
    while(next != NULL)
    {
        if(next->name != NULL)
        {
            
            if(strcmp(next->name, name) == 0)
            {
                return next;
            }
        }
        
        next = next->next;
    }
    
    return NULL;
}
    
int sasm_location_label_get_max(SASMLocationLabel *root)
{
    SASMLocationLabel *next = root;
    int i = 0;
    
    // Check if root is NULL
    if(root == NULL)
        return -1; // Let the caller know something went wrong
    
    while(next != NULL)
    {
        next = next->next;
        
        i++;
    }
    
    i--;
    
    return i;
}

SASMLocationLabel *sasm_location_label_get(SASMLocationLabel *root, int id)
{
    int i = 0;
    
    SASMLocationLabel *next = root;
    
    while(next != NULL)
    {
        if(id == i)
            return next;
            
        next = next->next;
        
        i++;
    }
    
    return NULL;
}

bool sasm_location_label_add(SASMLocationLabel *root, SASMLocationLabel *new)
{
    SASMLocationLabel *next = root;
    
    if(next == NULL)
        return false;
    
    while(next != NULL)
    {
        if(next->next == NULL)
        {
            next->next = new;
            break;
        }
        
        next = next->next;
    }
            
    
    return true;
}

SASMLocationToFill *sasm_location_to_fill_create(char *name, uint32_t location)
{

    SASMLocationToFill *new = malloc(sizeof(SASMLocationToFill));
    
    if(new == NULL)
        return NULL;
        
    // Set parameters
    if(name != NULL) // The string needs to be copied or it gets corrupted due to strtok things
    {
        char *new_name = malloc(strlen(name) + 1);
        
        if(new_name == NULL)
            return NULL;
        
        new->name = strcpy(new_name, name);
    }
    else
    {
        new->name = NULL;
    }
    new->next = NULL;
    new->location = location;
    
    return new;
}
    
int sasm_location_to_fill_get_max(SASMLocationToFill *root)
{
    SASMLocationToFill *next = root;
    int i = 0;
    
    // Check if root is NULL
    if(root == NULL)
        return -1; // Let the caller know something went wrong
    
    while(next != NULL)
    {
        next = next->next;
        
        i++;
    }
    
    i--;
    
    return i;
}

SASMLocationToFill *sasm_location_to_fill_get(SASMLocationToFill *root, int id)
{
    int i = 0;
    
    SASMLocationToFill *next = root;
    
    while(next != NULL)
    {
        if(id == i)
            return next;
            
        next = next->next;
        
        i++;
    }
    
    return NULL;
}

bool sasm_location_to_fill_add(SASMLocationToFill *root, SASMLocationToFill *new)
{
    SASMLocationToFill *next = root;
    
    if(next == NULL)
        return false;
    
    while(next != NULL)
    {
        if(next->next == NULL)
        {
            next->next = new;
            break;
        }
        
        next = next->next;
    }
    
    return true;
}

int sasm_fill_in_labels(FILE *fp, SASMLocationLabel *ll_root, SASMLocationToFill *ltf_root)
{
    // First perform some checks to ensure everything is okay
    if(fp == NULL)
        return SASM_ERROR_IOERROR;
        
    if(ll_root == NULL || ltf_root == NULL)
        return SASM_ERROR_LABELERROR;
    
    // For each SASMLocationToFill search for the corresponding SASMLocationLabel and fill in the blank
    SASMLocationToFill *next = ltf_root;
    
    while(next != NULL)
    {
        if(next->name != NULL) // Root element's name is always NULL
        {        
            SASMLocationLabel *location = sasm_location_label_get_by_name(ll_root, next->name);
            
            if(location == NULL)
                return SASM_ERROR_LABELERROR;
            
            // Seek to place where location must be filled in and write dword
            if(fseek(fp, next->location, SEEK_SET) != 0)
                return SASM_ERROR_IOERROR;
                
            if(sasm_write_dword(fp, location->location) != 1) return SASM_ERROR_IOERROR;
        }
        
        next = next->next;
     }
     
     return SASM_ERROR_NOERROR;
}

int sasm_location_label_serialize(FILE *fp, SASMLocationLabel *ll)
{
    if(fp == NULL)
        return SASM_ERROR_IOERROR;
    
    if(ll == NULL)
        return SASM_ERROR_LABELERROR;
    
    if(ll->name == NULL)
        return SASM_ERROR_LABELERROR;
    
    /* The format for label serialization:

        uint16_t Name Length
        char*    Name (No Null Terminator)
        uint8_t     1 if Global, 0 if Local
        uint32_t Location
    */

    if(sasm_write_word(fp, (uint16_t)strlen(ll->name)) != 1) return SASM_ERROR_IOERROR;

    if(fwrite(ll->name, sizeof(char), strlen(ll->name), fp) != strlen(ll->name)) return SASM_ERROR_IOERROR;

    if(sasm_write_byte(fp, (uint8_t)ll->global) != 1) return SASM_ERROR_IOERROR;

    if(sasm_write_dword(fp, ll->location) != 1) return SASM_ERROR_IOERROR;

    return SASM_ERROR_NOERROR;
}

int sasm_location_to_fill_serialize(FILE *fp, SASMLocationToFill *ltf)
{
    if(fp == NULL)
        return SASM_ERROR_IOERROR;
    
    if(ltf == NULL)
        return SASM_ERROR_LABELERROR;
    
    if(ltf->name == NULL)
        return SASM_ERROR_LABELERROR;
    
    /* The format for location-to-fill serialization:

        uint16_t Name Length
        char*    Name (No Null Terminator)
        uint32_t Location
    */

    if(sasm_write_word(fp, (uint16_t)strlen(ltf->name)) != 1) return SASM_ERROR_IOERROR;

    if(fwrite(ltf->name, sizeof(char), strlen(ltf->name), fp) != strlen(ltf->name)) return SASM_ERROR_IOERROR;

    if(sasm_write_dword(fp, ltf->location) != 1) return SASM_ERROR_IOERROR;

    return SASM_ERROR_NOERROR;
}
