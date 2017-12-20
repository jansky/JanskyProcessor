#include <linker.h>

#include <stdio.h>
#include <stdlib.h>

void linker_fatal_error()
{
    fprintf(stderr, "Error: A fatal error was encountered. Linking stopped.\n");
}

int linker_read_code_from_file(FILE *input, FILE *output)
{
    if(input == NULL || output == NULL)
        return SASM_ERROR_IOERROR;
    
    char byte = '\0';

    while(fread(&byte, sizeof(char), 1, input) == 1)
    {
        if(fwrite(&byte, sizeof(char), 1, output) != 1)
            return SASM_ERROR_IOERROR;
    }

    if(ferror(input) != 0)
        return SASM_ERROR_IOERROR;
    
    fflush(output);
    
    return SASM_ERROR_NOERROR;
}

int linker_write_first_pass(LinkerSection *s_root, FILE *output_fp)
{
    if(s_root == NULL)
        return SASM_ERROR_LABELERROR;
    
    if(output_fp == NULL)
        return SASM_ERROR_IOERROR;
    
    LinkerSection *next = s_root;

    while(next != NULL)
    {
        if(next->name != NULL && next->fp != NULL)
        {
            /* Get the offset */

            next->location_offset = (uint32_t)ftell(output_fp);

            /* Get all of the compiled code for this section */

            if(linker_read_code_from_file(next->fp, output_fp) != SASM_ERROR_NOERROR)
                return SASM_ERROR_IOERROR;           

        }

        next = next->next;
    }

    return SASM_ERROR_NOERROR;
}

int linker_write_second_pass(LinkerSection *s_root, FILE *output_fp, LinkerLocationLabel *global_root)
{
    if(s_root == NULL || global_root == NULL)
        return SASM_ERROR_LABELERROR;
    
    if(output_fp == NULL)
        return SASM_ERROR_IOERROR;
    
    /* Create an array of section offsets to speed things up */

    uint32_t max_section = 0;

    LinkerSection *s_count_next = s_root;

    while(s_count_next != NULL)
    {
        max_section++;
        s_count_next = s_count_next->next;
    }

    uint32_t *offsets = calloc(max_section, sizeof(uint32_t));

    if(offsets == NULL)
        return SASM_ERROR_UNKNOWNERROR;
    
    s_count_next = s_root;
    int offset_i = 0;

    while(s_count_next != NULL)
    {
        offsets[offset_i] = s_count_next->location_offset;

        s_count_next = s_count_next->next;

        offset_i++;
    }

    /* Go section by section and fill in all of the locations-to-fill */

    s_count_next = s_root;
    int current_section_id = 0;

    while(s_count_next != NULL)
    {
        if(s_count_next->name != NULL && s_count_next->fp != NULL && s_count_next->l_root != NULL && s_count_next->ltf_root != NULL)
        {
            LinkerLocationToFill *ltf_next = s_count_next->ltf_root;

            while(ltf_next != NULL)
            {
                if(ltf_next->name != NULL)
                {
                    /* First look for the name in local labels */

                    LinkerLocationLabel *found_label = linker_location_label_get_by_name(s_count_next->l_root, ltf_next->name);

                    if(found_label == NULL)
                    {
                        /* Then try the globals */

                        found_label = linker_location_label_get_by_name(global_root, ltf_next->name);
                    }

                    if(found_label == NULL)
                    {
                        /* The label does not exist. */
                        fprintf(stderr, "Error: In section %s, label \'%s\' does not exist in current scope.\n", s_count_next->name, ltf_next->name);
                        return SASM_ERROR_LABELINVALID;
                    }

                    uint32_t absolute_label_location = offsets[found_label->section_id] + found_label->location;

                    long absolute_fill_location = (long)(offsets[current_section_id] + ltf_next->location);

                    /* Fill It In */

                    if(fseek(output_fp, absolute_fill_location, SEEK_SET) != 0)
                        return SASM_ERROR_IOERROR;

                    if(fwrite(&absolute_label_location, sizeof(uint32_t), 1, output_fp) != 1)
                        return SASM_ERROR_IOERROR;              
                }

                ltf_next = ltf_next->next;
            }
        }

        current_section_id++;

        s_count_next = s_count_next->next;
    }

    fflush(output_fp);

    return SASM_ERROR_NOERROR;
}