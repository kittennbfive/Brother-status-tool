/*
This file is part of Brother-status-tool

(c) 2022 by kittennbfive

AGPLv3+ and NO WARRANTY! EXPERIMENTAL STUFF!
*/

#ifndef __PRINTERCOM_H__
#define __PRINTERCOM_H__

#define USB_PID 0x0052 //ADJUST THIS! (use lsusb or similar)

//#define USE_DUMMY_DATA //don't talk to the printer but use fixed dummy data, for easy testing

//#define DEBUG_MODE_VERBOSE //print a lot of stuff

#define TRIES_MAX 10 //maximum number of USB "read-requests" before giving up if no data is received from the printer - might need tweaking depending on your exact printer model and other parameters

unsigned int get_status_data_from_printer(char * const status_data, const unsigned int sz_buffer);

#endif
