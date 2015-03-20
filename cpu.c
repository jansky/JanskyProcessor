#include <stdlib.h>
#include <stdio.h>
#include "cpu.h"
#include "error.h"

CPU *cpu_init(void)
{
	emu_error = 0;
	
	CPU *cpu = malloc(sizeof(CPU));
	
	//Check if Memory Allocation succeeded?
	if(cpu == NULL)
	{
		emu_error = EMUERR_ALLOCERROR;
		return NULL;
	}
	
		
	//Initialize all the registers
	
	cpu->ar1 = 0;
	cpu->ar2 = 0;
	cpu->ar3 = 0;
	cpu->ar4 = 0;
	cpu->ar5 = 0;
	
	cpu->ip = 0;
	cpu->bp = 0;
	
	cpu->sp = 0;
	cpu->sb = 0;
	
	cpu->fr1 = 0.0;
	cpu->fr2 = 0.0;
	cpu->fr3 = 0.0;
	cpu->fr4 = 0.0;
	cpu->fr5 = 0.0;
	
	cpu->cr1 = 0;
	
	cpu->pr1 = 0;
	cpu->pr2 = 0;
	
	cpu->fpr1 = 0.0;
	cpu->fpr2 = 0.0;
	
	cpu->flr1 = 0;
	
	//We're online
	cpu->halted = false;
	
	return cpu;
}

CPURESULT *cpu_result_create(int error, uint32_t address, BYTE instruction)
{
	CPURESULT *result = malloc(sizeof(CPURESULT));
	
	if(result == NULL)
	{
		emu_error = EMUERR_ALLOCERROR;
	}
	
	result->error = error;
	result->address = address;
	result->instruction = instruction;
	
	return result;
}

DWORD get_value_from_register(CPU *cpu, DWORD regid)
{
	switch(regid)
	{
					
						case 0x00:
							return cpu->ar1;
							break;
						case 0x01:
							return cpu->ar2;
							break;
						case 0x02:
							return cpu->ar3;
							break;
						case 0x03:
							return cpu->ar4;
							break;
						case 0x04:
							return cpu->ar5;
							break;
						case 0x05:
							return cpu->ip;
							break;
						case 0x06:
							return cpu->bp;
							break;
						case 0x07:
							return cpu->sp;
							break;
						case 0x08:
							return cpu->bp;
							break;
						//FPU not implemented yet
						case 0x0A:
							cpu->cr1;
							break;
						case 0x0B:
							return cpu->pr1;
							break;
						case 0x0C:
							return cpu->pr2;
							break;
						case 0x0F:
							return cpu->flr1;
							break;
						default:
							emu_error = EMUERR_OUTOFRANGE;
							break;
					}
	return 0;
}

bool put_value_in_register(CPU *cpu, DWORD regid, DWORD value)
{
	//printf("%x\n", regid);
	switch(regid)
	{
					
						case 0x00:
							cpu->ar1 = value;
							break;
						case 0x01:
							cpu->ar2 = value;
							break;
						case 0x02:
							cpu->ar3 = value;
							break;
						case 0x03:
							cpu->ar4 = value;
							break;
						case 0x04:
							cpu->ar5 = value;
							break;
						case 0x05:
							cpu->ip = value;
							break;
						case 0x06:
							cpu->bp = value;
							break;
						case 0x07:
							cpu->sp = value;
							break;
						case 0x08:
							cpu->sb = value;
							break;
						//FPU not implemented yet
						case 0x0A:
							cpu->cr1 = value;
							break;
						case 0x0B:
							cpu->pr1 = value;
							break;
						case 0x0C:
							cpu->pr2 = value;
							break;
						case 0x0F:
							cpu->flr1 = value;
							break;
						default:
							return false;
							break;
					}
	return true;
}

DWORD get_value(CPU *cpu, RAMUNIT *ram, BYTE loc_id, DWORD location)
{
	switch(loc_id)
	{
		case 0x00:
			return get_value_from_register(cpu, location);
			break;
		case 0x01:
			return get_dword_at_ram_address(ram, location);
			break;
		case 0x02:
			return location;
			break;
		case 0x03:
			return get_dword_at_ram_address(ram, get_value_from_register(cpu, location));
			break;
		case 0x04:
			return get_dword_at_ram_address(ram, get_dword_at_ram_address(ram, location));
			break;
		default:
			emu_error = EMUERR_OUTOFRANGE;
			return 0;
	}
}

bool put_value(CPU *cpu, RAMUNIT *ram, BYTE loc_id, DWORD location, DWORD value)
{
	switch(loc_id)
	{
		case 0x00:
			
			return put_value_in_register(cpu, location, value);
			break;
		case 0x01:
			
			set_dword_at_ram_address(ram, location, value);
			
			if(emu_error != 0)
				return false;
			
			break;
		default:
			return false;
	}
	
	return true;
}

CPURESULT *cpu_exec_instruction(CPU *cpu, RAMUNIT *ram)
{
	uint32_t curr_address = cpu->ip;
	BYTE instruction;
	
	
	if(cpu->halted)
	{
		emu_error = EMUERR_ILLEGALOPERATION;
		return NULL;
	}
	
	instruction = get_byte_at_ram_address(ram, curr_address);
	
	//printf("%d - ", curr_address);
	//printf("%x - ", instruction);
	
	if(emu_error != 0)
	{
		//We have a problem
		//Illegal memory access
		return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, 0xFF);
	}
	
	switch(instruction)
	{
		//HLT
		case 0x00:
			printf("CPU Halted\n");
			cpu->halted = true;
			break;
		//PUT
		case 0x05:
		
			
			
			if(curr_address + 9 >= ram->bytesize)
			{
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			}
			
			BYTE loc_id;
			DWORD location;
			DWORD value;
			
			curr_address++;
			
			loc_id = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			
			location = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			value = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			if(!put_value(cpu, ram, loc_id, location, value))
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			
			
			
			
			break;
		//CPY
		case 0x06:
		
			if(curr_address + 10 >= ram->bytesize)
			{
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			}
			
			BYTE loc_id_1;
			DWORD location_1;
			
			BYTE loc_id_2;
			DWORD location_2;
			
			DWORD value1;
			
			
			curr_address++;
			
			loc_id_1 = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			location_1 = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			loc_id_2 = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			
			location_2 = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			//Now we read and set the values
			
			value1 = get_value(cpu, ram, loc_id_1, location_1);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
				
			if(!put_value(cpu, ram, loc_id_2, location_2, value1))
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
				
			
			
			break;
		//ADD
		case 0x07:
		{
		
			if(curr_address + 10 >= ram->bytesize)
			{
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			}
			
			BYTE loc_id_1;
			DWORD location_1;
			
			BYTE loc_id_2;
			DWORD location_2;
			
			DWORD value1;
			DWORD value2;
			
			curr_address++;
			
			loc_id_1 = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			location_1 = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			loc_id_2 = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			
			location_2 = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			//Now we read the values
			
			value1 = get_value(cpu, ram, loc_id_1, location_1);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
				
			value2 = get_value(cpu, ram, loc_id_2, location_2);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
				
			
			
			if(!put_value(cpu, ram, loc_id_1, location_1, value1 + value2))
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
				
			
			
			break;
		}
		//SUB
		case 0x08:
		{
			if(curr_address + 10 >= ram->bytesize)
			{
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			}
			
			BYTE loc_id_1;
			DWORD location_1;
			
			BYTE loc_id_2;
			DWORD location_2;
			
			DWORD value1;
			DWORD value2;
			
			curr_address++;
			
			loc_id_1 = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			location_1 = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			loc_id_2 = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			
			location_2 = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			//Now we read the values
			
			value1 = get_value(cpu, ram, loc_id_1, location_1);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
				
			value2 = get_value(cpu, ram, loc_id_2, location_2);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
				
			
			
			if(!put_value(cpu, ram, loc_id_1, location_1, value1 - value2))
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
				
			
			
			break;
		}
		//MUL
		case 0x09:
		{
			if(curr_address + 10 >= ram->bytesize)
			{
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			}
			
			BYTE loc_id_1;
			DWORD location_1;
			
			BYTE loc_id_2;
			DWORD location_2;
			
			DWORD value1;
			DWORD value2;
			
			curr_address++;
			
			loc_id_1 = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			location_1 = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			loc_id_2 = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			
			location_2 = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			//Now we read the values
			
			value1 = get_value(cpu, ram, loc_id_1, location_1);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
				
			value2 = get_value(cpu, ram, loc_id_2, location_2);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
				
			
			
			if(!put_value(cpu, ram, loc_id_1, location_1, value1 * value2))
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
				
				
				
			
			
			break;
		}
		//DIV
		case 0x0A:
		{
			if(curr_address + 10 >= ram->bytesize)
			{
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			}
			
			BYTE loc_id_1;
			DWORD location_1;
			
			BYTE loc_id_2;
			DWORD location_2;
			
			DWORD value1;
			DWORD value2;
			
			curr_address++;
			
			loc_id_1 = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			location_1 = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			loc_id_2 = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			
			location_2 = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			//Now we read the values
			
			value1 = get_value(cpu, ram, loc_id_1, location_1);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
				
			value2 = get_value(cpu, ram, loc_id_2, location_2);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
				
			
			
			if(!put_value(cpu, ram, loc_id_1, location_1, value1 / value2))
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			
			//Store Remainder in cr1
			cpu->cr1 = value1 % value2;
			
				
			
			
			break;
		}
		//PUSH
		case 0x0B:
		{
			emu_error = 0;
			
			if(curr_address + 4 >= ram->bytesize)
			{
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			}
			
			BYTE loc_id;
			DWORD location;
			DWORD value;
			
			curr_address++;
			
			loc_id = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			location = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			value = get_value(cpu, ram, loc_id, location);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			
			if(!stack_push(cpu, ram, value))
				return cpu_result_create(CPURESULT_STACKFLOWERROR, curr_address, instruction);
			
			
					
				
			break;
		}
		//POP
		case 0x0C:
		{
			emu_error = 0;
			
			if(curr_address + 4 >= ram->bytesize)
			{
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			}
			
			BYTE loc_id;
			DWORD location;
			DWORD value;
			
			curr_address++;
			
			loc_id = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			location = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			value = stack_pop(cpu, ram);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_STACKFLOWERROR, curr_address, instruction);
			
			if(!put_value(cpu, ram, loc_id, location, value))
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			
			
			break;
		}
		//JMP
		case 0x0F:
		{
			if(curr_address + 5 >= ram->bytesize)
			{
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			}
			
			
			curr_address++;
			
			//Push RET pointer onto stack
			if(!stack_push(cpu, ram, curr_address + 5))
				return cpu_result_create(CPURESULT_STACKFLOWERROR, curr_address, instruction);
			
			BYTE loc_id = get_byte_at_ram_address(ram, curr_address);
			curr_address++;
			DWORD location = get_dword_at_ram_address(ram, curr_address);
			
			DWORD value = get_value(cpu, ram, loc_id, location);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			
			curr_address = value;
			
			break;
		}
		//CMP
		case 0x10:
		{
		
			if(curr_address + 10 >= ram->bytesize)
			{
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			}
			
			BYTE loc_id_1;
			DWORD location_1;
			
			BYTE loc_id_2;
			DWORD location_2;
			
			DWORD value1;
			DWORD value2;
			
			curr_address++;
			
			loc_id_1 = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			location_1 = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			loc_id_2 = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			
			location_2 = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			//Now we read the values
			
			value1 = get_value(cpu, ram, loc_id_1, location_1);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
				
			value2 = get_value(cpu, ram, loc_id_2, location_2);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
				
		    
		    if(value1 == value2)
		    {
				cpu->flr1 = 0x01;
			}
			else
			{
				cpu->flr1 = 0x00;
				
				if(value1 > value2)
				{
					cpu->flr1 = 0x02;
				}
				
				if(value1 < value2)
				{
					cpu->flr1 = 0x03;
				}
			}
			
							
			
			
			break;
		}
		//RET
		case 0x11:
		{
			DWORD ret_address = stack_pop(cpu, ram);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_STACKFLOWERROR, curr_address, instruction);
				
			curr_address = ret_address;
			
			break;
		}
		//JMPC
		case 0x12:
		{
			if(curr_address + 6 >= ram->bytesize)
			{
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			}
			
			
			curr_address++;
			DWORD ret_address = curr_address + 6;
			
			
			
			BYTE condition = get_byte_at_ram_address(ram, curr_address);
			curr_address++;
			BYTE loc_id = get_byte_at_ram_address(ram, curr_address);
			curr_address++;
			DWORD location = get_dword_at_ram_address(ram, curr_address);
			curr_address += 4;
			
			DWORD value = get_value(cpu, ram, loc_id, location);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			
			bool condition_met = false;
			
			if(condition == cpu->flr1 || (condition == 0x00 && cpu->flr1 != 1))
				condition_met = true;
				
				
			if(condition_met)
			{
				//Push RET pointer onto stack
				if(!stack_push(cpu, ram, ret_address))
					return cpu_result_create(CPURESULT_STACKFLOWERROR, curr_address, instruction);
				curr_address = value;
			}
			
			break;
		}
		//JMPR
		case 0x13:
		{
			if(curr_address + 10 >= ram->bytesize)
			{
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			}
			
			BYTE loc_id_1;
			DWORD location_1;
			
			BYTE loc_id_2;
			DWORD location_2;
			
			DWORD value1;
			DWORD value2;
			
			curr_address++;
			
			loc_id_1 = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			location_1 = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			loc_id_2 = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			
			location_2 = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			//Now we read the values
			
			value1 = get_value(cpu, ram, loc_id_1, location_1);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
				
			value2 = get_value(cpu, ram, loc_id_2, location_2);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			
			//Push RET pointer onto stack
			if(!stack_push(cpu, ram, value2))
				return cpu_result_create(CPURESULT_STACKFLOWERROR, curr_address, instruction);
			
			curr_address = value1;
			
			
			
			
			
			
				
			
			
			break;
		}
		//JMPRC
		case 0x14:
		{
			if(curr_address + 11 >= ram->bytesize)
			{
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			}
			
			BYTE condition;
			BYTE loc_id_1;
			DWORD location_1;
			
			BYTE loc_id_2;
			DWORD location_2;
			
			DWORD value1;
			DWORD value2;
			
			curr_address++;
			
			condition = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			
			loc_id_1 = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			location_1 = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			loc_id_2 = get_byte_at_ram_address(ram, curr_address);
			
			curr_address++;
			
			location_2 = get_dword_at_ram_address(ram, curr_address);
			
			curr_address += 4;
			
			//Now we read the values
			
			value1 = get_value(cpu, ram, loc_id_1, location_1);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
				
			value2 = get_value(cpu, ram, loc_id_2, location_2);
			
			if(emu_error != 0)
				return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
			
			bool condition_met = false;
			
			if(condition == cpu->flr1 || (condition == 0x00 && cpu->flr1 != 1))
				condition_met = true;
			
			
				
			
				
				
			if(condition_met)
			{
				//Push RET pointer onto stack
				if(!stack_push(cpu, ram, value2))
					return cpu_result_create(CPURESULT_STACKFLOWERROR, curr_address, instruction);
				curr_address = value1;
			}
			
			
			
			
			
			
				
			
			
			break;
		}
		
			
		case 0xFF:
			printf("Test.\n");
			cpu->ar1 = 254;
			curr_address++;
			break;
		default:
			//Instruction not defined
			return cpu_result_create(CPURESULT_INSTRUCTIONNOEXIST, curr_address, instruction);
			break;
	}
	
	//printf("%d\n", curr_address);
	
	cpu->ip = curr_address;
	
	return cpu_result_create(CPURESULT_SUCCESS, curr_address, instruction);
}

void print_end_dump(CPU *cpu, CPURESULT *result, RAMUNIT *ram)
{
	printf("\n-----\n");
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
	printf("cr1 - %x\n", cpu->cr1);
	printf("pr1 - %x\n", cpu->pr1);
	printf("pr2 - %x\n", cpu->pr2);
	/*
	printf("fpr1 - %f\n", cpu->fpr1);
	printf("fpr2 - %f\n", cpu->fpr2);
	*/
	printf("flr1 - %x\n", cpu->flr1);
	
	printf("CPU last instruction: %x\n", result->instruction);
	
	printf("CPU last instruction result: %x\n", result->error);
}

bool stack_push(CPU *cpu, RAMUNIT *ram, DWORD value)
{
	emu_error = 0;
	uint32_t insertwhere = cpu->sp;
	insertwhere -= 3;
	
	if(insertwhere < cpu->sb)
	{
		return false;
	}
	
	set_dword_at_ram_address(ram, insertwhere, value);
	
	if(emu_error != 0)
	{
		return false;
	}
	
	cpu->sp -= 4;
	
	return true;
}

DWORD stack_pop(CPU *cpu, RAMUNIT *ram)
{
	emu_error = 0;
	if(cpu->sp + 4 >= ram->bytesize)
	{
		emu_error = EMUERR_OUTOFRANGE;
		return 0;
	}
	
	DWORD value = get_dword_at_ram_address(ram, cpu->sp + 1);
	
	if(emu_error != 0)
		return 0;
	
	cpu->sp += 4;
	
	return value;
}
		
	
	
	
		
	
	
	
	
	
	
	
