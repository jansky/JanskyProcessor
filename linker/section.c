#include <linker.h>

#include <stdlib.h>
#include <string.h>

LinkerSection *linker_section_create(char *name, char *type, FILE *fp, long code_begin_location, uint32_t code_section_length, LinkerLocationLabel *l_root, LinkerLocationToFill *ltf_root)
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

    if(type != NULL)
    {
        section->type = strdup(type);

        if(section->type == NULL)
            return NULL;
    }
    else
    {
        section->type == NULL;
    }

    section->fp = fp;
    section->code_begin_location = code_begin_location;
    section->code_section_length = code_section_length;
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

int linker_section_get_count(LinkerSection *s_root)
{
    if(s_root == NULL)
        return 0;
    
    int count = 0;
    
    LinkerSection *s_next = s_root;

    while(s_next != NULL)
    {
        count++;

        s_next = s_next->next;
    }

    return count;
}

LinkerSection *linker_section_get_by_id(LinkerSection *s_root, int id)
{
    if(s_root == NULL || id < 0)
        return NULL;
    
    LinkerSection *next = s_root;

    int count = 0;

    while(next != NULL)
    {
        if(count == id)
            return next;
        
        next = next->next;
        count++;
    }

    return NULL;
}

bool linker_section_id_in_array(int *section_order, int num_sections_to_write, int id)
{
    if(section_order == NULL)
        return false;
    
    for(int i = 0; i < num_sections_to_write; i++)
    {
        if(section_order[i] == id)
            return true;
    }

    return false;
}

char *linker_get_filename_from_fullname(char *fullname)
{
    if(fullname == NULL)
        return NULL;
    
    /* Get position of last $ */

    uintptr_t fullname_start = (uintptr_t)fullname;

    char *last_dollar_sign = strrchr(fullname, '$');

    if(last_dollar_sign == NULL)
        return fullname;
    
    size_t bytes_to_copy = (size_t)((uintptr_t)last_dollar_sign - fullname_start) - 1;

    char *result = calloc(bytes_to_copy+1, sizeof(char));

    if(result == NULL)
        return NULL;
    
    return strncpy(result, fullname, bytes_to_copy);
}

int *linker_section_get_file_global_sections(LinkerSection *s_root, char *fullname, int *num_file_global_sections)
{
    if(s_root == NULL || fullname == NULL || num_file_global_sections == NULL)
        return NULL;
    
    int *file_global_sections = calloc(1, sizeof(int));

    int num_allocated = 1;
    int num_written = 0;
    int section_id = 0;

    LinkerSection *s_next = s_root;

    char *filename = linker_get_filename_from_fullname(fullname);

    if(filename == NULL)
        return NULL;

    while(s_next != NULL)
    {
        if(num_allocated == num_written)
        {
            file_global_sections = realloc(file_global_sections, (num_allocated+1)*sizeof(int));

            if(file_global_sections == NULL)
                return NULL;
            
            num_allocated++;
        }

        if(s_next->name != NULL)
        {
            char *s_fullname = strdup(s_next->name);
            char *s_filename = linker_get_filename_from_fullname(s_fullname);

            if(s_filename == NULL)
                return NULL;
            
            if(strcmp(filename,s_filename) == 0)
            {
                file_global_sections[num_written] = section_id;
                num_written++;
            }

            free(s_fullname);
            free(s_filename);
        }

        section_id++;
        s_next = s_next->next;
    }

    *num_file_global_sections = num_written;

    return file_global_sections;
}

int linker_read_sections_from_file(char *filename, LinkerSection *s_root, LinkerLocationLabel *global_root)
{
    if(filename == NULL)
        return SASM_ERROR_STRINGERROR;
    
    if(s_root == NULL || global_root == NULL)
        return SASM_ERROR_LABELERROR;
    
    FILE *fp = fopen(filename, "r+b");

    if(fp == NULL)
    {
        fprintf(stderr, "Error: %s is not a valid SASM Compiled Object file.\n", filename);
        return SASM_ERROR_IOERROR;
    }
    
    /* Verify magic number */
    /* SACO = 4F 43 41 53 (little endian) */

    uint32_t magic_num = 0;

    if(fread(&magic_num, sizeof(uint32_t), 1, fp) != 1)
    {
        fprintf(stderr, "Error: %s is not a valid SASM Compiled Object file.\n", filename);
        return SASM_ERROR_IOERROR;
    }
    
    if(magic_num != 0x4f434153)
    {
        fprintf(stderr, "Error: %s is not a valid SASM Compiled Object file.\n", filename);
        return SASM_ERROR_IOERROR;
    }

    int current_section_id = -1;

    LinkerSection *s_next = s_root;

    while(s_next != NULL)
    {
        current_section_id++;

        s_next = s_next->next;
    }

    current_section_id++;

    uint32_t number_of_sections = 0;

    if(fread(&number_of_sections, sizeof(uint32_t), 1, fp) != 1)
        return SASM_ERROR_IOERROR;
    
    for(uint32_t s_i = 0; s_i < number_of_sections; s_i++)
    {
        /* Get the Name */
        /* This will be used unaltered for the section type.
           The section's actual, unique name is filename$type */

        uint8_t name_length = 0;
        char *name = NULL;

        if(fread(&name_length, sizeof(uint8_t), 1, fp) != 1)
            return SASM_ERROR_IOERROR;
        
        name = calloc(name_length+1,sizeof(char));

        if(name == NULL)
            return SASM_ERROR_STRINGERROR;
        
        if(fread(name, sizeof(char), name_length, fp) != name_length)
            return SASM_ERROR_IOERROR;
        
        char *fullname = calloc(strlen(filename) + name_length + 2, sizeof(char)); // We need space for a $ and null terminator

        if(fullname == NULL)
            return SASM_ERROR_STRINGERROR;

        strcpy(fullname, filename);

        strcat(fullname, "$");

        strcat(fullname, name);

        /* Read the location labels */

        LinkerLocationLabel *l_root = linker_location_labels_deserialize(fp, current_section_id, global_root);

        if(l_root == NULL)
            return SASM_ERROR_IOERROR;
        
        /* Read the locations to fill */

        LinkerLocationToFill *ltf_root = linker_locations_to_fill_deserialize(fp);

        if(ltf_root == NULL)
            return SASM_ERROR_IOERROR;

        /* Get the length and location of the code section */

        uint32_t code_section_length = 0;

        if(fread(&code_section_length, sizeof(uint32_t), 1, fp) != 1)
            return SASM_ERROR_IOERROR;
        
        long code_section_location = ftell(fp);

        /* Fast-foward to beyond the code section, so we can read the next section */

        if(fseek(fp, code_section_location+code_section_length, SEEK_SET) != 0)
            return SASM_ERROR_IOERROR;
        
        LinkerSection *s_new = linker_section_create(fullname, name, fp, code_section_location, code_section_length, l_root, ltf_root);

        if(s_new == NULL)
            return SASM_ERROR_UNKNOWNERROR;

        if(linker_section_add(s_root, s_new) != SASM_ERROR_NOERROR)
            return SASM_ERROR_UNKNOWNERROR;

        current_section_id++;
    }

    return SASM_ERROR_NOERROR;
}

int *linker_get_section_write_order(LinkerSection *s_root, char **types, int num_types, int *num_sections_to_write)
{
    if(s_root == NULL || types == NULL || num_sections_to_write == NULL)
        return NULL;
    
    int *section_order = calloc(1, sizeof(int));

    if(section_order == NULL)
        return NULL;
    
    int num_written = 0;
    int num_allocated = 1;
    int section_id = 0;
    
    for(int t_i = 0; t_i < num_types; t_i++)
    {
        if(num_written == num_allocated)
        {
            section_order = realloc(section_order,(num_allocated+1)*sizeof(int));

            if(section_order == NULL)
                return NULL;
            
            num_allocated++;
        }

        LinkerSection *s_next = s_root;

        while(s_next != NULL)
        {
            if(s_next->name != NULL && s_next->type != NULL)
            {
                if(strcmp(s_next->type, types[t_i]) == 0)
                {
                    section_order[num_written] = section_id;

                    num_written++;
                }
            }

            section_id++;

            s_next = s_next->next;
        }

        section_id = 0;
    }

    *num_sections_to_write = num_written;

    return section_order;
}