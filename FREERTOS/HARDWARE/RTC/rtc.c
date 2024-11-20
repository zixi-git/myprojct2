#include "includes.h"
void rtc_init(void)
{
	
	RTC_DateTypeDef  RTC_DateStructure;
	RTC_InitTypeDef  RTC_InitStructure;
	RTC_TimeTypeDef  RTC_TimeStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;
   /* Enable the PWR clock，使能电源管理时钟 */
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
 
   /* Allow access to RTC，允许访问备份电路，备份电路包含了RTC */
   PWR_BackupAccessCmd(ENABLE);
     

 

   /* Enable the LSE OSC，使能LSE的振荡时钟（32.768KHz） */
   RCC_LSEConfig(RCC_LSE_ON);
 
   /* Wait till LSE is ready，等待LSE就绪 */  
   while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
 
 
   /* Select the RTC Clock Source，选中LSE作为RTC的时钟源 */
   RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
  

   /* Enable the RTC Clock，使能RTC的硬件时钟 */
   RCC_RTCCLKCmd(ENABLE);
 
   /* Wait for RTC APB registers synchronisation，等待RTC相关寄存器就绪 */
   RTC_WaitForSynchro();
   
   /* Configure the RTC data register and RTC prescaler*/
   
    /* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)
	                = 32768 /(127+1)/(255+1)
					= 1Hz
	
	*/
   RTC_InitStructure.RTC_AsynchPrediv = 127;
   RTC_InitStructure.RTC_SynchPrediv = 255;
   RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
   RTC_Init(&RTC_InitStructure);
   

   
   /* Set the date: 2022/11/ 星期1 */
   RTC_DateStructure.RTC_Year = 0x22;
   RTC_DateStructure.RTC_Month = RTC_Month_November;
   RTC_DateStructure.RTC_Date = 0x07;
   RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Monday;
   RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
   
   /* Set the time to 16h 40mn 00s AM */
   RTC_TimeStructure.RTC_H12     = RTC_H12_PM;
   RTC_TimeStructure.RTC_Hours   = 0x20;
   RTC_TimeStructure.RTC_Minutes = 0x27;
   RTC_TimeStructure.RTC_Seconds = 0x55; 
   
   RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   
   
	//关闭唤醒功能
	RTC_WakeUpCmd(DISABLE);
	
	//为唤醒功能选择RTC配置好的时钟源
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	//设置唤醒计数值为自动重载，写入值默认是0
	RTC_SetWakeUpCounter(1-1);
	
	//清除RTC唤醒中断标志
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	//使能RTC唤醒中断
	RTC_ITConfig(RTC_IT_WUT, ENABLE);

	//使能唤醒功能
	RTC_WakeUpCmd(ENABLE);

	/* Configure EXTI Line22，配置外部中断控制线22 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;			//当前使用外部中断控制线22
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;		//上升沿触发中断 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;			//使能外部中断控制线22
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;		//允许RTC唤醒中断触发
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY;	//抢占优先级为0x3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;		//响应优先级为0x3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//使能
	NVIC_Init(&NVIC_InitStructure);   

}

void  RTC_WKUP_IRQHandler(void)
{
	uint32_t ulReturn;

	BaseType_t  xHigherPriorityTaskWoken = pdFALSE;	

	/* 进入临界段，临界段可以嵌套 */
	ulReturn = taskENTER_CRITICAL_FROM_ISR();	
	
	//检测标志位
	if(RTC_GetITStatus(RTC_IT_WUT) == SET)
	{
		
		//设置事件标志组
		xEventGroupSetBitsFromISR(g_event_group,EVENT_GROUP_RTC_WAKEUP,&xHigherPriorityTaskWoken);	
		
		//清空标志位	
		RTC_ClearITPendingBit(RTC_IT_WUT);
		
		EXTI_ClearITPendingBit(EXTI_Line22);
	}

	/* 若接收消息队列任务的优先级高于当前运行的任务，则退出中断后立即进行任务切换，执行前者;
	   否则等待下一个时钟节拍才进行任务切换
	*/
	if(xHigherPriorityTaskWoken)
	{
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}	
	
	/* 退出临界段 */
	taskEXIT_CRITICAL_FROM_ISR( ulReturn );			
}


