#include "disassembler.h"

int dr_disassemble_instruction(FILE *ip, FILE *op)
{
	BYTE instruction = 0;

	emu_error = 0;

	instruction = dr_get_byte(ip);

	if(emu_error != 0)
		return 1;
	
	switch(instruction)
	{
	case 0x00:
	{
		fprintf(op, "hlt");
		break;

	}
	case 0x03:
	{
		fprintf(op, "int ");

		BYTE b = dr_get_byte(ip);

		if(emu_error != 0)
			return 1;
		
		fprintf(op, "0x%x", b);
		break;

	}
	case 0x05:
	{
		fprintf(op, "put ");

		if(dr_output_location_info(ip, op) != 0)
			return 1;
		
		DWORD d = dr_get_dword(ip);

		if(emu_error != 0)
			return 1;
		
		fprintf(op, " 0x%x", d);
		break;
	}
	case 0x06:
	{
		fprintf(op, "cpy ");

		if(dr_output_location_info(ip, op) != 0)
			return 1;
		
		fprintf(op, " ");

		if(dr_output_location_info(ip, op) != 0)
			return 1;
		
		break;
	}
	case 0x07:
	{
		fprintf(op, "add ");
		if(dr_output_location_info(ip, op) != 0)
			return 1;
		
		fprintf(op, " ");

		if(dr_output_location_info(ip, op) != 0)
			return 1;
		break;
	}
	case 0x08:
	{
		fprintf(op, "sub ");
		if(dr_output_location_info(ip, op) != 0)
			return 1;
		
		fprintf(op, " ");

		if(dr_output_location_info(ip, op) != 0)
			return 1;
		break;
	}
	case 0x09:
	{
		fprintf(op, "mul ");
		if(dr_output_location_info(ip, op) != 0)
			return 1;
		
		fprintf(op, " ");

		if(dr_output_location_info(ip, op) != 0)
			return 1;
		break;
	}
	case 0x0A:
	{
		fprintf(op, "div ");
		if(dr_output_location_info(ip, op) != 0)
			return 1;
		
		fprintf(op, " ");

		if(dr_output_location_info(ip, op) != 0)
			return 1;
		break;
	}
	case 0x0B:
	{
		fprintf(op, "push ");
		if(dr_output_location_info(ip, op) != 0)
			return 1;
		break;
	}
	case 0x0C:
	{
		fprintf(op, "pop ");
		if(dr_output_location_info(ip, op) != 0)
			return 1;
		break;
	}
	case 0x0F:
	{
		fprintf(op, "jmp ");
		if(dr_output_location_info(ip, op) != 0)
			return 1;
		break;
	}
	case 0x10:
	{
		fprintf(op, "cmp ");
		if(dr_output_location_info(ip, op) != 0)
			return 1;
		
		fprintf(op, " ");

		if(dr_output_location_info(ip, op) != 0)
			return 1;
		break;
	}
	case 0x11:
	{
		fprintf(op, "ret");
		break;
	}
	case 0x12:
	{
		fprintf(op, "jmpc ");
		if(dr_output_condition(ip, op) != 0)
			return 1;
		fprintf(op, " ");
		if(dr_output_location_info(ip, op) != 0)
			return 1;
		break;
	}
	case 0x13:
	{
		fprintf(op, "jmpr ");
		if(dr_output_location_info(ip, op) != 0)
			return 1;
		
		fprintf(op, " ");

		if(dr_output_location_info(ip, op) != 0)
			return 1;
		break;
	}
	case 0x14:
	{
		fprintf(op, "jmprc ");
		if(dr_output_condition(ip, op) != 0)
			return -1;
		
		fprintf(op, " ");
		if(dr_output_location_info(ip, op) != 0)
			return 1;
		
		fprintf(op, " ");

		if(dr_output_location_info(ip, op) != 0)
			return 1;
		break;
	}
	case 0x15:
	{
		fprintf(op, "iret");
		break;
	}
	case 0x16:
	{
		fprintf(op, "uip ");
		if(dr_output_location_info(ip, op) != 0)
			return 1;
		
		fprintf(op, " ");

		if(dr_output_location_info(ip, op) != 0)
			return 1;
		break;
	}
	case 0xFF:
	{
		fprintf(op, "tst");
		break;
	}
	default:
	{
		return 2;
	}
	}

	return 0;
}

int dr_output_register_name(FILE *op, uint32_t reg)
{
	if(op == NULL)
		return -1;
	
    switch(reg)
    {
                
    case 0x00:
        fprintf(op, "ar1");
        break;
    case 0x01:
        fprintf(op, "ar2");
        break;
    case 0x02:
        fprintf(op, "ar3");
        break;
    case 0x03:
        fprintf(op, "ar4");
        break;
    case 0x04:
        fprintf(op, "ar5");
        break;
    case 0x05:
        fprintf(op, "ip");
        break;
    case 0x06:
        fprintf(op, "bp");
        break;
    case 0x07:
        fprintf(op, "sp");
        break;
    case 0x08:
        fprintf(op, "sb");
        break;
        //FPU not implemented yet
    case 0x0A:
        fprintf(op, "cr1");
        break;
    case 0x0B:
        fprintf(op, "pr1");
        break;
    case 0x0C:
        fprintf(op, "pr2");
        break;
    case 0x0F:
        fprintf(op, "flr1");
        break;
	case 0x10:
        fprintf(op, "nr1");
        break;
	case 0x11:
        fprintf(op, "nr2");
        break;
	case 0x12:
        fprintf(op, "nr3");
        break;
	case 0x13:
        fprintf(op, "nr4");
        break;
	case 0x14:
        fprintf(op, "nr5");
        break;
	case 0x15:
        fprintf(op, "nr6");
        break;
	case 0x16:
        fprintf(op, "nr7");
        break;
	case 0x17:
        fprintf(op, "or1");
        break;
    default:
    {
        return 1;
    }
    }

    return 0;
}

int dr_output_location_info(FILE *ip, FILE *op)
{
    BYTE loc_id;
	DWORD location;

	if(fread(&loc_id, sizeof(BYTE), 1, ip) != 1)
	{
		return 1;
	}

	if(fread(&location, sizeof(DWORD), 1, ip) != 1)
	{
		return 1;
	}

	/* Types o, bo, po, and bpo come after all the others. Let's deal with them first */

	if(loc_id >= 0x0A)
	{
		switch(loc_id)
		{
		case 0x0A:
		{
			fprintf(op, "po ");

			fprintf(op, "0x%x", location);
			break;
		}
		case 0x0B:
		{
			fprintf(op, "o ");

			fprintf(op, "0x%x", location);
			break;
		}
		case 0x0C:
		{
			fprintf(op, "bpo ");

			fprintf(op, "0x%x", location);
			break;
		}
		case 0x0D:
		{
			fprintf(op, "bo ");

			fprintf(op, "0x%x", location);
			break;
		}
		default:
		{
			return 2;
		}
		}

		return 0;
	}

	if(loc_id > 0x04)
	{
		loc_id -= 0x05;
		fprintf(op, "b");
	}

	switch(loc_id)
	{
	case 0x00:
	{
		fprintf(op, "r ");

		if(dr_output_register_name(op, location) != 0)
			return 2;

		break;
	}
	case 0x01:
	{
		fprintf(op, "m ");

		fprintf(op, "0x%x", location);
		break;
	}
	case 0x02:
		fprintf(op, "a ");
		fprintf(op, "0x%x", location);
		break;
	case 0x03:
		fprintf(op, "pr ");

		if(dr_output_register_name(op, location) != 0)
			return 2;

		break;
	case 0x04:
		fprintf(op, "pm ");
		fprintf(op, "0x%x", location);
		break;
	default:
	{
		return 2;
	}
	}

	return 0;
}

int dr_output_condition(FILE *ip, FILE *op)
{
	emu_error = 0;

	BYTE condition = dr_get_byte(ip);

	if(emu_error != 0)
		return 1;

	switch(condition)
	{
	case 0x00:
	{
		fprintf(op, "ne");
		break;
	}
	case 0x01:
	{
		fprintf(op, "eq");
		break;
	}
	case 0x02:
	{
		fprintf(op, "gt");
		break;
	}
	case 0x03:
	{
		fprintf(op, "lt");
		break;
	}
	default:
	{
		return 2;
	}
	}

	return 0;
}



BYTE dr_get_byte(FILE *ip)
{
    BYTE b = 0;

    if(ip == NULL)
    {
        emu_error = EMUERR_NULLERROR;
        return 0;
    }

    if(fread(&b, sizeof(BYTE), 1, ip) != 1)
    {
        emu_error = EMUERR_NULLERROR;
        return 0;
    }

    return b;
}

DWORD dr_get_dword(FILE *ip)
{
    DWORD b = 0;

    if(ip == NULL)
    {
        emu_error = EMUERR_NULLERROR;
        return 0;
    }

    if(fread(&b, sizeof(DWORD), 1, ip) != 1)
    {
        emu_error = EMUERR_NULLERROR;
        return 0;
    }

    return b;
}