/***********************************************************
*
* File name  : timer-handler.c
* Version    : 1.0
* Description: timeout handler
*
************************************************************/

#include <string.h>
#include <errno.h>

#include "client-timer.h"
#include "timeout-handler.h"

timer_t timers[MAX_TIMER_NUM] = {0};

static void clientHeartBeatExpired();
static void clientRegisterExpired();
static void clientLoginExpired();
static void clientBindExpired();
static void clientUnbindExpired();

REQUEST_TIMEOUT_OBJECT gReqTimeoutObj[] =
{
	[CLIENT_MIN_TIMER_ID] =
	{
		.name = "min timer",
		.count = 0,
		.expires = 5,
		.cb = NULL,
		.timer = 0
	},

	[HEART_BEAT_TIMER_ID] =
	{
		.name = "heartbeat",
		.count = 0,
		.expires = 3,
		.cb = clientHeartBeatExpired,
		.timer = 0
	},

	[REGISTER_TIMER_ID] =
	{
		.name = "register",
		.count = 0,
		.expires = 5,
		.cb = clientRegisterExpired,
		.timer = 0
	},

	[LOGIN_TIMER_ID] =
	{
		.name = "login",
		.count = 0,
		.expires = 7,
		.cb = clientLoginExpired,
		.timer = 0
	},

	[BIND_TIMER_ID] =
	{
		.name = "bind",
		.count = 0,
		.expires = 2,
		.cb = clientBindExpired,
		.timer = 0
	},

	[UNBIND_TIMER_ID] =
	{
		.name = "unbind",
		.count = 0,
		.expires = 5,
		.cb = clientUnbindExpired,
		.timer = 0
	},

	[CLIENT_MAX_TIMER_ID] =
	{
		.name = "max timer",
		.count = 0,
		.expires = 5,
		.cb = NULL,
		.timer = 0
	},
};

/******************************************************************************
* FUNCTION		: recordTimeoutCount()
* DESCRIPTION	: record timeout times for every API
* INPUT			:
* OUTPUT		:
* RETURN		:
******************************************************************************/
static int recordTimeoutCount(int idx)
{
	gReqTimeoutObj[idx].count++;
	
	MSG_LOG("API: %s, timeout %d times.\n", 
	gReqTimeoutObj[idx].name, gReqTimeoutObj[idx].count);
	
	return CLIENT_OK;
}

/******************************************************************************
* FUNCTION		: initReqTimeoutCount()
* DESCRIPTION	:
* INPUT 		:
* OUTPUT		:
* RETURN		:
******************************************************************************/
int initReqTimeoutCount(int idx)
{
	int i = 0;

	if (idx > CLIENT_MIN_TIMER_ID && idx < CLIENT_MAX_TIMER_ID)
	{
		gReqTimeoutObj[idx].count = 0;
		return 0;
	}

	MSG_LOG("initialize all count 0.\n");
	for (i = CLIENT_MIN_TIMER_ID + 1; i < CLIENT_MAX_TIMER_ID; i++)
	{
		gReqTimeoutObj[i].count = 0;
	}

	return 0;
}

/******************************************************************************
* FUNCTION		: clientHeartBeatExpired()
* DESCRIPTION	:
* INPUT			:
* OUTPUT		:
* RETURN		:
******************************************************************************/
static void clientHeartBeatExpired()
{
	MSG_LOG("heartBeat timeout!\n");
	(void)recordTimeoutCount(HEART_BEAT_TIMER_ID);
	return ;
}

/******************************************************************************
* FUNCTION		: clientRegisterExpired()
* DESCRIPTION	:
* INPUT			:
* OUTPUT		:
* RETURN		:
******************************************************************************/
static void clientRegisterExpired()
{
	MSG_LOG("register timeout!\n");
	(void)recordTimeoutCount(REGISTER_TIMER_ID);
	return ;
}

/******************************************************************************
* FUNCTION		: clientLoginExpired()
* DESCRIPTION	:
* INPUT			:
* OUTPUT		:
* RETURN		:
******************************************************************************/
static void clientLoginExpired()
{
	MSG_LOG("Login timeout!\n");
	(void)recordTimeoutCount(LOGIN_TIMER_ID);
	return ;
}

/******************************************************************************
* FUNCTION		: clientBindExpired()
* DESCRIPTION	:
* INPUT			:
* OUTPUT		:
* RETURN		:
******************************************************************************/
static void clientBindExpired()
{
	MSG_LOG("Bind timeout!\n");
	(void)recordTimeoutCount(BIND_TIMER_ID);
	return ;
}

/******************************************************************************
* FUNCTION		: clientRegisterExpired()
* DESCRIPTION	:
* INPUT			:
* OUTPUT		:
* RETURN		:
******************************************************************************/
static void clientUnbindExpired()
{
	MSG_LOG("Unbind timeout!\n");
	(void)recordTimeoutCount(UNBIND_TIMER_ID);
	return ;
}

/******************************************************************************
* FUNCTION		: clientTimerHandler()
* DESCRIPTION	:
* INPUT			:
* OUTPUT		:
* RETURN		:
******************************************************************************/
static void clientTimerHandler(int signum, siginfo_t *siginfo, void *context)
{
	int sigval = siginfo->si_value.sival_int;

	if (gReqTimeoutObj[sigval].cb)
	{
		gReqTimeoutObj[sigval].cb();
	}
}

/******************************************************************************
* FUNCTION		: clientCreateTimer()
* DESCRIPTION	: create a timer, when time's up, will send a signal "signum" to the handler_func
* INPUT			:
* OUTPUT		:
* RETURN		:
******************************************************************************/
static timer_t clientCreateTimer(int sigval)
{
	static int isHanderSet = 0;
	struct sigevent se;
	struct sigaction sa;
	int index = sigval;

	if (sigval < CLIENT_MIN_TIMER_ID || sigval > CLIENT_MAX_TIMER_ID)
	{
		MSG_ERR("tpCreateTimer: signum is too small\n");
		return 0;
	}

	/* signal already registered? */
	if (timers[index])
	{
		MSG_ERR("tpCreateTimer: signal %d has been used\n", sigval);
		return 0;
	}

	if (isHanderSet == 0)
	{
		memset(&sa, 0, sizeof(sa));
		sa.sa_sigaction = clientTimerHandler;
		sa.sa_flags = SA_SIGINFO;
		sa.sa_restorer = NULL;
		sigemptyset(&sa.sa_mask);

		if (sigaction(SIGNUM_RT_TIMER, &sa, NULL) < 0)
		{
			MSG_ERR("register timer signal cb failed. errno = %d.\n", errno);
			return 0;
		}

		isHanderSet = 1;
	}

	memset (&se, 0, sizeof (se));
	se.sigev_notify = SIGEV_SIGNAL;
	se.sigev_signo = SIGNUM_RT_TIMER;
	se.sigev_value.sival_int = sigval;
	se.sigev_notify_function = NULL;

	if (timer_create(CLOCK_MONOTONIC, &se, &timers[index]) < 0)
	{
		timers[index] = 0;
		MSG_ERR("timer_creat failed\n");
		return 0;
	}

	return timers[index];
}

/******************************************************************************
* FUNCTION		: initClientTimer()
* DESCRIPTION	:
* INPUT			:
* OUTPUT		:
* RETURN		:
******************************************************************************/
int initClientTimer()
{
	int ret = CLIENT_ERR;
	int idx = 0;

	for (idx = CLIENT_MIN_TIMER_ID + 1; idx < CLIENT_MAX_TIMER_ID; idx++)
	{
		if (NULL == gReqTimeoutObj[idx].cb)
		{
			continue;
		}

		gReqTimeoutObj[idx].timer = clientCreateTimer(idx);
		if (0 >= gReqTimeoutObj[idx].timer)
		{
			MSG_ERR("create gReqTimeoutObj[%d] timer failed!\n", idx);
			return CLIENT_ERR;
		}
	}

	return CLIENT_OK;
}

/******************************************************************************
* FUNCTION		: clientSetTimer()
* DESCRIPTION	: let the timer run
* INPUT			:
* OUTPUT		:
* RETURN		:
******************************************************************************/
static int clientSetTimer(timer_t timer_id, int firstRun, int interval)
{
	struct itimerspec ts, ots;

	ts.it_value.tv_sec = firstRun;
	ts.it_value.tv_nsec =  0;
	ts.it_interval.tv_sec = interval;
	ts.it_interval.tv_nsec = 0;

	if (timer_settime(timer_id, 0, &ts, &ots) < 0)
	{
		MSG_ERR("setTimer failed!\n");
		return (-1);
	}

	return (0);
}

/******************************************************************************
* FUNCTION		: clientTimerSwitch()
* DESCRIPTION	: start or stop timer
* INPUT			:
* OUTPUT		:
* RETURN		:
******************************************************************************/
int clientTimerSwitch(int idx, int type)
{
	if (idx <= CLIENT_MIN_TIMER_ID
		|| idx >= CLIENT_MAX_TIMER_ID)
	{
		MSG_ERR("idx of timer invalid.\n");
		return CLIENT_ERR;
	}

	/* stop timer */
	if (0 == type)
	{
		MSG_LOG("stop timer: %s.\n", gReqTimeoutObj[idx].name);
		return clientSetTimer(gReqTimeoutObj[idx].timer, 0, 0);
	}
	/* start timer */
	else
	{
		MSG_LOG("start timer: %s.\n", gReqTimeoutObj[idx].name);
		return clientSetTimer(gReqTimeoutObj[idx].timer, gReqTimeoutObj[idx].expires, 0);
	}
}

/******************************************************************************
* FUNCTION		: clientDeleteTimer()
* DESCRIPTION	: delete a time which has been created.
* INPUT			:
* OUTPUT		:
* RETURN		:
******************************************************************************/
int clientDeleteTimer(timer_t timer_id)
{
	int i;

	for (i = 0; i < MAX_TIMER_NUM; i ++)
	{
		if (timers[i] == timer_id)
		{
			timers[i] = 0;
			return timer_delete(timer_id);
		}
	}

	MSG_LOG("delete timer id %d failed: not found\n", (int)timer_id);
	return -1;
}

/******************************************************************************
* FUNCTION		: clientDeleteAllTimer()
* DESCRIPTION	: delete all timer.
* INPUT			:
* OUTPUT		:
* RETURN		:
******************************************************************************/
int clientDeleteAllTimer()
{
	int i = 0;
	int ret = -1;

	for (i = 0; i < MAX_TIMER_NUM; i ++)
	{
		if (0 < timers[i])
		{
			ret = timer_delete(timers[i]);
			if (0 != ret)
			{
				MSG_ERR("delete timer %d failed\n", (int)timers[i]);
				return CLIENT_ERR;
			}

			timers[i] = 0;
		}
	}

	MSG_LOG("delete timer successed!\n");
	return CLIENT_OK;
}
