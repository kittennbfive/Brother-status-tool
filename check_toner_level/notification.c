/*
This file is part of Brother-status-tool

(c) 2022 by kittennbfive

AGPLv3+ and NO WARRANTY! EXPERIMENTAL STUFF!
*/

#include <stdlib.h>
#include <stdio.h>

#include <libnotify/notify.h>

#include "notification.h"

void show_notification(const event_t ev, char const * const body)
{
	NotifyNotification *notif;
	notify_init("printer_check_toner");
	
	switch(ev)
	{
		case EV_PRINTER_NOT_FOUND:
			notif=notify_notification_new("Printer not found!", "Could not check toner level!", "dialog-error");
			notify_notification_set_timeout(notif, NOTIFY_EXPIRES_NEVER);
			break;
			
		case EV_USB_ERROR:
			notif=notify_notification_new("Communication error with printer!", body, "dialog-error");
			notify_notification_set_timeout(notif, NOTIFY_EXPIRES_NEVER);
			break;
		
		case EV_PARSER_ERROR:
			notif=notify_notification_new("Internal error while parsing printer status data!", body, "dialog-error");
			notify_notification_set_timeout(notif, NOTIFY_EXPIRES_NEVER);
			break;
		
		case EV_LOW_ON_TONER:
			notif=notify_notification_new("Printer is low on toner!", body, "dialog-warning");
			notify_notification_set_timeout(notif, NOTIFY_EXPIRES_NEVER);
			break;
		
		case EV_SHOW_TONER_LEVEL:
			notif=notify_notification_new("Toner levels read from printer are fine!", body, "dialog-information");
			break;
	}
	notify_notification_show(notif, NULL);
}
