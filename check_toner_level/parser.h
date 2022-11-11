/*
This file is part of Brother-status-tool

(c) 2022 by kittennbfive

AGPLv3+ and NO WARRANTY! EXPERIMENTAL STUFF!
*/

#ifndef __PARSER_H__
#define __PARSER_H__
#include <limits.h>

//#define DEBUG_MODE_VERBOSE //print a lot of stuff

#define PARSER_FLOAT_INVALID -1 //or use macro NAN from math.h (GNU Extension)
#define PARSER_UINT_INVALID UINT_MAX

typedef enum
{
	REMAINING_TONER_BLACK=0,
	REMAINING_TONER_CYAN,
	REMAINING_TONER_MAGENTA,
	REMAINING_TONER_YELLOW,
	
	REMAINING_DRUM_BLACK,
	REMAINING_DRUM_CYAN,
	REMAINING_DRUM_MAGENTA,
	REMAINING_DRUM_YELLOW,
	
	REMAINING_BELT,
	REMAINING_PF_KIT1,
	REMAINING_FUSER,
	
	DRUM_COUNT_BLACK,
	DRUM_COUNT_CYAN,
	DRUM_COUNT_MAGENTA,
	DRUM_COUNT_YELLOW,
	
	AV_VALUES_COUNT	//do not use, for internal use only
} value_names_t;

void parse_data(char * const data); //will modify data!
float parser_get_value_float(const value_names_t value);
unsigned int parser_get_value_uint(const value_names_t value);

#endif
