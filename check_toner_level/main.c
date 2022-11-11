/*
This file is part of Brother-status-tool

(c) 2022 by kittennbfive

AGPLv3+ and NO WARRANTY! EXPERIMENTAL STUFF!
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "main.h"
#include "error.h"
#include "printercom.h"
#include "parser.h"
#include "notification.h"

int main(void)
{
	char status_data[SZ_STATUS_DATA_MAX];
	unsigned int nb_bytes_received;
	nb_bytes_received=get_status_data_from_printer(status_data, SZ_STATUS_DATA_MAX);
			
#ifdef DEBUG_MODE_VERBOSE
	printf("received %u bytes:\n", nb_bytes_received);
	
	unsigned int i;
	for(i=0; i<nb_bytes_received; i++)
		//printf("%02X ", status_data[i]);
		printf("%c", status_data[i]);
	printf("\n");
#else
	(void)nb_bytes_received; //avoid warning
#endif
	
	parse_data(status_data);
	
	float toner_black=parser_get_value_float(REMAINING_TONER_BLACK);
	float toner_cyan=parser_get_value_float(REMAINING_TONER_CYAN);
	float toner_magenta=parser_get_value_float(REMAINING_TONER_MAGENTA);
	float toner_yellow=parser_get_value_float(REMAINING_TONER_YELLOW);

	char buf[100];
	sprintf(buf, "black %.2f%% - cyan %.2f%% - magenta %.2f%% - yellow %.2f%%", toner_black, toner_cyan, toner_magenta, toner_yellow);

#ifdef DEBUG_MODE_VERBOSE
	printf("remaining toner: %s\n", buf);
	printf("alert limits are: %d%% - %d%% - %d%% - %d%%\n", ALERT_LIMIT_TONER_BL_PERCENT, ALERT_LIMIT_TONER_CYAN_PERCENT, ALERT_LIMIT_TONER_MAGENTA_PERCENT, ALERT_LIMIT_TONER_YELLOW_PERCENT);
#endif

	if(toner_black<ALERT_LIMIT_TONER_BL_PERCENT||toner_cyan<ALERT_LIMIT_TONER_CYAN_PERCENT||toner_magenta<ALERT_LIMIT_TONER_MAGENTA_PERCENT||toner_yellow<ALERT_LIMIT_TONER_YELLOW_PERCENT)
	{
#ifdef DEBUG_MODE_VERBOSE
		printf("LOW ON TONER - showing notification! (%s)\n", buf);
#endif
		show_notification(EV_LOW_ON_TONER, buf);
	}
	else
	{
#ifdef DEBUG_MODE_VERBOSE
		printf("toner levels are fine\n");
#endif

#ifdef ALWAYS_NOTIFY_TONER_LEVEL
		show_notification(EV_SHOW_TONER_LEVEL, buf);
#endif
	}

	return 0;
}
