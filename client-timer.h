/******************************************************************************
*
* FILE NAME  :	client-timer.h
* VERSION    :	1.0
* DESCRIPTION:	program for testing posix timer.
*
******************************************************************************/

#ifndef _CLIENT_TIMER_H_
#define _CLIENT_TIMER_H_

#include <stdio.h>

typedef struct
{
	int opera;	/* opera type */
	int timer;	/* timer index */
} OPEAR_ARR;

#define OPERA_NUM 5
#define SLEEP_TIME (OPERA_NUM + 2)

#define CLIENT_OK		0
#define CLIENT_ERR	-1

#define MSG_LOG(...) do { \
	printf("Function:%s, Line:%d.", __FUNCTION__, __LINE__); \
	printf(__VA_ARGS__); \
	printf("\n"); \
} while(0)

#define MSG_ERR(...) do { \
	printf("Function:%s, Line:%d.", __FUNCTION__, __LINE__); \
	printf(__VA_ARGS__); \
	printf("\n"); \
} while(0)

#endif

