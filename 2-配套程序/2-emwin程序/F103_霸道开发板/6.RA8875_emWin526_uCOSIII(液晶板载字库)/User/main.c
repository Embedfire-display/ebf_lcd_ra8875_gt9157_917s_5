/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   5.0寸电容液晶触摸画板实验
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
**/ 


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "includes.h"


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;


/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

__align(8) static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskCreate (void);
static  void  AppObjCreate  (void);
static  void  AppTaskStart  (void *p_arg);


static void cbTimer(OS_TMR *p_tmr, void *p_arg);


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int  main (void)
{
    OS_ERR  err;
    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR ) AppTaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_START_PRIO,
                 (CPU_STK    *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    OS_ERR      err;
	OS_TMR      MyTmr;
   (void)p_arg;	
	
    BSP_Init();                                                 /* Initialize BSP functions                             */
	CPU_Init();

    //Mem_Init();                                                 /* Initialize Memory Management Module                  */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

	OS_DEBUG_FUNC();
    
	OS_INFO("Creating Application Tasks...\n\r");
    AppTaskCreate();                                            /* Create Application Tasks                             */
    
    OS_INFO("Creating Application Events...\n\r");
    AppObjCreate();                                             /* Create Application Objects                           */
    

	OSTmrCreate((OS_TMR              *)&MyTmr,
               (CPU_CHAR            *)"MyTimer",          
               (OS_TICK              )2,                
               (OS_TICK              )2,                
               (OS_OPT               )OS_OPT_TMR_PERIODIC,
               (OS_TMR_CALLBACK_PTR  )cbTimer,          
               (void                *)0,
               (OS_ERR              *)err);
  
	OSTmrStart((OS_TMR *)&MyTmr,(OS_ERR *)err);
	
	
	
	
	//OSTaskDel(&AppTaskStartTCB,&err);	
    while (DEF_TRUE) 
	{                                          /* Task body, always written as an infinite loop.       */
		emWinMainApp();
    }
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION TASKS
*
* Description:  This function creates the application tasks.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION EVENTS
*
* Description:  This function creates the application kernel objects.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppObjCreate (void)
{
}

//按键定时扫描的定时器
static void cbTimer(OS_TMR *p_tmr, void *p_arg)
{
	(void)p_arg;
	GT9xx_GetOnePiont();
}


/* ------------------------------------------end of file---------------------------------------- */

