#include "includes.h"
void rtc_init(void)
{
	
	RTC_DateTypeDef  RTC_DateStructure;
	RTC_InitTypeDef  RTC_InitStructure;
	RTC_TimeTypeDef  RTC_TimeStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;
   /* Enable the PWR clock��ʹ�ܵ�Դ����ʱ�� */
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
 
   /* Allow access to RTC��������ʱ��ݵ�·�����ݵ�·������RTC */
   PWR_BackupAccessCmd(ENABLE);
     

 

   /* Enable the LSE OSC��ʹ��LSE����ʱ�ӣ�32.768KHz�� */
   RCC_LSEConfig(RCC_LSE_ON);
 
   /* Wait till LSE is ready���ȴ�LSE���� */  
   while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
 
 
   /* Select the RTC Clock Source��ѡ��LSE��ΪRTC��ʱ��Դ */
   RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
  

   /* Enable the RTC Clock��ʹ��RTC��Ӳ��ʱ�� */
   RCC_RTCCLKCmd(ENABLE);
 
   /* Wait for RTC APB registers synchronisation���ȴ�RTC��ؼĴ������� */
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
   

   
   /* Set the date: 2022/11/ ����1 */
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
   
	//�رջ��ѹ���
	RTC_WakeUpCmd(DISABLE);
	
	//Ϊ���ѹ���ѡ��RTC���úõ�ʱ��Դ
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	//���û��Ѽ���ֵΪ�Զ����أ�д��ֵĬ����0
	RTC_SetWakeUpCounter(1-1);
	
	//���RTC�����жϱ�־
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	//ʹ��RTC�����ж�
	RTC_ITConfig(RTC_IT_WUT, ENABLE);

	//ʹ�ܻ��ѹ���
	RTC_WakeUpCmd(ENABLE);

	/* Configure EXTI Line22�������ⲿ�жϿ�����22 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;			//��ǰʹ���ⲿ�жϿ�����22
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//�ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;		//�����ش����ж� 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;			//ʹ���ⲿ�жϿ�����22
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;		//����RTC�����жϴ���
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY;	//��ռ���ȼ�Ϊ0x3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;		//��Ӧ���ȼ�Ϊ0x3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//ʹ��
	NVIC_Init(&NVIC_InitStructure);   

}

void  RTC_WKUP_IRQHandler(void)
{
	uint32_t ulReturn;

	BaseType_t  xHigherPriorityTaskWoken = pdFALSE;	

	/* �����ٽ�Σ��ٽ�ο���Ƕ�� */
	ulReturn = taskENTER_CRITICAL_FROM_ISR();	
	
	//����־λ
	if(RTC_GetITStatus(RTC_IT_WUT) == SET)
	{
		
		//�����¼���־��
		xEventGroupSetBitsFromISR(g_event_group,EVENT_GROUP_RTC_WAKEUP,&xHigherPriorityTaskWoken);	
		
		//��ձ�־λ	
		RTC_ClearITPendingBit(RTC_IT_WUT);
		
		EXTI_ClearITPendingBit(EXTI_Line22);
	}

	/* ��������Ϣ������������ȼ����ڵ�ǰ���е��������˳��жϺ��������������л���ִ��ǰ��;
	   ����ȴ���һ��ʱ�ӽ��ĲŽ��������л�
	*/
	if(xHigherPriorityTaskWoken)
	{
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}	
	
	/* �˳��ٽ�� */
	taskEXIT_CRITICAL_FROM_ISR( ulReturn );			
}


