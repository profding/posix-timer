/******************************************************************************
*
* FILE NAME  :	client-timer.c
* VERSION    :	1.0
* DESCRIPTION:	program for testing posix timer...
*
******************************************************************************/

#include <unistd.h>
#include "client-timer.h"
#include "timeout-handler.h"

/******************************************************************************
* FUNCTION		: clientClientInit()
* DESCRIPTION	: init environment for running
* INPUT			:
* OUTPUT		:
* RETURN		: 0:success; -1:failed
******************************************************************************/
int clientClientInit(void)
{
	int iRet = CLIENT_ERR;

	/* init timer, used to handle timeout event */
	iRet = initClientTimer();
	if (CLIENT_OK != iRet)
	{
		MSG_ERR("init client-client's timer failed!\n");
		(void)clientDeleteAllTimer();
		return CLIENT_ERR;
	}
	
	MSG_LOG("Init client-client environment successful.\n");
	return CLIENT_OK;
}

/******************************************************************************
* FUNCTION		: clientClientExit()
* DESCRIPTION	: clean source befor exit client client
* INPUT			:
* OUTPUT		:
* RETURN		: 0:success; -1:failed
******************************************************************************/
int clientClientExit(void)
{
	int iRet = CLIENT_ERR;

	iRet = clientDeleteAllTimer();
	if (CLIENT_OK != iRet)
	{
		MSG_ERR("fatal error: delete timer failed.\n");
		return CLIENT_ERR;
	}

	return CLIENT_OK;
}

/******************************************************************************
* FUNCTION		: posixTimerTest()
* DESCRIPTION	: test posix timer
* INPUT			:
* OUTPUT		:
* RETURN		:
******************************************************************************/
static void posixTimerTest()
{
	int i = 0;
	OPEAR_ARR operas[OPERA_NUM] = {0};
	
	MSG_LOG("please input %d operations: \n", OPERA_NUM);
	for (i = 0; i < OPERA_NUM; i++)
	{
		printf("No%d:\n", i);
		scanf("%d %d", &(operas[i].opera), &(operas[i].timer));
	}
	
	for (i = 0; i < OPERA_NUM; i++)
	{
		(void)clientTimerSwitch(operas[i].timer, operas[i].opera);
		sleep(SLEEP_TIME);
	}
	
	return ;
}

/******************************************************************************
* FUNCTION		: main()
* DESCRIPTION	: 
* INPUT			:
* OUTPUT		:
* RETURN		:
******************************************************************************/
int main(int argc, char** argv)
{
	int iRet = CLIENT_ERR;

	/* init posix timer */
	iRet = clientClientInit();
	if (CLIENT_OK != iRet)
	{
		MSG_ERR("init client client environment failed!\n");
		return CLIENT_ERR;
	}

	/* test posix timer */
	posixTimerTest();

	/* release timers before exit */
	iRet = clientClientExit();
	if (CLIENT_OK != iRet)
	{
		MSG_ERR("clientClientExit failed!\n");
		return CLIENT_ERR;
	}

	return CLIENT_OK;
}

