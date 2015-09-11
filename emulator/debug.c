#include "debug.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

beakpoint *debug_breakpoint_create(uint32_t offset, uint32_t address)
{
	breakpoint *b = malloc(sizeof(breakpoint));

	if(b == NULL)
		return NULL;

	b->offset = offset;
	b->address = address;
	b->next = NULL;

	return b;

}

uint32_t debug_breakpoint_get_max(breakpoint *root)
{
	uint32_t i = 0;

	breakpoint *old = root;

	while(old != NULL)
	{
		old = old->next;

		i++;
	}

	i--;

	return i;
}

breakpoint *debug_breakpoint_get(breakpoint *root, uint32_t id)
{
	uint32_t i = 0;

	breakpoint *old = root;

	while(old != NULL)
	{
		if(id == i)
			return old;

		old = old->next;

		i++;
	}

	return NULL;
}

bool debug_breakpoint_add(breakpoint *root, breakpoint *new)
{
	uint32_t max = debug_breakpoint_get_max(root);

	breakpoint *end = debug_breakpoint_get(root, max);

	if(end == NULL)
		return false;

	end->next = new;

	return true;
}

bool debug_is_on_breakpoint(breakpoint *root, CPU *cpu)
{
	uint32_t i = 0;

	for(; i < debug_breakpoint_get_max(root); i++)
	{
		breakpoint *current = debug_breakpoint_get(i);

		if(current != NULL)
		{
			if(cpu->or1 == current->offset && cpu->ip == current->address)
				return true;
		}
	}

	return false;
}

bool debug_do_interface(breakpoint *root, CPU *cpu, RAMUNIT *ram)
{
	char input[201];

	char *to_parse;



	while(1)
	{
		printf("(dbg) ");
		fgets(input, 200, stdin);

		to_parse = malloc(201);

		if(to_parse == NULL)
			return NULL;

		strncpy(to_parse, input, 200);

		//remove trailing newline
		strtok(to_parse, "\n");

		//first test for commands with no arguments
		if(strncmp(to_parse, "help", 200) == 0)
		{
			// print help message
			;
		}
		else if(strncmp(to_parse, "continue", 200) == 0)
		{
			//exit out of loop
			break;
		}
		else if(strncmp(to_parse, "regdump", 200) == 0)
		{
			//we need to print a register dump
			;
		}
		else
		{
			//now test for commands with arguments (hooray for strtok)

			char *cmdname = strtok(to_parse, " ");

			if(cmdname)
			{
				if(strncmp(cmdname, "break", 5) == 0)
				{
					//we need to set a breakpoint
					;
				}
				else if(strncmp(cmdname, "print", 5) == 0)
				{
					//we need to print a value
					;
				}
				else if(strncmp(cmdname, "set", 5) == 0)
				{
					//we need to set a value
					;
				}
				else
				{
					fprintf(stderr, "Debug Error: Command \'%s\' not recognized.", cmdname);
				}
			}
			else
			{
				fprintf(stderr, "Debug Error: Command \'%s\' not recognized.", to_parse);
			}
		}
				

		

		
	}

	return true;
}
	
