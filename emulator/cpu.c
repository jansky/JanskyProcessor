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

	cpu->nr1 = 0;
	cpu->nr2 = 0;
	cpu->nr3 = 0;
	cpu->nr4 = 0;
	cpu->nr5 = 0;

	cpu->or1 = 0;
	
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
		return cpu->sb;
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
	case 0x10:
		return cpu->nr1;
		break;
	case 0x11:
		return cpu->nr2;
		break;
	case 0x12:
		return cpu->nr3;
		break;
	case 0x13:
		return cpu->nr4;
		break;
	case 0x14:
		return cpu->nr5;
		break;
	case 0x15:
		return cpu->nr6;
		break;
	case 0x16:
		return cpu->nr7;
		break;
	case 0x17:
		return cpu->or1;
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
	case 0x10:
		cpu->nr1 = value;
		break;
	case 0x11:
		cpu->nr2 = value;
		break;
	case 0x12:
		cpu->nr3 = value;
		break;
	case 0x13:
		cpu->nr4 = value;
		break;
	case 0x14:
		cpu->nr5 = value;
		break;
	case 0x15:
		cpu->nr6 = value;
		break;
	case 0x16:
		cpu->nr7 = value;
		break;
	case 0x17:
		cpu->or1 = value;
		break;
	default:
		return false;
		break;
	}
	return true;
}

DWORD get_value(CPU *cpu, RAMUNIT *ram, BYTE loc_id, DWORD location)
{
	DWORD offset = cpu->or1;
	
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
	case 0x05:
		return (BYTE)get_value_from_register(cpu, location);
		break;
	case 0x06:
		return get_byte_at_ram_address(ram, location);
		break;
	case 0x07:
		return (BYTE)location;
		break;
	case 0x08:
		return get_byte_at_ram_address(ram, get_value_from_register(cpu, location));
		break;
	case 0x09:
		return get_byte_at_ram_address(ram, get_dword_at_ram_address(ram, location));
		break;
	case 0x0A:
		return get_dword_at_ram_address(ram, offset+location);
		break;
	case 0x0B:
	{
		
		return (offset+location);
		break;
	}
	case 0x0C:
		return get_byte_at_ram_address(ram, offset+location);
		break;
	case 0x0D:
		return (BYTE)(offset+location);
		break;
	default:
		emu_error = EMUERR_OUTOFRANGE;
		return 0;
		break;
	}
}

bool put_value(CPU *cpu, RAMUNIT *ram, BYTE loc_id, DWORD location, DWORD value)
{
	DWORD offset = cpu->or1;
	
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
	case 0x03:
	{
		set_dword_at_ram_address(ram, get_value_from_register(cpu, location), value);

		if(emu_error != 0)
			return false;
		break;
	}
	case 0x04:
	{
		set_dword_at_ram_address(ram, get_dword_at_ram_address(ram, location), value);

		if(emu_error != 0)
			return false;
	}
	
	case 0x05:
			
		return put_value_in_register(cpu, location, (BYTE)value);
		break;
	case 0x06:
			
		set_byte_at_ram_address(ram, location, (BYTE)value);
			
		if(emu_error != 0)
			return false;
			
		break;
	case 0x08:
	{
		set_byte_at_ram_address(ram, get_value_from_register(cpu, location), (BYTE)value);

		if(emu_error != 0)
			return false;
		break;
	}
	case 0x09:
	{
		set_byte_at_ram_address(ram, get_dword_at_ram_address(ram, location), (BYTE)value);

		if(emu_error != 0)
			return false;
	}
	case 0x0A:
	{
		set_dword_at_ram_address(ram, offset+location, value);
			
		if(emu_error != 0)
			return false;
		break;
	}
	case 0x0C:
	{
		set_byte_at_ram_address(ram, offset+location, (BYTE)value);
			
		if(emu_error != 0)
			return false;
		
		break;
	}	
	default:
		return false;
		break;
	}
	
	return true;
}

CPURESULT *cpu_exec_instruction(CPU *cpu, RAMUNIT *ram, bool oninterrupt)
{
	uint32_t curr_address = (cpu->ip);
	BYTE instruction;

	bool update_ip = true;

	if(!oninterrupt)
		curr_address += cpu->or1;
	
	
	if(cpu->halted)
	{
		emu_error = EMUERR_ILLEGALOPERATION;
		return NULL;
	}

	//printf("%d (0x%x) - ", curr_address, curr_address);
	
	instruction = get_byte_at_ram_address(ram, curr_address);
	
	//printf("%d - ", curr_address);
	//printf("%x - ", instruction);

	//printf("0x%x\n", instruction);
	
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
		//INT
	case 0x03:
	{
//			printf("Interrupt called.\n");
		if(curr_address + 1 >= ram->bytesize)
		{
			return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);
		}
			
		curr_address += 2;
		cpu->ip = curr_address;

		if(!oninterrupt)
			cpu->ip -= cpu->or1;
			
		return cpu_result_create(get_byte_at_ram_address(ram, curr_address - 1), curr_address, instruction);
		break;
	}
			
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
		DWORD ret_address = curr_address + 5;

		if(!oninterrupt)
			ret_address -= cpu->or1;
		
		if(!stack_push(cpu, ram, ret_address))
			return cpu_result_create(CPURESULT_STACKFLOWERROR, curr_address, instruction);
			
		BYTE loc_id = get_byte_at_ram_address(ram, curr_address);
		curr_address++;
		DWORD location = get_dword_at_ram_address(ram, curr_address);
			
		DWORD value = get_value(cpu, ram, loc_id, location);

	


	       	if(emu_error != 0)
			return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);

		if(!oninterrupt)
			value += cpu->or1;
			
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

		if(!oninterrupt)
			ret_address += cpu->or1;
				
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

		if(!oninterrupt)
			ret_address -= cpu->or1;
			
			
			
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

			if(!oninterrupt)
				value += cpu->or1;

			
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

		if(!oninterrupt)
		{
			//value2 -= cpu->or1;
			value1 += cpu->or1;
		}
			
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
			if(!oninterrupt)
			{
				//value2 -= cpu->or1;
				value1 += cpu->or1;
			}
			
			//Push RET pointer onto stack
			if(!stack_push(cpu, ram, value2))
				return cpu_result_create(CPURESULT_STACKFLOWERROR, curr_address, instruction);
			curr_address = value1;
		}
			
			
			
			
			
			
				
			
			
		break;
	}
	//IRET = 0x15 (handled in interrupt.c)
	//UIP
	case 0x16:
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
				
			
		//update register or1
		if(!put_value(cpu, ram, 0x00, 0x17, value1))
			return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);

		//update register ip
		if(!put_value(cpu, ram, 0x00, 0x05, value2))
			return cpu_result_create(CPURESULT_ILLEGALACCESS, curr_address, instruction);

		update_ip = false;
			
		
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

	if(update_ip)
	{
		cpu->ip = curr_address;
		if(!oninterrupt)
			cpu->ip -= cpu->or1;
	}
	
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
	printf("nr1 - %x\n", cpu->nr1);
	printf("nr2 - %x\n", cpu->nr2);
	printf("nr3 - %x\n", cpu->nr3);
	printf("nr4 - %x\n", cpu->nr4);
	printf("nr5 - %x\n", cpu->nr5);
	printf("nr6 - %x\n", cpu->nr6);
	printf("or1 - %x\n", cpu->or1);
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
	//printf("stack push called.\n");
	
	if(insertwhere < cpu->sb)
	{
		//printf("stack push failure line 1007\n");
		return false;
	}
	
	set_dword_at_ram_address(ram, insertwhere, value);
	
	if(emu_error != 0)
	{
		//printf("stack push failure line 1015\n");
		return false;
	}
	
	cpu->sp -= 4;

	//printf("stack push value 0x%x\n", value);
	
	return true;
}

DWORD stack_pop(CPU *cpu, RAMUNIT *ram)
{
	//printf("stack pop called.\n");
	emu_error = 0;
	if(cpu->sp + 4 >= ram->bytesize)
	{
		//printf("stack pop failure line 1029\n");
		emu_error = EMUERR_OUTOFRANGE;
		return 0;
	}
	
	DWORD value = get_dword_at_ram_address(ram, cpu->sp + 1);
	
	if(emu_error != 0)
	{
		//printf("stack pop failure line 1038\n");
		return 0;
	}
	
	cpu->sp += 4;
	
	return value;
}
		
	
	
	
		
	
	
	
	
	
	
	
