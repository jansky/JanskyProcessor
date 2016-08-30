#include "sasm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if(argc < 3)
    {
        fprintf(stderr, "Usage: %s file.s output\n", argv[0]);
        exit(SASM_ERROR_USAGEINVALID);
    }
    
    char *input_file = argv[1];
    char *output_file = argv[2];
    
    FILE *input_fp;
    FILE *output_fp;
    
    SASMLocationLabel *ll_root;
    SASMLocationToFill *ltf_root;
    
    char *line;
    int line_no = 1;
    
    // Initialize file pointers
    input_fp = fopen(input_file, "r+b");
    output_fp = fopen(output_file, "w+b");
    
    if(input_fp == NULL)
    {
        fprintf(stderr, "ERROR: Unable to open file %s\n", input_file);
        exit(SASM_ERROR_IOERROR);
    }
    
    if(output_fp == NULL)
    {
        fprintf(stderr, "ERROR: Unable to open file %s\n", input_file);
        exit(SASM_ERROR_IOERROR);
    }
        
    // Initialize labels
    ll_root = sasm_location_label_create(NULL, 0);
    ltf_root = sasm_location_to_fill_create(NULL, 0);
    
    if(ll_root == NULL || ltf_root == NULL)
    {
        fprintf(stderr, "ERROR: Unable to initialize label lists.\n");
        exit(SASM_ERROR_LABELERROR);
    }
        
        
    // Begin reading input assembly file line by line
    line = malloc(sizeof(char)*(SASM_MAX_LINE_LENGTH + 1));
    
    if(line == NULL)
    {
        fprintf(stderr, "ERROR: Unable to allocate memory for reading assembly file.\n");
        exit(SASM_ERROR_STRINGERROR);
    }
    
    while(fgets(line, SASM_MAX_LINE_LENGTH, input_fp) != NULL)
    {
    	strtok(line, "\n"); // Remove trailing newline
    	
        int result = sasm_assemble_line(line, output_fp, ll_root, ltf_root);
        
        if(result != SASM_ERROR_NOERROR)
        {
            fprintf(stderr, "[Error at %s:%d] %d\n", input_file, line_no, result);
            exit(result);
        }
        
        line_no++;
        
        free(line);
        
        line = malloc(sizeof(char)*(SASM_MAX_LINE_LENGTH + 1));
    
        if(line == NULL)
        {
            fprintf(stderr, "ERROR: Unable to allocate memory for reading assembly file.\n");
            exit(SASM_ERROR_STRINGERROR);
        }
    }
    
    // Fill in labels
    int label_result = sasm_fill_in_labels(output_fp, ll_root, ltf_root);
    
    if(label_result != SASM_ERROR_NOERROR)
    {
        fprintf(stderr, "ERROR: Code %d when filling in labels.\n", label_result);
        exit(label_result);
    }
    
    return 0;
}
    
    
