/*
This file is part of Brother-status-tool

(c) 2022 by kittennbfive

AGPLv3+ and NO WARRANTY! EXPERIMENTAL STUFF!
*/

#ifndef __ERROR_H__
#define __ERROR_H__

#define ERR_MSG(msg) error(__FUNCTION__, msg)
#define WARN_MSG(msg) warning(__FUNCTION__, msg)

void error(char const * const func, char const * const msg);
void warning(char const * const func, char const * const msg);

#endif
