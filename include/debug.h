#pragma once
#include "memory.h"
#include "cpu.h"
#include "types.h"
#include "error.h"
#include "gc.h"

#define DEBUG_SIGNAL_NOSIGNAL 0
#define DEBUG_SIGNAL_CONTINUE 1
#define DEBUG_SIGNAL_EXIT 2
#define DEBUG_SIGNAL_ERROR 3

typedef struct breakpoint
{
	uint32_t offset;
	uint32_t address;

	bool enabled;

	struct breakpoint *next;
} breakpoint;

breakpoint *debug_breakpoint_create(uint32_t offset, uint32_t address, bool enabled);
bool debug_breakpoint_add(breakpoint *root, breakpoint *new);
bool debug_breakpoint_del(breakpoint *root, uint32_t id);
breakpoint *debug_breakpoint_get(breakpoint *root, uint32_t id);
uint32_t debug_breakpoint_get_max(breakpoint *root);

bool debug_is_on_breakpoint(breakpoint *root, CPU *cpu);

breakpoint *debug_do_interface(breakpoint *b_root, CPU *cpu, RAMUNIT *ram, FILE *memory_fp);

int debug_do_line(breakpoint **b_root, CPU *cpu, RAMUNIT *ram, char *to_parse, FILE *memory_fp);

void debug_do_regdump(CPU *cpu);
