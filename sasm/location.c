#include "sasm.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

uint32_t get_register_id_from_string(char *reg)
{
    if(reg == NULL)
        return 0x1111;
        
    if(strcmp(reg, "ar1")) return 0x00;
    if(strcmp(reg, "ar2")) return 0x01;
    if(strcmp(reg, "ar3")) return 0x02;
    if(strcmp(reg, "ar4")) return 0x03;
    if(strcmp(reg, "ar5")) return 0x04;
    
    if(strcmp(reg, "ip")) return 0x05;
    if(strcmp(reg, "bp")) return 0x06;
    
    if(strcmp(reg, "sp")) return 0x07;
    if(strcmp(reg, "sb")) return 0x08;
    
    if(strcmp(reg, "cr1")) return 0x0A;
    
    if(strcmp(reg, "pr1")) return 0x0B;
    if(strcmp(reg, "pr2")) return 0x0C;
    
    if(strcmp(reg, "flr1")) return 0x0F;
    
    if(strcmp(reg, "nr1")) return 0x10;
    if(strcmp(reg, "nr2")) return 0x11;
    if(strcmp(reg, "nr3")) return 0x12;
    if(strcmp(reg, "nr4")) return 0x13;
    if(strcmp(reg, "nr5")) return 0x14;
    if(strcmp(reg, "nr6")) return 0x15;
    if(strcmp(reg, "nr7")) return 0x16;
    
    if(strcmp(reg, "or1")) return 0x17;
    
    return 0x2222; // Invalid register name
}

int sasm_write_get_location(FILE *fp, char *loctype, char *locarg, SASMLocationToFill *root)
{
    // Perform some checks to ensure everything is okay
    
    bool using_label = false;
    char *loctype_to_parse;
    
    if(fp == NULL)
        return SASM_ERROR_IOERROR;
        
    if(loctype == NULL || locarg == NULL)
        return SASM_ERROR_STRINGERROR;
        
    if(root == NULL)
        return SASM_ERROR_LABELERROR;
        
    // "switch(){}" the location type
    
    // support loctype 'l' for 'la' for backwards compatibility
    if(strcmp(loctype, "l") == 0)
    {
        if(sasm_write_byte(0x02) != 1) return SASM_ERROR_IOERROR;
        
        // Add entry to location to fill list
        long location = ftell(fp); // Get position of where to fill in location
        
        if(location == -1)
            return SASM_ERROR_IOERROR;
            
        SASMLocationToFill *new_ltf = sasm_location_to_fill_create(locarg, (uint32_t)location);
        
        if(new_ltf == NULL)
            return SASM_ERROR_LABELERROR;
            
        if(!sasm_location_to_fill_add(root, new_ltf)) return SASM_ERROR_LABELERROR; // Add new location to fill to list
        
        // Fill in the location dword with zeros, it will be filled in later
        if(sasm_write_dword(0x00000000) != 1) return SASM_ERROR_IOERROR;
        
        return SASM_ERROR_NOERROR; // Operation was a success
    }
    
    //Look for loctypes that are incompatible with labels
    if(strcmp(loctype, "r") == 0)
    {
        if(sasm_write_byte(0x00) != 1) return SASM_ERROR_IOERROR;
        
        uint32_t reg = get_register_id_from_string(locarg);
        
        if(reg == 0x1111) // locarg was NULL for some reason
            return SASM_ERROR_STRINGERROR;
        
        if(reg == 0x2222) // invalid register type
            return SASM_ERROR_REGISTERINVALID;
            
        if(sasm_write_dword(reg) != 1) return SASM_ERROR_IOERROR; // Write register ID
        
        return SASM_ERROR_NOERROR; // Operation was a success
    }
    
    if(strcmp(loctype, "pr") == 0)
    {
        if(sasm_write_byte(0x03) != 1) return SASM_ERROR_IOERROR;
        
        uint32_t reg = get_register_id_from_string(locarg);
        
        if(reg == 0x1111) // locarg was NULL for some reason
            return SASM_ERROR_STRINGERROR;
        
        if(reg == 0x2222) // invalid register type
            return SASM_ERROR_REGISTERINVALID;
            
        if(sasm_write_dword(reg) != 1) return SASM_ERROR_IOERROR; // Write register ID
        
        return SASM_ERROR_NOERROR; // Operation was a success
    }
    
    if(strcmp(loctype, "br") == 0)
    {
        if(sasm_write_byte(0x05) != 1) return SASM_ERROR_IOERROR;
        
        uint32_t reg = get_register_id_from_string(locarg);
        
        if(reg == 0x1111) // locarg was NULL for some reason
            return SASM_ERROR_STRINGERROR;
        
        if(reg == 0x2222) // invalid register type
            return SASM_ERROR_REGISTERINVALID;
            
        if(sasm_write_dword(reg) != 1) return SASM_ERROR_IOERROR; // Write register ID
        
        return SASM_ERROR_NOERROR; // Operation was a success
    }
    
    if(strcmp(loctype, "bpr") == 0)
    {
        if(sasm_write_byte(0x08) != 1) return SASM_ERROR_IOERROR;
        
        uint32_t reg = get_register_id_from_string(locarg);
        
        if(reg == 0x1111) // locarg was NULL for some reason
            return SASM_ERROR_STRINGERROR;
        
        if(reg == 0x2222) // invalid register type
            return SASM_ERROR_REGISTERINVALID;
            
        if(sasm_write_dword(reg) != 1) return SASM_ERROR_IOERROR; // Write register ID
        
        return SASM_ERROR_NOERROR; // Operation was a success
    }
    
    // Now see if we're using a label
    if(loctype[0] == 'l')
    {
        // Ensure that there are some more characters to parse
        if(strlen(loctype) < 2)
            return SASM_ERROR_SYNTAXINVALID;
        
        using_label = true;
        
        loctype_to_parse = (char*)(loctype + 1);
    }
    else
    {
        loctype_to_parse = loctype;
    }
    
    // Parse the rest of the location types
    // They all take either number arguments or labels
    if(strcmp(loctype_to_parse, "m") == 0)
    {
        if(sasm_write_byte(0x01) != 1) return SASM_ERROR_IOERROR;
    }
    else if(strcmp(loctype_to_parse, "a") == 0)
    {
        if(sasm_write_byte(0x02) != 1) return SASM_ERROR_IOERROR;
    }
    else if(strcmp(loctype_to_parse, "pm") == 0)
    {
        if(sasm_write_byte(0x04) != 1) return SASM_ERROR_IOERROR;
    }
    else if(strcmp(loctype_to_parse, "bm") == 0)
    {
        if(sasm_write_byte(0x06) != 1) return SASM_ERROR_IOERROR;
    }
    else if(strcmp(loctype_to_parse, "ba") == 0)
    {
        if(sasm_write_byte(0x07) != 1) return SASM_ERROR_IOERROR;
    }
    else if(strcmp(loctype_to_parse, "bpm") == 0)
    {
        if(sasm_write_byte(0x09) != 1) return SASM_ERROR_IOERROR;
    }
    else if(strcmp(loctype_to_parse, "po") == 0)
    {
        if(sasm_write_byte(0x0A) != 1) return SASM_ERROR_IOERROR;
    }
    else if(strcmp(loctype_to_parse, "o") == 0)
    {
        if(sasm_write_byte(0x0B) != 1) return SASM_ERROR_IOERROR;
    }
    else if(strcmp(loctype_to_parse, "bpo") == 0)
    {
        if(sasm_write_byte(0x0C) != 1) return SASM_ERROR_IOERROR;
    }
    else if(strcmp(loctype_to_parse, "bo") == 0)
    {
        if(sasm_write_byte(0x0D) != 1) return SASM_ERROR_IOERROR;
    }
    else
    {
        //Invalid loctype
        return SASM_ERROR_SYNTAXINVALID;
    }
    
    // Write the locarg
    if(using_label)
    {
        // Add entry to location to fill list
        long location = ftell(fp); // Get position of where to fill in location
        
        if(location == -1)
            return SASM_ERROR_IOERROR;
            
        SASMLocationToFill *new_ltf = sasm_location_to_fill_create(locarg, (uint32_t)location);
        
        if(new_ltf == NULL)
            return SASM_ERROR_LABELERROR;
            
        if(!sasm_location_to_fill_add(root, new_ltf)) return SASM_ERROR_LABELERROR; // Add new location to fill to list
        
        // Fill in the location dword with zeros, it will be filled in later
        if(sasm_write_dword(0x00000000) != 1) return SASM_ERROR_IOERROR;
        
        return SASM_ERROR_NOERROR; // Operation was a success
     }
     else
     {
        uint32_t number = sasm_parse_number(locarg);
        
        if(sasm_error != 0)
            return SASM_ERROR_NUMBERERROR;
        
        if(sasm_write_dword(number) != 1) return SASM_ERROR_IOERROR;
        
        return SASM_ERROR_NOERROR; // Operation was a success
     }
     
     return SASM_ERROR_UNKNOWNERROR; // We shouldn't end up here
}

int sasm_write_put_location(FILE *fp, char *loctype, char *locarg, SASMLocationToFill *root)
{
    // Perform some checks to ensure everything is okay
    
    bool using_label = false;
    char *loctype_to_parse;
    
    if(fp == NULL)
        return SASM_ERROR_IOERROR;
        
    if(loctype == NULL || locarg == NULL)
        return SASM_ERROR_STRINGERROR;
        
    if(root == NULL)
        return SASM_ERROR_LABELERROR;
        
    // "switch(){}" the location type
    
    //Look for loctypes that are incompatible with labels
    if(strcmp(loctype, "r") == 0)
    {
        if(sasm_write_byte(0x00) != 1) return SASM_ERROR_IOERROR;
        
        uint32_t reg = get_register_id_from_string(locarg);
        
        if(reg == 0x1111) // locarg was NULL for some reason
            return SASM_ERROR_STRINGERROR;
        
        if(reg == 0x2222) // invalid register type
            return SASM_ERROR_REGISTERINVALID;
            
        if(sasm_write_dword(reg) != 1) return SASM_ERROR_IOERROR; // Write register ID
        
        return SASM_ERROR_NOERROR; // Operation was a success
    }
    
    if(strcmp(loctype, "pr") == 0)
    {
        if(sasm_write_byte(0x03) != 1) return SASM_ERROR_IOERROR;
        
        uint32_t reg = get_register_id_from_string(locarg);
        
        if(reg == 0x1111) // locarg was NULL for some reason
            return SASM_ERROR_STRINGERROR;
        
        if(reg == 0x2222) // invalid register type
            return SASM_ERROR_REGISTERINVALID;
            
        if(sasm_write_dword(reg) != 1) return SASM_ERROR_IOERROR; // Write register ID
        
        return SASM_ERROR_NOERROR; // Operation was a success
    }
    
    if(strcmp(loctype, "br") == 0)
    {
        if(sasm_write_byte(0x05) != 1) return SASM_ERROR_IOERROR;
        
        uint32_t reg = get_register_id_from_string(locarg);
        
        if(reg == 0x1111) // locarg was NULL for some reason
            return SASM_ERROR_STRINGERROR;
        
        if(reg == 0x2222) // invalid register type
            return SASM_ERROR_REGISTERINVALID;
            
        if(sasm_write_dword(reg) != 1) return SASM_ERROR_IOERROR; // Write register ID
        
        return SASM_ERROR_NOERROR; // Operation was a success
    }
    
    if(strcmp(loctype, "bpr") == 0)
    {
        if(sasm_write_byte(0x08) != 1) return SASM_ERROR_IOERROR;
        
        uint32_t reg = get_register_id_from_string(locarg);
        
        if(reg == 0x1111) // locarg was NULL for some reason
            return SASM_ERROR_STRINGERROR;
        
        if(reg == 0x2222) // invalid register type
            return SASM_ERROR_REGISTERINVALID;
            
        if(sasm_write_dword(reg) != 1) return SASM_ERROR_IOERROR; // Write register ID
        
        return SASM_ERROR_NOERROR; // Operation was a success
    }
    
    // Now see if we're using a label
    if(loctype[0] == 'l')
    {
        // Ensure that there are some more characters to parse
        if(strlen(loctype) < 2)
            return SASM_ERROR_SYNTAXINVALID;
        
        using_label = true;
        
        loctype_to_parse = (char*)(loctype + 1);
    }
    else
    {
        loctype_to_parse = loctype;
    }
    
    // Parse the rest of the location types
    // They all take either number arguments or labels
    if(strcmp(loctype_to_parse, "m") == 0)
    {
        if(sasm_write_byte(0x01) != 1) return SASM_ERROR_IOERROR;
    }
    else if(strcmp(loctype_to_parse, "pm") == 0)
    {
        if(sasm_write_byte(0x04) != 1) return SASM_ERROR_IOERROR;
    }
    else if(strcmp(loctype_to_parse, "bm") == 0)
    {
        if(sasm_write_byte(0x06) != 1) return SASM_ERROR_IOERROR;
    }
    else if(strcmp(loctype_to_parse, "bpm") == 0)
    {
        if(sasm_write_byte(0x09) != 1) return SASM_ERROR_IOERROR;
    }
    else if(strcmp(loctype_to_parse, "po") == 0)
    {
        if(sasm_write_byte(0x0A) != 1) return SASM_ERROR_IOERROR;
    }
    else if(strcmp(loctype_to_parse, "bpo") == 0)
    {
        if(sasm_write_byte(0x0C) != 1) return SASM_ERROR_IOERROR;
    }
    else
    {
        //Invalid loctype
        return SASM_ERROR_SYNTAXINVALID;
    }
    
    // Write the locarg
    if(using_label)
    {
        // Add entry to location to fill list
        long location = ftell(fp); // Get position of where to fill in location
        
        if(location == -1)
            return SASM_ERROR_IOERROR;
            
        SASMLocationToFill *new_ltf = sasm_location_to_fill_create(locarg, (uint32_t)location);
        
        if(new_ltf == NULL)
            return SASM_ERROR_LABELERROR;
            
        if(!sasm_location_to_fill_add(root, new_ltf)) return SASM_ERROR_LABELERROR; // Add new location to fill to list
        
        // Fill in the location dword with zeros, it will be filled in later
        if(sasm_write_dword(0x00000000) != 1) return SASM_ERROR_IOERROR;
        
        return SASM_ERROR_NOERROR; // Operation was a success
     }
     else
     {
        uint32_t number = sasm_parse_number(locarg);
        
        if(sasm_error != 0)
            return SASM_ERROR_NUMBERERROR;
        
        if(sasm_write_dword(number) != 1) return SASM_ERROR_IOERROR;
        
        return SASM_ERROR_NOERROR; // Operation was a success
     }
     
     return SASM_ERROR_UNKNOWNERROR; // We shouldn't end up here
}
