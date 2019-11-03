#include "debug.h"
#include "disassembler.h"
#include "gc.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

breakpoint *debug_breakpoint_create(uint32_t offset, uint32_t address, bool enabled)
{
	breakpoint *b = malloc(sizeof(breakpoint));

	if(b == NULL)
		return NULL;

	b->offset = offset;
	b->address = address;
	b->enabled = enabled;
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

	for(; i <= debug_breakpoint_get_max(root); i++)
	{
		breakpoint *current = debug_breakpoint_get(root, i);

		if(current != NULL)
		{
			//printf("\nbreakpoint - 0x%x+0x%x", current->offset, current->address);
			if(current->enabled && cpu->or1 == current->offset && cpu->ip == current->address)
				return true;
		}
	}

	return false;
}

breakpoint *debug_do_interface(breakpoint *b_root, CPU *cpu, RAMUNIT *ram, FILE *memory_fp)
{
	char input[201];

	int debug_signal = DEBUG_SIGNAL_NOSIGNAL;

	breakpoint *root = b_root;

	if(cpu == NULL || ram == NULL)
	{
		fprintf(stderr, "Debug Error: Unable to access emulator CPU or RAM.\n");
		return root;
	}

	if(ram->data == NULL)
	{
		fprintf(stderr, "Debug Error: Unable to read data from emulator RAM.\n");
		return root;
	}

	if(memory_fp == NULL)
	{

		fprintf(stderr, "Debug Error: Unable to open RAMUNIT for reading.\n");
		return root;
		
	}

	printf("\nWelcome to the debugger.\n\n");

	printf("or1 - 0x%x\tip - 0x%x\n\n", cpu->or1, cpu->ip);

	while(debug_signal == DEBUG_SIGNAL_NOSIGNAL)
	{
		printf("(dbg) ");
		fgets(input, 200, stdin);

		char *to_parse = strdup(input);

		if(to_parse == NULL)
		{
			fprintf(stderr, "Debug Error: Unable to get user input.\n");
			break;
		}

		debug_signal = debug_do_line(&root, cpu, ram, to_parse, memory_fp);

	}


    return root;
	
}

int debug_do_line(breakpoint **b_root, CPU *cpu, RAMUNIT *ram, char *to_parse, FILE *memory_fp)
{
	breakpoint *root = *b_root;

	if(memory_fp == NULL)
	{

		return DEBUG_SIGNAL_ERROR;
		
	}

	if(to_parse[0] != '\n')
	{

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
			printf("Debugger relinquishing control.\n\n");
			return DEBUG_SIGNAL_CONTINUE;
		}
		else if(strncmp(to_parse, "regdump", 200) == 0)
		{
			//we need to print a register dump
			debug_do_regdump(cpu);
		}
		else if(strncmp(to_parse, "location", 200) == 0)
		{
			//we need to print the current location
			printf("or1 - 0x%x\tip - 0x%x\n", cpu->or1, cpu->ip);
		}
		else if(strncmp(to_parse, "quit", 200) == 0)
		{
			//we need to quit
			printf("\n\nEmulator exiting.\n");
			return DEBUG_SIGNAL_EXIT;
		}
		else if(strncmp(to_parse, "stacklist", 200) == 0)
		{
			//we need to print the contents of the stack

			if(cpu->sp < cpu->sb)
			{
				fprintf(stderr, "Debug Error: The stack is in an overflow state.\n");
				return DEBUG_SIGNAL_ERROR;
			}
				
			uint32_t sp_new;

			sp_new = cpu->sp;

			while(sp_new + 4 < ram->bytesize)
			{
				emu_error = 0;
				
				DWORD value = get_dword_at_ram_address(ram, sp_new + 1);

				if(emu_error != 0)
				{
					fprintf(stderr, "Debug Error: Memory error.\n");
					return DEBUG_SIGNAL_ERROR;
				}
			
				printf("0x%x (%d)\n", value, value);

				sp_new += 4;
			}
			
		}					
		else if(strncmp(to_parse, "breaklist", 200) == 0)
		{
			//we need to print a list of breakpoints
			if(root == NULL)
			{
				printf("No breakpoints have been set.\n");
				return DEBUG_SIGNAL_NOSIGNAL;
			}				

			uint32_t i = 0;

			printf("ID\tStatus\tLocation\n\n");

			for(; i <= debug_breakpoint_get_max(root); i++)
			{
				breakpoint *current = debug_breakpoint_get(root, i);

				if(current != NULL)
				{
					//printf("\nbreakpoint - 0x%x+0x%x", current->offset, current->address);
					printf("%d\t%d\t0x%x+0x%x\n", i, current->enabled, current->offset, current->address);
					
						
				}
			}
			
		}
		else if(strncmp(to_parse, "interop", 200) == 0)
		{
			if(cpu->iinfo == NULL)
			{
				fprintf(stderr, "Debug Error: Interop is broken.\n");
				return DEBUG_SIGNAL_ERROR;
			}

			
			printf("Interop Status\n\n");

			printf("Disk\n");
			printf("----\n");
			
			//disk status
			if(cpu->iinfo->disk_enabled)
			{
				printf("Enabled: yes\nRoot Directory: %s\n", cpu->iinfo->root);
			}
			else
				printf("Enabled: no\n");

			printf("\n");
			
				
		}
		else if(strncmp(to_parse, "pop", 200) == 0)
		{
			emu_error = 0;
			
			uint32_t n;

			n = stack_pop(cpu, ram);

			if(emu_error != 0)
			{
				fprintf(stderr, "Debug Error: Stack underflow (no value popped, sp remained constant).\n");
				return DEBUG_SIGNAL_ERROR;
			}			
			
			printf("stack pop - 0x%x (%d)\n", n, n);			
		}
		else if(strncmp(to_parse, "peek", 200) == 0)
		{
			emu_error = 0;
			
			uint32_t n;

			n = stack_pop(cpu, ram);

			if(emu_error != 0)
				fprintf(stderr, "Debug Error: Stack underflow (no value popped, sp remained constant).\n");
			else
			{
				printf("stack peek - 0x%x (%d)\n", n, n);

				if(!stack_push(cpu, ram, n))
					fprintf(stderr, "Debug Error: Stack overflow. The stack is likely broken (you should quit).\n");
			}


			

			
		}
		else if(strncmp(to_parse, "dsmbl", 200) == 0)
		{
			emu_error = 0;

			char *disassembled_str = NULL;
			size_t disassembled_str_size;

			FILE *dsmbl_fp = open_memstream(&disassembled_str, &disassembled_str_size);

			if(dsmbl_fp == NULL)
				fprintf(stderr, "Debug Error: Unable to read code from memory.\n");
			else
			{
				printf("0x%x+0x%x: ", cpu->or1, cpu->ip);

				if(fseek(memory_fp, cpu->or1 + cpu->ip, SEEK_SET) != 0)
				{
					fprintf(stderr, "Debug Error: Unable to read code from memory.\n");
				}
				else
				{
					if(dr_disassemble_instruction(memory_fp, dsmbl_fp) == 0)
					{
						fflush(dsmbl_fp);

						if(disassembled_str != NULL)
						{
							printf("%s\n", disassembled_str);
						}
						else
						{
							printf("<Invalid Code>\n");
						}
					}
					else
					{
						printf("<Invalid Code>\n");
					}
				
				}					

				fclose(dsmbl_fp);
				free(disassembled_str);
			}

		}
		else if(strncmp(to_parse, "dsmbln", 200) == 0)
		{
			emu_error = 0;

			char *disassembled_str = NULL;
			size_t disassembled_str_size;

			FILE *dsmbl_fp = open_memstream(&disassembled_str, &disassembled_str_size);

			if(dsmbl_fp == NULL)
				fprintf(stderr, "Debug Error: Unable to read code from memory.\n");
			else
			{
				printf("0x%x+0x%x: ", cpu->or1, (int)ftell(memory_fp)-(cpu->or1));

				
				if(dr_disassemble_instruction(memory_fp, dsmbl_fp) == 0)
				{
					fflush(dsmbl_fp);

					if(disassembled_str != NULL)
					{
						printf("%s\n", disassembled_str);
					}
					else
					{
						printf("<Invalid Code>\n");
					}
				}
				else
				{
					printf("<Invalid Code>\n");
				}
							

				fclose(dsmbl_fp);
				free(disassembled_str);
			}

		}
		else
		{
			//now test for commands with arguments (hooray for strtok)

			char *cmdname = strtok(to_parse, " ");

			if(cmdname)
			{
				if(strcmp(cmdname, "break") == 0)
				{
					char *offset;
					char *address;

					uint32_t o;
					uint32_t a;

					offset = strtok(NULL, " ");

					if(offset == NULL)
						fprintf(stderr, "Debug Error: Proper usage: break [offset] [address].\n");
					else
					{
						address = strtok(NULL, " ");

						if(address == NULL)
							fprintf(stderr, "Debug Error: Proper usage: break [offset] [address].\n");
						else
						{
							if(sscanf(offset, "%x", &o) != 1)
								fprintf(stderr, "Debug Error: Proper usage: break [offset] [address].\n");
							else
							{
								if(sscanf(address, "%x", &a) != 1)
									fprintf(stderr, "Debug Error: Proper usage: break [offset] [address].\n");
								else
								{
									if(o+a >= ram->bytesize)
										fprintf(stderr, "Debug Error: Memory address out of range.\n");
									else
									{
										breakpoint *b_new;

										b_new = debug_breakpoint_create(o, a, true);

										if(b_new == NULL)
											fprintf(stderr, "Debug Error: Could not create breakpoint.\n");
										else
										{
											if(root == NULL)
											{
												root = b_new;
												printf("Added breakpoint at 0x%x+0x%x\n", o, a);
											}
											else
											{
												if(!debug_breakpoint_add(root, b_new))
													fprintf(stderr, "Debug Error: Could not add new breakpoint to list.\n");
												else
													printf("Added breakpoint at 0x%x+0x%x\n", o, a);
											}
										}
									}
								
								
							}
						}
					}
				}
			
				
				}
				else if(strcmp(cmdname, "dsmblo") == 0)
				{
					char *num;

					uint32_t n;

					num = strtok(NULL, " ");

					if(num == NULL)
					{
						fprintf(stderr, "Debug Error: Proper usage: dsmblo [offset].\n");
					}
					else
					{
						if(sscanf(num, "%x", &n) != 1)
							fprintf(stderr, "Debug Error: Proper usage: dsmblo [offset].\n");
						else
						{
							emu_error = 0;

							char *disassembled_str = NULL;
							size_t disassembled_str_size;

							FILE *dsmbl_fp = open_memstream(&disassembled_str, &disassembled_str_size);

							if(dsmbl_fp == NULL)
								fprintf(stderr, "Debug Error: Unable to read code from memory.\n");
							else
							{
								printf("0x%x+0x%x: ", cpu->or1, n);

								if(fseek(memory_fp, (cpu->or1)+n, SEEK_SET) != 0)
								{
									fprintf(stderr, "Debug Error: Unable to read code from memory.\n");
								}
								else
								{
									if(dr_disassemble_instruction(memory_fp, dsmbl_fp) == 0)
									{
										fflush(dsmbl_fp);

										if(disassembled_str != NULL)
										{
											printf("%s\n", disassembled_str);
										}
										else
										{
											printf("<Invalid Code>\n");
										}
									}
									else
									{
										printf("<Invalid Code>\n");
									}
								
								}					

								fclose(dsmbl_fp);
								free(disassembled_str);
							}
						}
					}
				}
				else if(strcmp(cmdname, "dsmbla") == 0)
				{
					char *num;

					uint32_t n;

					num = strtok(NULL, " ");

					if(num == NULL)
					{
						fprintf(stderr, "Debug Error: Proper usage: dsmbla [address].\n");
					}
					else
					{
						if(sscanf(num, "%x", &n) != 1)
							fprintf(stderr, "Debug Error: Proper usage: dsmbla [address].\n");
						else
						{
							emu_error = 0;

							char *disassembled_str = NULL;
							size_t disassembled_str_size;

							FILE *dsmbl_fp = open_memstream(&disassembled_str, &disassembled_str_size);

							if(dsmbl_fp == NULL)
								fprintf(stderr, "Debug Error: Unable to read code from memory.\n");
							else
							{
								printf("0x%x: ", n);

								if(fseek(memory_fp, n, SEEK_SET) != 0)
								{
									fprintf(stderr, "Debug Error: Unable to read code from memory.\n");
								}
								else
								{
									if(dr_disassemble_instruction(memory_fp, dsmbl_fp) == 0)
									{
										fflush(dsmbl_fp);

										if(disassembled_str != NULL)
										{
											printf("%s\n", disassembled_str);
										}
										else
										{
											printf("<Invalid Code>\n");
										}
									}
									else
									{
										printf("<Invalid Code>\n");
									}
								
								}					

								fclose(dsmbl_fp);
								free(disassembled_str);
							}
						}
					}
				}
				else if(strcmp(cmdname, "push") == 0)
				{
					char *num;

					uint32_t n;

					num = strtok(NULL, " ");

					if(num == NULL)
					{
						fprintf(stderr, "Debug Error: Proper usage: push [hexadecimal number].\n");
					}
					else
					{
						if(sscanf(num, "%x", &n) != 1)
							fprintf(stderr, "Debug Error: Proper usage: push [hexadecimal number].\n");
						else
						{
							if(!stack_push(cpu, ram, n))
								fprintf(stderr, "Debug Error: Stack overflow (no value pushed, sp remained constant).\n");
							else
								printf("stack push - 0x%x (%d)\n", n, n);
						}
					}
				}
				else if(strcmp(cmdname, "h2d") == 0)
				{
					char *num;

					uint32_t n;

					num = strtok(NULL, " ");

					if(num == NULL)
					{
						fprintf(stderr, "Debug Error: Proper usage: h2d [hexadecimal number].\n");
					}
					else
					{
						if(sscanf(num, "%x", &n) != 1)
							fprintf(stderr, "Debug Error: Proper usage: h2d [hexadecimal number].\n");
						else
							printf("0x%x = %d in decimal.\n", n, n);
					}
				}
				else if(strcmp(cmdname, "d2h") == 0)
				{
					char *num;

					uint32_t n;

					num = strtok(NULL, " ");

					if(num == NULL)
					{
						fprintf(stderr, "Debug Error: Proper usage: d2h [decimal number].\n");
					}
					else
					{
						if(sscanf(num, "%d", &n) != 1)
							fprintf(stderr, "Debug Error: Proper usage: h2d [hexadecimal number].\n");
						else
							printf("%d = 0x%x in hexadecimal.\n", n, n);
					}
				}
				else if(strcmp(cmdname, "breakstatus") == 0)
				{
					char *id;
					char *status;

					uint32_t i;
					uint32_t s;

					id = strtok(NULL, " ");

					if(id == NULL)
						fprintf(stderr, "Debug Error: Proper usage: breakstatus [id] [status].\n");
					else
					{
						status = strtok(NULL, " ");

						if(status == NULL)
							fprintf(stderr, "Debug Error: Proper usage: breakstatus [id] [status].\n");
						else
						{
							if(sscanf(id, "%x", &i) != 1)
								fprintf(stderr, "Debug Error: Proper usage: breakstatus [id] [status].\n");
							else
							{
								if(sscanf(status, "%x", &s) != 1)
									fprintf(stderr, "Debug Error: Proper usage: breakstatus [id] [status].\n");
								else
								{
									if(s > 1 || s < 0) 
										fprintf(stderr, "Debug Error: Invalid breakpoint status.\n");
									else
									{
										breakpoint *b_set;

										b_set = debug_breakpoint_get(root, i);

										if(b_set == NULL)
											fprintf(stderr, "Debug Error: Invalid ID.\n");
										else
										{
											b_set->enabled = (bool)s;
											printf("Set status of breakpoint %d (0x%x+0x%x) to %d.\n", i, b_set->offset, b_set->address, s);
										}
									}
								
								
							}
						}
					}
				}
			
				
				}
				else if(strcmp(cmdname, "printr") == 0)
				{
					//we need to print a value from a register
					char *reg = strtok(NULL, " ");

					if(reg == NULL)
						fprintf(stderr, "Debug Error: Register must be specified.\n");
					else
					{

						if(strcmp(reg, "ar1") == 0)
						{
							printf("ar1 - 0x%x (%d)\n", cpu->ar1, cpu->ar1);
						}
						else if(strcmp(reg, "ar2") == 0)
						{
							printf("ar2 - 0x%x (%d)\n", cpu->ar2, cpu->ar2);
						}
						else if(strcmp(reg, "ar3") == 0)
						{
							printf("ar3 - 0x%x (%d)\n", cpu->ar3, cpu->ar3);
						}
						else if(strcmp(reg, "ar4") == 0)
						{
							printf("ar4 - 0x%x (%d)\n", cpu->ar4, cpu->ar4);
						}
						else if(strcmp(reg, "ar5") == 0)
						{
							printf("ar5 - 0x%x (%d)\n", cpu->ar5, cpu->ar5);
						}
						else if(strcmp(reg, "ip") == 0)
						{
							printf("ip - 0x%x (%d)\n", cpu->ip, cpu->ip);
						}
						else if(strcmp(reg, "bp") == 0)
						{
							printf("bp - 0x%x (%d)\n", cpu->bp, cpu->bp);
						}
						else if(strcmp(reg, "sp") == 0)
						{
							printf("sp - 0x%x (%d)\n", cpu->sp, cpu->sb);
						}
						else if(strcmp(reg, "nr1") == 0)
						{
							printf("nr1 - 0x%x (%d)\n", cpu->nr1, cpu->nr1);
						}
						else if(strcmp(reg, "nr2") == 0)
						{
							printf("nr2 - 0x%x (%d)\n", cpu->nr2, cpu->nr2);
						}
						else if(strcmp(reg, "nr3") == 0)
						{
							printf("nr3 - 0x%x (%d)\n", cpu->nr3, cpu->nr3);
						}
						else if(strcmp(reg, "nr4") == 0)
						{
							printf("nr4 - 0x%x (%d)\n", cpu->nr4, cpu->nr4);
						}
						else if(strcmp(reg, "nr5") == 0)
						{
							printf("nr5 - 0x%x (%d)\n", cpu->nr5, cpu->nr5);
						}
						else if(strcmp(reg, "nr6") == 0)
						{
							printf("nr6 - 0x%x (%d)\n", cpu->nr6, cpu->nr6);
						}
						else if(strcmp(reg, "nr7") == 0)
						{
							printf("nr7 - 0x%x (%d)\n", cpu->nr7, cpu->nr7);
						}
						else if(strcmp(reg, "or1") == 0)
						{
							printf("or1 - 0x%x (%d)\n", cpu->or1, cpu->or1);
						}
						else if(strcmp(reg, "cr1") == 0)
						{
							printf("cr1 - 0x%x (%d)\n", cpu->cr1, cpu->cr1);
						}
						else if(strcmp(reg, "pr1") == 0)
						{
							printf("pr1 - 0x%x (%d)\n", cpu->pr1, cpu->pr1);
						}
						else if(strcmp(reg, "pr2") == 0)
						{
							printf("pr2 - 0x%x (%d)\n", cpu->pr2, cpu->pr2);
						}
						else if(strcmp(reg, "flr1") == 0)
						{
							printf("flr1 - 0x%x (%d)\n", cpu->flr1, cpu->flr1);
						}
						else
						{
							fprintf(stderr, "Debug Error: Register \'%s\' is not a valid register.\n", reg);
						}
					}
				}
				else if(strcmp(cmdname, "setr") == 0)
				{
					//we need to set a register's value
					char *reg = strtok(NULL, " ");
					char *value;

					uint32_t v;

					if(reg == NULL)
						fprintf(stderr, "Debug Error: Register must be specified.\n");
					else
					{
						value = strtok(NULL, " ");

						if(value == NULL)
							fprintf(stderr, "Debug Error: Value must be specified.\n");
						else
						{
							if(sscanf(value, "%x", &v) != 1)
								fprintf(stderr, "Debug Error: Value must be a number.\n");
							else
							{

								if(strcmp(reg, "ar1") == 0)
								{
									cpu->ar1 = v;
									
									printf("ar1 set to  0x%x (%d)\n", v, v);
									
								}
								else if(strcmp(reg, "ar2") == 0)
								{
									cpu->ar2 = v;
									
									printf("ar2 set to  0x%x (%d)\n", v, v);
								}
								else if(strcmp(reg, "ar3") == 0)
								{
									cpu->ar3 = v;
									
									printf("ar3 set to  0x%x (%d)\n", v, v);
								}
								else if(strcmp(reg, "ar4") == 0)
								{
									cpu->ar4 = v;
									
									printf("ar4 set to  0x%x (%d)\n", v, v);
								}
								else if(strcmp(reg, "ar5") == 0)
								{
									cpu->ar5 = v;
									
									printf("ar5 set to  0x%x (%d)\n", v, v);
								}
								else if(strcmp(reg, "ip") == 0)
								{
									cpu->ip = v;
									
									printf("ip set to  0x%x (%d)\n", v, v);
								}
								else if(strcmp(reg, "bp") == 0)
								{
									cpu->bp = v;
									
									printf("bp set to  0x%x (%d)\n", v, v);
								}
								else if(strcmp(reg, "sp") == 0)
								{
									cpu->sp = v;
									
									printf("sp set to  0x%x (%d)\n", v, v);
								}
								else if(strcmp(reg, "nr1") == 0)
								{
									cpu->nr1 = v;
									
									printf("nr1 set to  0x%x (%d)\n", v, v);
								}
								else if(strcmp(reg, "nr2") == 0)
								{
									cpu->nr2 = v;
									
									printf("nr2 set to  0x%x (%d)\n", v, v);
								}
								else if(strcmp(reg, "nr3") == 0)
								{
									cpu->nr3 = v;
									
									printf("nr3 set to  0x%x (%d)\n", v, v);
								}
								else if(strcmp(reg, "nr4") == 0)
								{
									cpu->nr4 = v;
									
									printf("nr4 set to  0x%x (%d)\n", v, v);
								}
								else if(strcmp(reg, "nr5") == 0)
								{
									cpu->nr5= v;
									
									printf("nr5 set to  0x%x (%d)\n", v, v);
								}
								else if(strcmp(reg, "nr6") == 0)
								{
									printf("nr6 - 0x%x (%d)\n", cpu->nr6, cpu->nr6);
								}
								else if(strcmp(reg, "nr7") == 0)
								{
									cpu->nr7 = v;
									
									printf("nr7 set to  0x%x (%d)\n", v, v);
								}
								else if(strcmp(reg, "or1") == 0)
								{
									cpu->or1 = v;
									
									printf("or1 set to  0x%x (%d)\n", v, v);
								}
								else if(strcmp(reg, "cr1") == 0)
								{
									cpu->cr1 = v;
									
									printf("cr1 set to  0x%x (%d)\n", v, v);
								}
								else if(strcmp(reg, "pr1") == 0)
								{
									cpu->pr1 = v;
									
									printf("pr1 set to  0x%x (%d)\n", v, v);
								}
								else if(strcmp(reg, "pr2") == 0)
								{
									cpu->pr2 = v;
									
									printf("pr2 set to  0x%x (%d)\n", v, v);
								}
								else if(strcmp(reg, "flr1") == 0)
								{
									cpu->flr1 = v;
									
									printf("flr1 set to  0x%x (%d)\n", v, v);
								}
								else
								{
									fprintf(stderr, "Debug Error: Register \'%s\' is not a valid register.\n", reg);
								}
							}
						}
					}
				}
				else if(strcmp(cmdname, "memdump") == 0)
				{
					char *dumpfile;

					dumpfile = strtok(NULL, " ");

					if(dumpfile == NULL)
						fprintf(stderr, "Debug Error: Proper usage: memdump [filename]\n");
					else
					{
						if(!write_ram_contents_to_file(ram, dumpfile))
							fprintf(stderr, "Debug Error: Unable to write RAM dump to \'%s\'.\n", dumpfile);
						else
						{
							fprintf(stderr, "Wrote RAM dump to \'%s\'.\n", dumpfile);
							
						}
					}
				}
				else if(strcmp(cmdname, "printm") == 0)
				{
					//we need to print a value from memory
					char *type;
					char *offset;
					char *address;

					uint32_t o;
					uint32_t a;
							

					type = strtok(NULL, " ");

					if(type == NULL)
						fprintf(stderr, "Debug Error: Proper usage: printm [type] [offset] [address]\n");
					else
					{
						offset = strtok(NULL, " ");

						if(offset == NULL)
							fprintf(stderr, "Debug Error: Proper usage: printm [type] [offset] [address]\n");
						else
						{
							address = strtok(NULL, " ");

							if(address == NULL)
								fprintf(stderr, "Debug Error: Proper usage: printm [type] [offset] [address]\n");
							else
							{
								//conver to integers
								
								if(sscanf(offset, "%x", &o) != 1)
									fprintf(stderr, "Debug Error: Proper usage: printm [type] [offset] [address]\n");
								else
								{
									if(sscanf(address, "%x", &a) != 1)
										fprintf(stderr, "Debug Error: Proper usage: printm [type] [offset] [address]\n");
									else
									{
										switch(type[0])
										{
										case 's':
										{
											if(o+a >= ram->bytesize)
												fprintf(stderr, "Debug Error: Address out of bounds.\n");
											else
											{
												size_t str_loc;

												str_loc = (size_t)ram->data;
												str_loc += (size_t)o;
												str_loc += (size_t)a;
												printf("string at 0x%x - %s\n", o+a, (char*)(str_loc));
											}

											break;
										}
										case 'd':
										{
											emu_error = 0;

											if(o+a >= ram->bytesize)
												fprintf(stderr, "Debug Error: Address out of bounds.\n");

											uint32_t data = get_dword_at_ram_address(ram, o+a);

											if(emu_error != 0)
												fprintf(stderr, "Debug Error: Error retrieving data from memory.\n");
											else
												printf("DWORD at 0x%x - 0x%x (%d)\n", o+a, data, data);

											break;
										}
										case 'b':
										{
											emu_error = 0;

											if(o+a >= ram->bytesize)
												fprintf(stderr, "Debug Error: Address out of bounds.\n");

											uint8_t data = get_byte_at_ram_address(ram, o+a);

											if(emu_error != 0)
												fprintf(stderr, "Debug Error: Error retrieving data from memory.\n");
											else
												printf("BYTE at 0x%x - 0x%x (%d)\n", o+a, data, data);

											break;
										}
										case 'c':
										{
											emu_error = 0;

											if(o+a >= ram->bytesize)
												fprintf(stderr, "Debug Error: Address out of bounds.\n");

											uint8_t data = get_byte_at_ram_address(ram, o+a);

											if(emu_error != 0)
												fprintf(stderr, "Debug Error: Error retrieving data from memory.\n");
											else
												printf("character at 0x%x - %c (0x%x)\n", o+a, (char)data, data);

											break;
										}
										default:
										{
											fprintf(stderr, "Debug Error: Unrecognized type \'%c\'.\n", type[0]);
										}
										}
											
									}
								}
							
							}
						}
					}
				}
				else if(strcmp(cmdname, "setm") == 0)
				{
					//we need to print a value from memory
					char *type;
					char *offset;
					char *address;
					char *value;

					uint32_t o;
					uint32_t a;
					uint32_t v;
							

					type = strtok(NULL, " ");

					if(type == NULL)
						fprintf(stderr, "Debug Error: Proper usage: setm [type] [offset] [address] [value]\n");
					else
					{
						offset = strtok(NULL, " ");

						if(offset == NULL)
							fprintf(stderr, "Debug Error: Proper usage: setm [type] [offset] [address] [value]\n");
						else
						{
							address = strtok(NULL, " ");

							if(address == NULL)
								fprintf(stderr, "Debug Error: Proper usage: setm [type] [offset] [address] [value]\n");
							else
							{
								//conver to integers
								
								if(sscanf(offset, "%x", &o) != 1)
									fprintf(stderr, "Debug Error: Proper usage: setm [type] [offset] [address] [value]\n");
								else
								{
									if(sscanf(address, "%x", &a) != 1)
										fprintf(stderr, "Debug Error: Proper usage: setm [type] [offset] [address] [value]\n");
									else
									{
										value = strtok(NULL, "");

										if(value == NULL)
											fprintf(stderr, "Debug Error: Proper usage: setm [type] [offset] [address] [value]\n");
										else
										{
											switch(type[0])
											{
											case 's':
											{
												bool st_error = false;
												if(o+a+strlen(value)+1 >= ram->bytesize)
													fprintf(stderr, "Debug Error: Address out of bounds (or string will not fit).\n");
												else
												{
													for(int s_it = 0; s_it <= strlen(value); s_it++)
													{
														emu_error = 0;

														set_byte_at_ram_address(ram, o+a+s_it, value[s_it]);

														if(emu_error != 0)
														{
															fprintf(stderr, "Debug Error: Memory error.\n");
															st_error = true;
														}

													}

													if(!st_error)
														printf("string at 0x%x set to \"%s\"\n", o+a, value);
												}

												break;
											}
											case 'd':
											{

												
												
												emu_error = 0;

												if(o+a+3 >= ram->bytesize)
													fprintf(stderr, "Debug Error: Address out of bounds.\n");

												if(sscanf(value, "%x", &v) != 1 )
													fprintf(stderr, "Debug Error: Proper usage: setm [type] [offset] [address] [value]\n");
												else
												{

													set_dword_at_ram_address(ram, o+a, v);

													if(emu_error != 0)
														fprintf(stderr, "Debug Error: Memory error.\n");
													else
														printf("DWORD at 0x%x set to  0x%x (%d)\n", o+a, v, v);
												}

												break;
											}
											case 'b':
											{
												
												
												emu_error = 0;

												if(o+a+3 >= ram->bytesize)
													fprintf(stderr, "Debug Error: Address out of bounds.\n");

												if(sscanf(value, "%x", &v) != 1 )
													fprintf(stderr, "Debug Error: Proper usage: setm [type] [offset] [address] [value]\n");
												else
												{

													set_byte_at_ram_address(ram, o+a, (uint8_t)v);

													if(emu_error != 0)
														fprintf(stderr, "Debug Error: Memory error.\n");
													else
														printf("BYTE at 0x%x set to  0x%x (%d)\n", o+a, v, v);
												}

												break;
											}
											case 'c':
											{
												emu_error = 0;

												if(o+a >= ram->bytesize)
													fprintf(stderr, "Debug Error: Address out of bounds.\n");

												set_byte_at_ram_address(ram, o+a, (uint8_t)value[0]);

												if(emu_error != 0)
													fprintf(stderr, "Debug Error: Memory error.\n");
												else
													printf("character at 0x%x set to %c  (0x%x)\n", o+a, value[0], (uint8_t)value[0]);
											

												break;
											}
											default:
											{
												fprintf(stderr, "Debug Error: Unrecognized type \'%c\'.\n", type[0]);
											}
											}
										}
											
									}
								}
							
							}
						}
					}
				}			       
				else
				{
					fprintf(stderr, "Debug Error: Command \'%s\' not recognized or was passed invalid arguments.\n", cmdname);
				}
			}
			else
			{
				fprintf(stderr, "Debug Error: Command \'%s\' not recognized.\n", to_parse);
			}
		}

		free(to_parse);	
	}

	return DEBUG_SIGNAL_NOSIGNAL;
}

void debug_do_regdump(CPU *cpu)
{

	printf("Register Dump\n\n");
	
	printf("ar1 - %x\n", cpu->ar1);
	printf("ar2 - %x\n", cpu->ar2);
	printf("ar3 - %x\n", cpu->ar3);
	printf("ar4 - %x\n", cpu->ar4);
	printf("ar5 - %x\n", cpu->ar5);
	printf("ip  - %x\n", cpu->ip);
	printf("bp  - %x\n", cpu->bp);
	printf("sp  - %x\n", cpu->sp);
	printf("sb  - %x\n", cpu->sb);
	/*
	printf("fr1 - %f\n", cpu->fr1);
	printf("fr2 - %f\n", cpu->fr2);
	printf("fr3 - %f\n", cpu->fr3);
	printf("fr4 - %f\n", cpu->fr4);
	printf("fr5 - %f\n", cpu->fr5);\
	*/
	printf("nr1 - %x\n", cpu->nr1);
	printf("nr2 - %x\n", cpu->nr2);
	printf("nr3 - %x\n", cpu->nr3);
	printf("nr4 - %x\n", cpu->nr4);
	printf("nr5 - %x\n", cpu->nr5);
	printf("nr6 - %x\n", cpu->nr6);
	printf("nr7 - %x\n", cpu->nr7);
	printf("or1 - %x\n", cpu->or1);
	printf("cr1 - %x\n", cpu->cr1);
	printf("pr1 - %x\n", cpu->pr1);
	printf("pr2 - %x\n", cpu->pr2);
	/*
	printf("fpr1 - %f\n", cpu->fpr1);
	printf("fpr2 - %f\n", cpu->fpr2);
	*/
	printf("flr1 - %x\n", cpu->flr1);
}
	
