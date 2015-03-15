#pragma once
#include <stdbool.h>
#include "types.h"
#include "memory.h"

typedef struct CPU
{
	bool halted;
	
	//Registers
	DWORD ar1;
	DWORD ar2;
	DWORD ar3;
	DWORD ar4;
	DWORD ar5;
	
	DWORD ip;
	DWORD bp;
	
	DWORD sp;
	DWORD sb;
	
	float fr1;
	float fr2;
	float fr3;
	float fr4;
	float fr5;
	
	DWORD cr1;
	
	DWORD pr1;
	DWORD pr2;
	
	float fpr1;
	float fpr2;
	
	DWORD flr1;
} CPU;

typedef struct CPURESULT
{
	int error;
	
	uint32_t address;
	BYTE instruction;
} CPURESULT;

CPU *cpu_init(void);

CPURESULT *cpu_result_create(int error, uint32_t address, BYTE instruction);
CPURESULT *cpu_exec_instruction(CPU *cpu, RAMUNIT *ram);


DWORD get_value_from_register(CPU *cpu, DWORD regid);
bool put_value_in_register(CPU *cpu, DWORD regid, DWORD value);

DWORD get_value(CPU *cpu, RAMUNIT *ram, BYTE loc_id, DWORD location);
bool put_value(CPU *cpu, RAMUNIT *ram, BYTE loc_id, DWORD location, DWORD value);
void print_end_dump(CPU *cpu, CPURESULT *result, RAMUNIT *ram);

bool stack_push(CPU *cpu, RAMUNIT *ram, DWORD value);
DWORD stack_pop(CPU *cpu, RAMUNIT *ram);

#define CPURESULT_SUCCESS 0x00
#define CPURESULT_ILLEGALACCESS 0x01
#define CPURESULT_INSTRUCTIONNOEXIST 0x02
#define CPURESULT_STACKFLOWERROR 0x03


	
	

