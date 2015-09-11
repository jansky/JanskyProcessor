#pragma once
#include "memory.h"
#include "cpu.h"
#include "types.h"
#include "error.h"

typedef struct breakpoint
{
	uint32_t offset;
	uint32_t address;

	struct breakpoint *next;
} breakpoint;

breakpoint *debug_breakpoint_create(uint32_t offset, uint32_t address);
bool debug_breakpoint_add(breakpoint *root, breakpoint *new);
bool debug_breakpoint_del(breakpoint *root, uint32_t id);
breakpoint *debug_breakpoint_get(breakpoint *root, uint32_t id);
uint32_t debug_breakpoint_get_max(breakpoint *root);

bool debug_is_on_breakpoint(breakpoint *root, CPU *cpu);

bool debug_do_interface(breakpoint *root, CPU *cpu, RAMUNIT *ram);
