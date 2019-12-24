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

static  OS_TCB		AppTaskStartTCB;
static	OS_TCB		AppTaskScreenShotTCB;
static	OS_TCB		AppTaskGuiKeyPadTCB;
static	OS_TCB		AppTaskGuiTCB;


/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

__align(8)	static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];
static  CPU_STK  AppTaskScreenShotStk[APP_TASK_SCREENSHOT_STK_SIZE];
static  CPU_STK  AppTaskGuiKeyPadStk[APP_TASK_GUIKEYPAD_STK_SIZE];
static  CPU_STK  AppTaskGuiStk[APP_TASK_GUI_STK_SIZE];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskCreate (void);
static  void  AppObjCreate  (void);
static  void  AppTaskStart  (void *p_arg);
static	void  AppTaskScreenShot(void *p_arg);
static	void  AppTaskGuiKeyPad(void *p_arg);
static	void  AppTaskGui(void *p_arg);
static void cbTimer(OS_TMR *p_tmr, void *p_arg);

static  OS_SEM     	SEM_KEY2;
KEY Key2;	
static 	uint8_t		Pic_Name = 0;
__IO uint8_t key_flag=1;//1：允许按键截图，0：禁用按键截图
/*
************************************************************************
*						  FatFs
************************************************************************
*/
FRESULT result;
FIL file;
UINT bw;

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
	
	OSTaskDel(&AppTaskStartTCB,&err);	
//    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
//		OSTimeDlyHMSM(0, 0, 5, 0,
//                      OS_OPT_TIME_HMSM_STRICT,
//                      &err);
//    }
}


//按键定时扫描的定时器
static void cbTimer(OS_TMR *p_tmr, void *p_arg)
{
	OS_ERR      err;
	(void)p_arg;
	GT9xx_GetOnePiont();//触摸屏定时扫描

	/*按键定时扫描*/
	Key_RefreshState(&Key2);//刷新按键状态
	if(Key_AccessTimes(&Key2,KEY_ACCESS_READ)!=0 && key_flag)//按键被按下过
	{
		LED3_TOGGLE;
		OSSemPost ((OS_SEM  *)&SEM_KEY2,
               (OS_OPT   )OS_OPT_POST_ALL,
               (OS_ERR  *)&err);
		Key_AccessTimes(&Key2,KEY_ACCESS_WRITE_CLEAR);
	 }
}

/*
*********************************************************************************************************
*	函 数 名: _WriteByte2File()
*	功能说明: 
*	形    参：  	
*	返 回 值: 
*********************************************************************************************************
*/
void _WriteByte2File(uint8_t Data, void * p) 
{
	result = f_write(p, &Data, 1, &bw);
}

/*
*********************************************************************************************************
*                                          APP TASK FUN
*
* Description : 
*
* Arguments   : p_arg   is the argument passed to 'AppTask***()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/
static	void  AppTaskScreenShot(void *p_arg)
{
	OS_ERR  err;
	CPU_TS 	Ts;
	char 	buf[20];
	(void)p_arg;
	OS_DEBUG_FUNC();
	while(1)
	{
		//阻塞任务直到按键被按下！
		OSSemPend((OS_SEM   *)&SEM_KEY2,
						 (OS_TICK   )0,                     //如果这个参数设置为0就一直等待下去
						 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //如果没有信号量可用就等待
						 (CPU_TS   *)&Ts,             //这个参数是指向存放信号量被提交、强制解除等待状态、或者信号量被删除的时间戳        
						 (OS_ERR   *)&err);
		sprintf(buf,"0:/picture%d.bmp",Pic_Name);
		OSSchedLock(&err);
		printf("start shot picture\n\r");
		/* 创建截图 */
		result = f_open(&file,buf, FA_WRITE|FA_CREATE_ALWAYS);
		/* 向SD卡绘制BMP图片 */
		GUI_BMP_Serialize(_WriteByte2File,&file);
		
		/* 创建完成后关闭file */
		result = f_close(&file);
		printf("shot picture success\n\r");
		Pic_Name++;
		OSSchedUnlock(&err);
		OSTimeDlyHMSM(0, 0, 0, 10,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);		
	}
}

static	void  AppTaskGuiKeyPad(void *p_arg)
{
	OS_ERR      err;
	(void)p_arg;
	OS_DEBUG_FUNC();
	while(1)
	{
		//LED2_TOGGLE;
		OSTimeDlyHMSM(0, 0, 5, 0,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
	}
}

static	void  AppTaskGui(void *p_arg)
{
	(void)p_arg;
	OS_DEBUG_FUNC();
	while(1)
	{
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
	OS_ERR  err;
	OSTaskCreate((OS_TCB     *)&AppTaskScreenShotTCB,                /* Create the ScreenShot task                                */
                 (CPU_CHAR   *)"App Task Screen Shot",
                 (OS_TASK_PTR ) AppTaskScreenShot,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_SCREENSHOT_PRIO,
                 (CPU_STK    *)&AppTaskScreenShotStk[0],
                 (CPU_STK_SIZE) APP_TASK_SCREENSHOT_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_SCREENSHOT_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
	
	OSTaskCreate((OS_TCB     *)&AppTaskGuiKeyPadTCB,                /* Create the GuiKeyPad task                                */
                 (CPU_CHAR   *)"App Task GuiKeyPad",
                 (OS_TASK_PTR ) AppTaskGuiKeyPad,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_GUIKEYPAD_PRIO,
                 (CPU_STK    *)&AppTaskGuiKeyPadStk[0],
                 (CPU_STK_SIZE) APP_TASK_GUIKEYPAD_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_GUIKEYPAD_STK_SIZE,
                 (OS_MSG_QTY  ) 2u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
	
	OSTaskCreate((OS_TCB     *)&AppTaskGuiTCB,                /* Create the GuiKeyPad task                                */
                 (CPU_CHAR   *)"App Task Gui",
                 (OS_TASK_PTR ) AppTaskGui,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_GUI_PRIO,
                 (CPU_STK    *)&AppTaskGuiStk[0],
                 (CPU_STK_SIZE) APP_TASK_GUI_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_GUI_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
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
	OS_ERR      err;
	
	//创建一个按键，按键的具体细节如无兴趣可不管
	KeyCreate(&Key2,GetPinStateOfKey2);
	//创建一个信号量，按键按下的时候post一个信号量给其他任务
	OSSemCreate ((OS_SEM      *)&SEM_KEY2,     //指向信号量变量的指针
			   (CPU_CHAR    *)"SEM_KEY2",    //信号量的名字
			   (OS_SEM_CTR   )0,             //信号量这里是指示事件发生，所以赋值为0，表示事件还没有发生
			   (OS_ERR      *)&err);         //错误类型
}

/* ------------------------------------------end of file---------------------------------------- */

