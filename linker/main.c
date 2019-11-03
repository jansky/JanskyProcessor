#include "linker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int char_occurence_count(char *str, char c)
{
    if(str == NULL)
        return 0;
    
    int count = 0;
    
    for(int i = 0; i < strlen(str); i++)
    {
        if(str[i] == c)
            count++;
    }

    return count;
}

int main(int argc, char **argv)
{
    if(argc < 4)
    {
        fprintf(stderr, "Usage: %s format file1.o file2.o file3.o ... output-file\n", argv[0]);
        exit(1);
    }

    LinkerSection *s_root = linker_section_create(NULL, NULL, NULL, 0,0, NULL, NULL);
    LinkerLocationLabel *global_root = linker_location_label_create(NULL, 0, -1, 0);

    FILE *output_fp = fopen(argv[argc-1], "w+b");

    if(output_fp == NULL)
    {
        fprintf(stderr, "Error: Unable to open output file %s.\n", argv[argc-1]);
        exit(1);
    }

    /* Load the output format */

    /* First see how many types there are */

    int num_types = char_occurence_count(argv[1], ';') + 1;

    char **types = calloc(num_types,sizeof(char*));

    types[0] = strtok(argv[1], ";");

    if(types[0] == NULL)
    {
        fprintf(stderr, "Error: Unable to read output format from command line argument.\n");
        exit(1);
    }

    for(int types_i = 1; types_i < num_types; types_i++)
    {
        types[types_i] = strtok(NULL, ";");

        if(types[types_i] == NULL)
        {
            fprintf(stderr, "Error: Unable to read output format from command line argument.\n");
            exit(1);
        }
    }

    /* Every other argument but the last contains an input filename */

    for(int i = 2; i < (argc - 1); i++)
    {
        if(linker_read_sections_from_file(argv[i], s_root, global_root) != SASM_ERROR_NOERROR)
        {
            fprintf(stderr, "Error: Section loading failed.\n");
            linker_fatal_error();
            exit(1);
        }
    }

    /* Get the order sections must be written in */

    int num_sections_to_write = 0;

    int *section_order = linker_get_section_write_order(s_root, types, num_types, &num_sections_to_write);

    if(section_order == NULL)
    {
        fprintf(stderr, "Error: Unable to sort sections for writing.\n");
        exit(1);
    }

    if(num_sections_to_write != (linker_section_get_count(s_root)-1))
    {
        fprintf(stderr, "Warning: Number of sections to be written is less than number of sections loaded. Please check your output format.\n");

        /* Get sections that won't be included to help user */

        LinkerSection *s_warn_next = s_root;
        int s_warn_id = 0;

        fprintf(stderr, "Warning: The following sections will not be included: ");

        while(s_warn_next != NULL)
        {
            if(s_warn_next->name != NULL)
            {
                if(!linker_section_id_in_array(section_order, num_sections_to_write, s_warn_id))
                {
                    fprintf(stderr, "%s ", s_warn_next->name);
                }
            }

            s_warn_id++;
            s_warn_next = s_warn_next->next;
        }

        fprintf(stderr, "\n");
    }

    /* Write first and second pass */

    if(linker_write_first_pass(s_root, section_order, num_sections_to_write, output_fp) != SASM_ERROR_NOERROR)
    {
        fprintf(stderr, "Error: First pass failed.\n");
        linker_fatal_error();
        exit(1);
    }

    if(linker_write_second_pass(s_root, output_fp, global_root, section_order, num_sections_to_write) != SASM_ERROR_NOERROR)
    {
        fprintf(stderr, "Error: Second pass failed.\n");
        linker_fatal_error();
        exit(1);
    }

    /* And we're done */

    fclose(output_fp);

    return 0;
}