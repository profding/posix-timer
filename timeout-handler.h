/***********************************************************
*
* File name  : timeout-handler.h
* Version    : 1.0
* Description: posix real timer test
*
************************************************************/

#ifndef _TIMEOUT_HANDLER_H_
#define _TIMEOUT_HANDLER_H_

#include <signal.h>
#include <time.h>

#define MAX_TIMER_NUM	32
#define SIGNUM_RT_TIMER  (SIGRTMIN + 2)

typedef enum
{
	CLIENT_MIN_TIMER_ID,
	HEART_BEAT_TIMER_ID,
	REGISTER_TIMER_ID,
	LOGIN_TIMER_ID,
	BIND_TIMER_ID,
	UNBIND_TIMER_ID,
	CLIENT_MAX_TIMER_ID
} CLIENT_TIMER_ID;

typedef struct
{
	char *name;			/* API name */
	int count;			/* timeout times */
	int expires;		/* general 5s */
	timer_t timer;		/* timer id */
	void (*cb)(void);	/* call back */
} REQUEST_TIMEOUT_OBJECT;

/* initialize request timeout times */
int initReqTimeoutCount(int idx);
/* start or stop timer */
int clientTimerSwitch(int idx, int type);
/* delete timer */
int	clientDeleteTimer(timer_t timer_id);
/* delete all timer */
int clientDeleteAllTimer();
/* init timer */
int initClientTimer();

#endif
