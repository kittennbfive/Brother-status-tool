/*
This file is part of Brother-status-tool

(c) 2022 by kittennbfive

AGPLv3+ and NO WARRANTY! EXPERIMENTAL STUFF!
*/

#ifndef __NOTIFICATION_H__
#define __NOTIFICATION_H__

typedef enum
{
	EV_PRINTER_NOT_FOUND,
	EV_USB_ERROR,
	EV_PARSER_ERROR,
	EV_LOW_ON_TONER,
	EV_SHOW_TONER_LEVEL
} event_t;

void show_notification(const event_t ev, char const * const body);

#endif
