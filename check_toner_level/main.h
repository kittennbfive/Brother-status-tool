/*
This file is part of Brother-status-tool

(c) 2022 by kittennbfive

AGPLv3+ and NO WARRANTY! EXPERIMENTAL STUFF!
*/

#ifndef __MAIN_H__
#define __MAIN_H__

//adjust these! (integer please)
#define ALERT_LIMIT_TONER_BL_PERCENT 25
#define ALERT_LIMIT_TONER_CYAN_PERCENT 25
#define ALERT_LIMIT_TONER_MAGENTA_PERCENT 25
#define ALERT_LIMIT_TONER_YELLOW_PERCENT 25

#define ALWAYS_NOTIFY_TONER_LEVEL //show notification with toner level even if there is enough remaining, else only show warning if level(s) below limit(s)

//#define DEBUG_MODE_VERBOSE //print a lot of stuff

#define SZ_STATUS_DATA_MAX 1024 //fine for my printer, might need to be increased for other models

#endif
