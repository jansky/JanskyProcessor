#include "sasm.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

SASMLocationLabel *sasm_location_label_create(char *name, uint32_t location)
{

    SASMLocationLabel *new = malloc(sizeof(SASMLocationLabel));
    
    if(new == NULL)
        return NULL;
        
    // Set parameters
    new->name = name;
    new->location = location;
    
    return new;
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
    int max = sasm_location_label_get_max(root);
    
    if(max == -1)
        return false; // Something went wrong
        
    SASMLocationLabel *end = sasm_location_label_get(root, max);
    
    if(end == NULL)
        return false;
    
    end->next = new;
    
    return true;
}

SASMLocationToFill *sasm_location_to_fill_create(char *name, uint32_t location)
{

    SASMLocationToFill *new = malloc(sizeof(SASMLocationToFill));
    
    if(new == NULL)
        return NULL;
        
    // Set parameters
    new->name = name;
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
    int max = sasm_location_to_fill_get_max(root);
    
    if(max == -1)
        return false; // Something went wrong
        
    SASMLocationToFill *end = sasm_location_to_fill_get(root, max);
    
    if(end == NULL)
        return false;
    
    end->next = new;
    
    return true;
}
