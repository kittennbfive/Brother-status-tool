/*
This file is part of Brother-status-tool

(c) 2022 by kittennbfive

AGPLv3+ and NO WARRANTY! EXPERIMENTAL STUFF!
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "printercom.h"
#include "parser.h"

int main(void)
{
	char status_data[SZ_STATUS_DATA_MAX];
	unsigned int nb_bytes_received;
	nb_bytes_received=get_status_data_from_printer(status_data, SZ_STATUS_DATA_MAX);
	printf("%u bytes received from printer\n", nb_bytes_received);
	
	parse_data(status_data);
	
	printf("remaining toner: black %.2f%% - cyan %.2f%% - magenta %.2f%% - yellow %.2f%%\n", parser_get_value_float(REMAINING_TONER_BLACK), parser_get_value_float(REMAINING_TONER_CYAN), parser_get_value_float(REMAINING_TONER_MAGENTA), parser_get_value_float(REMAINING_TONER_YELLOW));
	
	printf("remaining drum lifetime: black %.2f%% - cyan %.2f%% - magenta %.2f%% - yellow %.2f%%\n", parser_get_value_float(REMAINING_DRUM_BLACK), parser_get_value_float(REMAINING_DRUM_CYAN), parser_get_value_float(REMAINING_DRUM_MAGENTA), parser_get_value_float(REMAINING_DRUM_YELLOW));
	
	printf("remaining belt lifetime: %.2f%%\n", parser_get_value_float(REMAINING_BELT));
	
	printf("remaining PF Kit1 (whatever that is...) lifetime: %.2f%%\n", parser_get_value_float(REMAINING_PF_KIT1));
	
	printf("remaining fuser lifetime: %.2f%%\n", parser_get_value_float(REMAINING_FUSER));
	
	printf("drum count: black %u - cyan %u - magenta %u - yellow %u\n", parser_get_value_uint(DRUM_COUNT_BLACK), parser_get_value_uint(DRUM_COUNT_CYAN), parser_get_value_uint(DRUM_COUNT_MAGENTA), parser_get_value_uint(DRUM_COUNT_YELLOW));
	
	return 0;
}
