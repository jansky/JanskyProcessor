#include <linker.h>

#include <stdio.h>
#include <stdlib.h>

void linker_fatal_error()
{
    fprintf(stderr, "Error: A fatal error was encountered. Linking stopped.\n");
}

int linker_read_code_from_file(FILE *input, FILE *output, long code_location, uint32_t code_length)
{
    if(input == NULL || output == NULL)
        return SASM_ERROR_IOERROR;
    
    char byte = '\0';

    if(fseek(input, code_location, SEEK_SET) != 0)
        return SASM_ERROR_IOERROR;

    for(uint32_t i = 0; i < code_length; i++)
    {
        if(fread(&byte, sizeof(char), 1, input) == 1)
        {
            if(fwrite(&byte, sizeof(char), 1, output) != 1)
                return SASM_ERROR_IOERROR;
        }
        else
        {
            return SASM_ERROR_IOERROR;
        }
    }
    
    fflush(output);
    
    return SASM_ERROR_NOERROR;
}

int linker_write_first_pass(LinkerSection *s_root, int *section_order, int num_sections_to_write, FILE *output_fp)
{
    if(s_root == NULL || section_order == NULL)
    {
        fprintf(stderr, "Error: Code 1\n");
        return SASM_ERROR_LABELERROR;
    }
        
    
    if(output_fp == NULL)
    {
        fprintf(stderr, "Error: Code 2\n");
        return SASM_ERROR_LABELERROR;
    }
    
    for(int i = 0; i < num_sections_to_write; i++)
    {
        int section_id = section_order[i];

        LinkerSection *next = linker_section_get_by_id(s_root, section_id);

        if(next == NULL)
        {
            fprintf(stderr, "Error: Code 3\n");
            return SASM_ERROR_LABELERROR;
        }
        
        if(next->name != NULL && next->fp != NULL)
        {
            /* Get the offset */

            next->location_offset = (uint32_t)ftell(output_fp);

            /* Get all of the compiled code for this section */

            if(linker_read_code_from_file(next->fp, output_fp, next->code_begin_location, next->code_section_length) != SASM_ERROR_NOERROR)
            {
                fprintf(stderr, "Error: Code 4\n");
                return SASM_ERROR_LABELERROR;
            }           

        }
        else
        {
            
            fprintf(stderr, "Error: Code 5\n");
            return SASM_ERROR_LABELERROR;
    
        }
    }   

    return SASM_ERROR_NOERROR;
}

int linker_write_second_pass(LinkerSection *s_root, FILE *output_fp, LinkerLocationLabel *global_root, int *section_order, int num_sections_to_write)
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
            /* Get file global sections */

            int num_file_global_sections = 0;
            int *file_global_sections = linker_section_get_file_global_sections(s_root, s_count_next->name, &num_file_global_sections);

            if(file_global_sections == NULL)
            {
                fprintf(stderr, "Error: Unable to load file global sections.\n");
                return SASM_ERROR_UNKNOWNERROR;
            }

            LinkerLocationToFill *ltf_next = s_count_next->ltf_root;

            while(ltf_next != NULL)
            {
                if(ltf_next->name != NULL)
                {
                    /* First look for the name in local labels */

                    LinkerLocationLabel *found_label = linker_location_label_get_by_name(s_count_next->l_root, ltf_next->name);

                    /* Then try for the name in other sections in same file (file globals) */

                    if(found_label == NULL)
                    {
                        for(int fg_i = 0; fg_i < num_file_global_sections; fg_i++)
                        {
                            LinkerSection *fg_s = linker_section_get_by_id(s_root, file_global_sections[fg_i]);

                            if(fg_s == NULL)
                                return SASM_ERROR_UNKNOWNERROR;
                            
                            found_label = linker_location_label_get_by_name(fg_s->l_root, ltf_next->name);

                            if(found_label != NULL)
                            {
                                /* Ensure label is in a section that is going to be written, and that it is a file global */

                                if(found_label->global == 2 && linker_section_id_in_array(section_order, num_sections_to_write, found_label->section_id))
                                    break;
                                else
                                    found_label = NULL;
                            }
                            

                        }
                    }

                    if(found_label == NULL)
                    {
                        /* Then try the globals */

                        found_label = linker_location_label_get_by_name(global_root, ltf_next->name);

                        /* But if the label is in a section that isn't going to be written, fail */

                        if(found_label != NULL)
                        {

                            if(!linker_section_id_in_array(section_order, num_sections_to_write, found_label->section_id))
                            {
                                /* The label does not exist. */
                                fprintf(stderr, "Error: In section %s, label \'%s\' does not exist in current scope.\n", s_count_next->name, ltf_next->name);
                                return SASM_ERROR_LABELINVALID;
                            }
                            
                        }
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

            free(file_global_sections);

        }

        
        current_section_id++;

        s_count_next = s_count_next->next;
    }

    fflush(output_fp);

    return SASM_ERROR_NOERROR;
}