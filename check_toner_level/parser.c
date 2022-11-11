/*
This file is part of Brother-status-tool

(c) 2022 by kittennbfive

AGPLv3+ and NO WARRANTY! EXPERIMENTAL STUFF!
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "error.h"
#include "notification.h"

#define NB_UNKNOWN_BYTES_MAX 4
#define NB_BYTES_MAX 10
#define SZ_LINE_MAX 50

typedef enum
{
	VT_PERCENT_2BYTES_2DECIMALS,
	VT_VALUE_2BYTES,
	VT_END_OF_LIST
} valuetype_t;

typedef struct
{
	unsigned int index;
	unsigned int nb_unknown_bytes;
	unsigned char unknown_bytes[NB_UNKNOWN_BYTES_MAX];
	valuetype_t value_type;
	char * description;
	value_names_t value_index;
} data_structure_t;

typedef enum
{
	TYPE_FLOAT,
	TYPE_UINT
} value_type_t;

typedef struct
{
	value_type_t value_type;
	bool valid_data;
	union
	{
		float value_float;
		unsigned int value_uint;
	};
} value_t;

//NO WARRANTY! CONSIDER WITH GREAT SCEPTICISM!
static const data_structure_t data_structure[]={
	{0x69, 4, {01, 04, 00, 00}, VT_PERCENT_2BYTES_2DECIMALS, "remaining Belt Life %", REMAINING_BELT},
	{0x6A, 4, {01, 04, 00, 00}, VT_PERCENT_2BYTES_2DECIMALS, "remaining PF Kit 1 Life %", REMAINING_PF_KIT1},
	{0x6D, 4, {01, 04, 00, 00}, VT_PERCENT_2BYTES_2DECIMALS, "remaining Fuser Life %", REMAINING_FUSER},
	{0x6F, 4, {01, 04, 00, 00,}, VT_PERCENT_2BYTES_2DECIMALS, "remaining Toner Black %", REMAINING_TONER_BLACK},
	{0x70, 4, {01, 04, 00, 00,}, VT_PERCENT_2BYTES_2DECIMALS, "remaining Toner Cyan %", REMAINING_TONER_CYAN},
	{0x71, 4, {01, 04, 00, 00,}, VT_PERCENT_2BYTES_2DECIMALS, "remaining Toner Magenta %", REMAINING_TONER_MAGENTA},
	{0x72, 4, {01, 04, 00, 00,}, VT_PERCENT_2BYTES_2DECIMALS, "remaining Toner Yellow %", REMAINING_TONER_YELLOW},
	{0x73, 4, {01, 04, 00, 00,}, VT_VALUE_2BYTES, "Drum Count Cyan", DRUM_COUNT_CYAN},
	{0x74, 4, {01, 04, 00, 00,}, VT_VALUE_2BYTES, "Drum Count Magenta", DRUM_COUNT_MAGENTA},
	{0x75, 4, {01, 04, 00, 00,}, VT_VALUE_2BYTES, "Drum Count Yellow", DRUM_COUNT_YELLOW},
	{0x7E, 4, {01, 04, 00, 00,}, VT_VALUE_2BYTES, "Drum Count Black", DRUM_COUNT_BLACK},
	{0x79, 4, {01, 04, 00, 00,}, VT_PERCENT_2BYTES_2DECIMALS, "remaining Drum Life Cyan %", REMAINING_DRUM_CYAN},
	{0x7A, 4, {01, 04, 00, 00,}, VT_PERCENT_2BYTES_2DECIMALS, "remaining Drum Life Magenta %", REMAINING_DRUM_MAGENTA},
	{0x7B, 4, {01, 04, 00, 00,}, VT_PERCENT_2BYTES_2DECIMALS, "remaining Drum Life Yellow %", REMAINING_DRUM_YELLOW},
	{0x80, 4, {01, 04, 00, 00,}, VT_PERCENT_2BYTES_2DECIMALS, "remaining Drum Life Black %", REMAINING_DRUM_BLACK},
	{0, 0, {}, VT_END_OF_LIST, NULL, 0}
};

static value_t * values=NULL;

static unsigned char parse_hex_byte(char const * const str)
{
	unsigned char ret=0;
	
	unsigned int i;
	for(i=0; i<2; i++)
	{
		ret<<=4;
		if(isdigit(str[i]))
			ret|=(str[i]-'0');
		else if(toupper(str[i])>='A' && toupper(str[i])<='F')
			ret|=(toupper(str[i])-'A'+10);
		else
		{
			show_notification(EV_PARSER_ERROR, "invalid hex digit");
			ERR_MSG("invalid hex digit");
		}
	}
	
	return ret;
}

static unsigned int parse_line(char const * const data, unsigned char * const bytes)
{
	//we can't use strtok() here as it is already used in parse_data() that calls this function!

#ifdef DEBUG_MODE_VERBOSE
	printf("parse: %s\n", data);
#endif

	char line[SZ_LINE_MAX];
	strncpy(line, data, SZ_LINE_MAX);
	
	char *ptr=line;
	
	if(memcmp(ptr, "DATA=\"", strlen("DATA=\"")) && (*ptr)!='\x0c') //ignore "end of message" marker == 0x0c
	{
		show_notification(EV_PARSER_ERROR, "invalid start of line");
		ERR_MSG("invalid start of line");
	}
	
	ptr+=strlen("DATA=\"");
	
	unsigned int index_bytes=0;
	char value_str[2];
	unsigned int index_value_str=0;

	while(*ptr)
	{
		if((*ptr)==',' || (*ptr)=='\"')
		{
			if(index_bytes<NB_BYTES_MAX)
				bytes[index_bytes++]=parse_hex_byte(value_str);
			else
			{
				show_notification(EV_PARSER_ERROR, "overflow bytes");
				ERR_MSG("overflow bytes");
			}
			index_value_str=0;
		}
		else if(index_value_str<2)
			value_str[index_value_str++]=(*ptr);
		else
		{
			show_notification(EV_PARSER_ERROR, "overflow value_str");
			ERR_MSG("overflow value_str");
		}
		ptr++;
	}

#ifdef DEBUG_MODE_VERBOSE
	printf("parsed %u bytes\n", index_bytes);
#endif
	
	return index_bytes;
}

static void interpret_bytes(unsigned char const * const bytes, const unsigned int nb_bytes)
{
	unsigned int i,j;
	bool found;
	
	for(i=0; data_structure[i].value_type!=VT_END_OF_LIST; i++)
	{
		found=false;
		if(nb_bytes && bytes[0]==data_structure[i].index)
		{	
			found=true;
			break;
		}
	}
	
	if(!found)
	{
#ifdef DEBUG_MODE_VERBOSE
		printf("nothing found for these bytes\n");
#endif
		return;
	}

#ifdef DEBUG_MODE_VERBOSE
	printf("found index 0x%02X: %s\n", bytes[0], data_structure[i].description);
#endif
	
	if(nb_bytes>=(1+data_structure[i].nb_unknown_bytes))
	{
		for(j=0; j<data_structure[i].nb_unknown_bytes; j++)
		{
			if(bytes[1+j]!=data_structure[i].unknown_bytes[j])
			{
#ifdef DEBUG_MODE_VERBOSE
				printf("missmatch unknown bytes\n");
#endif
				return;
			}
		}
	}
	else
	{
#ifdef DEBUG_MODE_VERBOSE
		printf("not enough unknown bytes\n");
#endif
		return;
	}

#ifdef DEBUG_MODE_VERBOSE
	printf("unknown bytes matching\n");
#endif

	switch(data_structure[i].value_type)
	{
		case VT_PERCENT_2BYTES_2DECIMALS:
			if(nb_bytes!=(1+data_structure[i].nb_unknown_bytes+2))
			{
#ifdef DEBUG_MODE_VERBOSE
				printf("not enough or too many bytes for value\n");
#endif
				return;
			}
			{
				unsigned short val=((unsigned short)bytes[5]<<8)|bytes[6];
#ifdef DEBUG_MODE_VERBOSE
				printf("%s: %u,%02u%%\n", data_structure[i].description, val/100, val%100);
#endif
				values[data_structure[i].value_index].value_float=(float)val/100;
				values[data_structure[i].value_index].value_type=TYPE_FLOAT;
				values[data_structure[i].value_index].valid_data=true;
			}
			break;
		
		case VT_VALUE_2BYTES:
			if(nb_bytes!=(1+data_structure[i].nb_unknown_bytes+2))
			{
#ifdef DEBUG_MODE_VERBOSE
				printf("not enough or too many bytes for value\n");
#endif
				return;
			}
			{
				unsigned short val=((unsigned short)bytes[5]<<8)|bytes[6];
#ifdef DEBUG_MODE_VERBOSE
				printf("%s: %u\n", data_structure[i].description, val);
#endif
				values[data_structure[i].value_index].value_uint=val;
				values[data_structure[i].value_index].value_type=TYPE_UINT;
				values[data_structure[i].value_index].valid_data=true;
			}
			break;
		
		case VT_END_OF_LIST: break; //to avoid compiler warning
	}
}

static void cleanup(void)
{
	free(values);
}

void parse_data(char * const data)
{
	values=malloc(AV_VALUES_COUNT*sizeof(value_t));
	if(!values)
	{
		show_notification(EV_PARSER_ERROR, "malloc for values failed");
		ERR_MSG("malloc for values failed");
	}
	
	atexit(&cleanup);
	
	unsigned int i;
	for(i=0; i<AV_VALUES_COUNT; i++)
		values[i].valid_data=false;
	
	char *ptr=strtok(data, "\r\n");
	if(!ptr)
	{
		show_notification(EV_PARSER_ERROR, "invalid data (no newline found)");
		ERR_MSG("invalid data (no newline found)");
	}
	
	if(strcmp(ptr, "@PJL INFO BRMAINTENANCE"))
	{
		show_notification(EV_PARSER_ERROR, "invalid first line");
		ERR_MSG("invalid first line, this is not the data we are looking for");
	}
	
	unsigned char bytes[NB_BYTES_MAX];
	unsigned int nb_bytes_parsed;
	
	while((ptr=strtok(NULL, "\r\n")))
	{
		nb_bytes_parsed=parse_line(ptr, bytes);		
		interpret_bytes(bytes, nb_bytes_parsed);
	}
}

float parser_get_value_float(const value_names_t value)
{
#ifdef DEBUG_MODE_VERBOSE
	printf("requested float value %u\n", (unsigned int)value);
#endif

	if(values[value].value_type!=TYPE_FLOAT)
	{
		WARN_MSG("this value is not of type float, returning PARSER_FLOAT_INVALID");
		return PARSER_FLOAT_INVALID;
	}

	if(values[value].valid_data)
		return values[value].value_float;
	else
	{
		WARN_MSG("no data for this value, returning PARSER_FLOAT_INVALID");
		return PARSER_FLOAT_INVALID;
	}
}

unsigned int parser_get_value_uint(const value_names_t value)
{
#ifdef DEBUG_MODE_VERBOSE
	printf("requested uint value %u\n", (unsigned int)value);
#endif

	if(values[value].value_type!=TYPE_UINT)
	{
		WARN_MSG("this value is not of type uint, returning PARSER_UINT_INVALID");
		return PARSER_UINT_INVALID;
	}

	if(values[value].valid_data)
		return values[value].value_uint;
	else
	{
		WARN_MSG("no data for this value, returning PARSER_UINT_INVALID");
		return PARSER_UINT_INVALID;
	}
}
