/***************************Copyright BestFu 2014-05-14*************************
文	件：	Insturct.c
说	明：	系统一些运行状态的，指示函数
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.06.30 
修　改：	暂无
*******************************************************************************/
#include "Instruct.h"
#include "SysHard.h"
#include "GPIO.h"
#include "Thread.h"

#if RUN_INSTRUCT
static void InstructLED_OFF(void);
#endif

/*******************************************************************************
函 数 名：	Instruct_Init
功能说明： 	指示相关函数初始化
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void Instruct_Init(void)
{
#if RUN_INSTRUCT
	GPIOx_Cfg(RUN_PORT, RUN_PIN, OUT_PP_2M);
	Thread_Login(MANY, 16, 250, &Instruct_Run);  
#endif
}


/*******************************************************************************
函 数 名：	Instruct
功能说明： 	设备响应指示函数
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void Instruct(void)
{
#if RUN_INSTRUCT
	Thread_Login(MANY, 20, 100, &Instruct_Run);
#endif
}

/*******************************************************************************
函 数 名：	Instruct_Run
功能说明： 	指示系统上电运行
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void Instruct_Run(void)
{
#if RUN_INSTRUCT
	RUN_ON;
	Thread_Login(ONCEDELAY, 0, 2, &InstructLED_OFF);
#endif
}

#if RUN_INSTRUCT
/*******************************************************************************
函 数 名：	RunLED_OFF
功能说明： 	运行指示灯关闭
参	  数： 	无
返 回 值：	无
*******************************************************************************/
static void InstructLED_OFF(void)
{
	RUN_OFF;
}
#endif

/**************************Copyright BestFu 2014-05-14*************************/
