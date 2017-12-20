#include "linker.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if(argc < 3)
    {
        fprintf(stderr, "Usage: %s file1.o file2.o file3.o ... output-file\n", argv[0]);
        exit(1);
    }

    LinkerSection *s_root = linker_section_create(NULL, NULL, NULL, NULL);
    LinkerLocationLabel *global_root = linker_location_label_create(NULL, 0, -1);

    FILE *output_fp = fopen(argv[argc-1], "w+b");

    if(output_fp == NULL)
    {
        fprintf(stderr, "Error: Unable to open output file %s.\n", argv[argc-1]);
        exit(1);
    }

    /* Every argument but the last contains an input filename */

    int current_section_id = 1;

    for(int i = 1; i < (argc - 1); i++)
    {
        LinkerSection *s_new = linker_section_create_from_file(argv[i], current_section_id, global_root);

        if(s_new == NULL)
        {
            linker_fatal_error();
            exit(1);
        }

        if(linker_section_add(s_root, s_new) != SASM_ERROR_NOERROR)
        {
            linker_fatal_error();
            exit(1);
        }

        current_section_id++;
    }

    /* Write first and second pass */

    if(linker_write_first_pass(s_root, output_fp) != SASM_ERROR_NOERROR)
    {
        linker_fatal_error();
        exit(1);
    }

    if(linker_write_second_pass(s_root, output_fp, global_root) != SASM_ERROR_NOERROR)
    {
        linker_fatal_error();
        exit(1);
    }

    /* And we're done */

    fclose(output_fp);

    return 0;
}