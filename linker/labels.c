#include "linker.h"

#include <stdlib.h>
#include <string.h>

LinkerLocationLabel *linker_location_label_create(char *name, uint32_t location, int section_id, char global)
{
    LinkerLocationLabel *label = malloc(sizeof(LinkerLocationLabel));

    if(label == NULL) return NULL;

    if(name != NULL)
    {
        label->name = strdup(name);

        if(label->name == NULL) return NULL;
    }
    else
    {
        label->name = NULL;
    }

    label->location = location;
    label->section_id = section_id;
    label->next = NULL;
    label->global = global;

    return label;
}

size_t linker_location_label_max(LinkerLocationLabel *l_root)
{
    size_t count = 0;

    LinkerLocationLabel *next = l_root;

    while(next != NULL)
    {
        count++;

        next = next->next;
    }

    return count;
}

int linker_location_label_add(LinkerLocationLabel *l_root, LinkerLocationLabel *l_new)
{
    if(l_root == NULL || l_new == NULL)
        return SASM_ERROR_LABELERROR;
    
    LinkerLocationLabel *next = l_root;

    while(next != NULL)
    {
        if(next->next == NULL)
        {
            next->next = l_new;
            break;
        }
        else
        {        
            next = next->next;
        }
        
    }

    return SASM_ERROR_NOERROR;
}

LinkerLocationLabel *linker_location_label_get_by_name(LinkerLocationLabel *l_root, char *name)
{
    if(l_root == NULL || name == NULL)
        return NULL;
    
    LinkerLocationLabel *next = l_root;

    while(next != NULL)
    {
        if(next->name != NULL)
        {
            if(strcmp(next->name,name) == 0)
                return next;
        }

        next = next->next;
    }

    return NULL;    
}

LinkerLocationToFill *linker_location_to_fill_create(char *name, uint32_t location)
{
    LinkerLocationToFill *ltf = malloc(sizeof(LinkerLocationToFill));

    if(ltf == NULL)
        return NULL;
    
    if(name != NULL)
    {
        ltf->name = strdup(name);

        if(ltf->name == NULL)
            return NULL;
    }
    else
    {
        ltf->name == NULL;
    }

    ltf->location = location;
    ltf->next = NULL;

    return ltf;
}

int linker_location_to_fill_add(LinkerLocationToFill *ltf_root, LinkerLocationToFill *ltf_new)
{
    if(ltf_root == NULL || ltf_new == NULL)
        return SASM_ERROR_LABELERROR;
    
    LinkerLocationToFill *next = ltf_root;

    while(next != NULL)
    {
        if(next->next == NULL)
        {
            next->next = ltf_new;
            break;
        }
        
        next = next->next;
        
    }

    return SASM_ERROR_NOERROR;
}

LinkerLocationLabel *linker_location_labels_deserialize(FILE *fp, int section_id, LinkerLocationLabel *global_root)
{
    if(fp == NULL || global_root == NULL)
        return NULL;
    
    LinkerLocationLabel *l_root = linker_location_label_create(NULL, 0, -1, 0);

    if(l_root == NULL)
        return NULL;

    uint32_t labels_count = 0;

    if(fread(&labels_count, sizeof(uint32_t), 1, fp) != 1)
        return NULL;
    
    for(uint32_t i = 0; i < labels_count; i++)
    {
        uint16_t name_length = 0;

        if(fread(&name_length, sizeof(uint16_t), 1, fp) != 1)
            return NULL;
        
        char *label_name = calloc(name_length + 1, sizeof(char));

        if(fread(label_name, sizeof(char), name_length, fp) != name_length)
            return NULL;
        
        uint8_t global = 0;

        if(fread(&global, sizeof(uint8_t), 1, fp) != 1)
            return NULL;
        
        uint32_t location = 0;

        if(fread(&location, sizeof(uint32_t), 1, fp) != 1)
            return NULL;
        
        LinkerLocationLabel *l_new = linker_location_label_create(strdup(label_name), location, section_id, global);

        if(l_new == NULL)
            return NULL;

        if(l_new->name == NULL)
            return NULL;
        
        if(global == 1)
        {
            if(linker_location_label_add(global_root, l_new) != SASM_ERROR_NOERROR)
                return NULL;
        }
        else
        {
            if(linker_location_label_add(l_root, l_new) != SASM_ERROR_NOERROR)
                return NULL;
        }

        free(label_name);
    }

    return l_root;
}

LinkerLocationToFill *linker_locations_to_fill_deserialize(FILE *fp)
{
    if(fp == NULL)
        return NULL;
    
    LinkerLocationToFill *ltf_root = linker_location_to_fill_create(NULL, 0);

    if(ltf_root == NULL)
        return NULL;

    uint32_t ltf_count = 0;

    if(fread(&ltf_count, sizeof(uint32_t), 1, fp) != 1)
        return NULL;
    
    for(uint32_t i = 0; i < ltf_count; i++)
    {
        uint16_t name_length = 0;

        if(fread(&name_length, sizeof(uint16_t), 1, fp) != 1)
            return NULL;
        
        char *ltf_name = calloc(name_length + 1, sizeof(char));

        if(fread(ltf_name, sizeof(char), name_length, fp) != name_length)
            return NULL;
        
        uint32_t location = 0;

        if(fread(&location, sizeof(uint32_t), 1, fp) != 1)
            return NULL;

        LinkerLocationToFill *ltf_new = linker_location_to_fill_create(strdup(ltf_name), location);

        if(ltf_new == NULL)
            return NULL;

        if(ltf_new->name == NULL)
            return NULL;

        if(linker_location_to_fill_add(ltf_root, ltf_new) != SASM_ERROR_NOERROR)
            return NULL;

        free(ltf_name);
    }

    return ltf_root;
}