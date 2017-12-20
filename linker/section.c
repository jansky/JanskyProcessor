#include <linker.h>

#include <stdlib.h>
#include <string.h>

LinkerSection *linker_section_create(char *name, FILE *fp, LinkerLocationLabel *l_root, LinkerLocationToFill *ltf_root)
{
    LinkerSection *section = malloc(sizeof(LinkerSection));

    if(section == NULL)
        return NULL;
    
    if(name != NULL)
    {
        section->name = strdup(name);

        if(section->name == NULL)
            return NULL;
    }
    else
    {
        section->name = NULL;
    }

    section->fp = fp;
    section->l_root = l_root;
    section->ltf_root = ltf_root;
    section->location_offset = 0;

    return section;
}

int linker_section_add(LinkerSection *s_root, LinkerSection *s_new)
{
    if(s_root == NULL || s_new == NULL)
        return SASM_ERROR_NOERROR;
    
    LinkerSection *next = s_root;

    while(next != NULL)
    {
        if(next->next == NULL)
        {
            next->next = s_new;
            break;
        }
        else
        {
            next = next->next;
        }
    }

    return SASM_ERROR_NOERROR;
}

LinkerSection *linker_section_get_by_id(LinkerSection *s_root, size_t id)
{
    if(s_root == NULL || id < 0)
        return NULL;
    
    if(id == 0)
        return s_root;
    
    LinkerSection *next = s_root;

    size_t count = 1;

    while(next != NULL)
    {
        if(count == id)
            return next;
        
        next = next->next;
    }

    return NULL;
}

LinkerSection *linker_section_create_from_file(char *name, int section_id, LinkerLocationLabel *global_root)
{
    if(name == NULL)
        return NULL;
    
    FILE *fp = fopen(name, "r+b");

    if(fp == NULL)
    {
        fprintf(stderr, "Error: %s is not a valid SASM Compiled Object file.\n", name);
        return NULL;
    }
    
    /* Verify magic number */
    /* SACO = 4F 43 41 53 (little endian) */

    uint32_t magic_num = 0;

    if(fread(&magic_num, sizeof(uint32_t), 1, fp) != 1)
    {
        fprintf(stderr, "Error: %s is not a valid SASM Compiled Object file.\n", name);
        return NULL;
    }
    
    if(magic_num != 0x4f434153)
    {
        fprintf(stderr, "Error: %s is not a valid SASM Compiled Object file.\n", name);
        return NULL;
    }
    
    /* Read the location labels */

    LinkerLocationLabel *l_root = linker_location_labels_deserialize(fp, section_id, global_root);

    if(l_root == NULL)
        return NULL;
    
    /* Read the locations to fill */

    LinkerLocationToFill *ltf_root = linker_locations_to_fill_deserialize(fp);

    if(ltf_root == NULL)
        return NULL;
    
    /* fp's location now points to the beginning of compiled code */

    return linker_section_create(name, fp, l_root, ltf_root);
}