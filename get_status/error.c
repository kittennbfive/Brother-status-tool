/*
This file is part of Brother-status-tool

(c) 2022 by kittennbfive

AGPLv3+ and NO WARRANTY! EXPERIMENTAL STUFF!
*/

#include <stdlib.h>
#include <stdio.h>

void error(char const * const func, char const * const msg)
{
	printf("ERROR: %s: %s\n", func, msg);
	exit(1);
}

void warning(char const * const func, char const * const msg)
{
	printf("WARNING: %s: %s\n", func, msg);
}
