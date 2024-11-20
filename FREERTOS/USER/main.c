#include "includes.h"
//菜单实现 每个显示模块的发送oled队列 都有一个全局开关 
//菜单管理就是对全局开关进行改变 假如第一页显示图片 第二页显示时间日期
//那么清屏关闭其他全局开关就可以显示第一页 对开关的控制进而控制页面
TaskHandle_t app_task_init_handle=NULL;
TaskHandle_t app_task_dht_handle=NULL;
TaskHandle_t app_task_oled_handle=NULL;
TaskHandle_t app_task_usart1_handle=NULL;
TaskHandle_t app_task_rtc_handle=NULL;

static void app_task_init(void *pvParameters);
void app_task_dht(void *pvParameters);
void app_task_oled(void *pvParameters);
void app_task_usart1(void *pvParameters);
void app_task_rtc(void *pvParameters);

void dgb_printf_safe(const char *format, ...)
{
#if DEBUG_PRINTF_EN

	va_list args;
	va_start(args, format);

	/* 获取互斥信号量 */
	xSemaphoreTake(g_mutex_printf, portMAX_DELAY);

	vprintf(format, args);

	/* 释放互斥信号量 */
	xSemaphoreGive(g_mutex_printf);

	va_end(args);
#else
	(void)0;
#endif
	
}
/* 任务列表 */
static const task_t task_tbl[] = {
	{app_task_dht, 	"app_task_dht", 		512, NULL, 5, &app_task_dht_handle},
	{app_task_oled, 	"app_task_oled", 		512, NULL, 5, &app_task_oled_handle},
	{app_task_usart1, 	"app_task_usart1", 		512, NULL, 5, &app_task_usart1_handle},
	{app_task_rtc, 	"app_task_rtc", 		512, NULL, 5, &app_task_rtc_handle},
	{0, 0, 0, 0, 0, 0}
};

/* 事件标志组句柄 */
EventGroupHandle_t g_event_group;

/* 互斥锁 */
SemaphoreHandle_t g_mutex_printf;
SemaphoreHandle_t g_mutex_oled;

/* 消息队列句柄 */
QueueHandle_t g_queue_usart1;
QueueHandle_t g_queue_oled;
QueueHandle_t g_queue_rtc;

/* 变量 */
volatile uint8_t oled_show_time=0;
volatile uint8_t oled_show_date=1;
int main(void)
{

	/* 设置系统中断优先级分组4 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* 初始化延时函数 */
	delay_init(168);								
	
	/* 初始化串口1 */
	uart_init(9600);     

		/* 创建 app_task_init任务 */
	xTaskCreate((TaskFunction_t)app_task_init,			/* 任务入口函数 */
				(const char *)"app_task_init",			/* 任务名字 */
				(uint16_t)512,							/* 任务栈大小 */
				(void *)NULL,							/* 任务入口函数参数 */
				(UBaseType_t)5,							/* 任务的优先级 */
				(TaskHandle_t *)&app_task_init_handle); /* 任务控制块指针 */
	
	
	/* 开启任务调度 */
	vTaskStartScheduler();
	
	while(1);
}
static void app_task_init(void *pvParameters)
{
	uint32_t i=0;
	/* 互斥信号量创建 */
	g_mutex_printf=xSemaphoreCreateMutex();
	g_mutex_oled=xSemaphoreCreateMutex();
	
	/* 创建事件标志组 */
	g_event_group = xEventGroupCreate();
	
	/* 创建消息队列 */
	g_queue_usart1=xQueueCreate(
                           QUEUE_USART1_LEN,
                           sizeof(usart_packet_t)
                      );
	g_queue_oled=xQueueCreate(
                           QUEUE_OLED_LEN,
                           sizeof(oled_t)
                      );
	/* 硬件初始化 */	
	dht11_init();
	LED_Init();
	OLED_Init();
	rtc_init();
	/* 创建用到的任务 */
	taskENTER_CRITICAL();
	i=0;
	while(task_tbl[i].pxTaskCode!=0)
	{
			xTaskCreate(
                          task_tbl[i].pxTaskCode,
                          task_tbl[i].pcName,
                          task_tbl[i].usStackDepth,
                          task_tbl[i].pvParameters,
                          task_tbl[i].uxPriority,
                          task_tbl[i].pxCreatedTask
                      );
		i++;
	}
	taskEXIT_CRITICAL();
	
	/* 删除初始化任务 */
	vTaskDelete(NULL);
}

void app_task_dht(void *pvParameters)
{
	uint8_t dh11_data[5]={0};
	uint8_t buf[16] = {0};
	oled_t oled_dh11;
	BaseType_t xReturn;
	while(1)
	{
		if(dht11_read(dh11_data)==0)
		{
			memset(buf,0,sizeof(buf));
			sprintf((char *)buf,"T:%d.%d H:%d.%d",dh11_data[2],dh11_data[3],dh11_data[0],dh11_data[1]);
			oled_dh11.ctrl=OLED_CTRL_SHOW_STRING;
			oled_dh11.x=0;
			oled_dh11.y=0;
			oled_dh11.font_size=16;
			oled_dh11.str=buf;
			xReturn=xQueueSend(g_queue_oled,
                       &oled_dh11,
                       100);
			if (xReturn != pdPASS)
				dgb_printf_safe("[app_task_dht] xQueueSend oled string error code is %d\r\n", xReturn);
			dgb_printf_safe("[app_task_dht] %s\r\n", buf);
		}
		/* 睡眠延时1000ms */
		vTaskDelay(2000);
		
	}
}
void app_task_oled(void *pvParameters)
{
	BaseType_t xReturn;
    oled_t oled;
	
	OLED_Clear();
	while(1)
	{
		xReturn=xQueueReceive(g_queue_oled,
                             &oled,
                             portMAX_DELAY);
		if(xReturn!=pdPASS)
			continue;
		dgb_printf_safe("%s\r\n",oled.str);
		switch(oled.ctrl)
		{
			case OLED_CTRL_SHOW_STRING :
			{
				OLED_ShowString(oled.x,
								oled.y,
								oled.str,
								oled.font_size);
			}
			break;
		}
		//memset(&oled,0,sizeof(oled));
		
	}

}

void app_task_usart1(void *pvParameters)
{
	BaseType_t xReturn = pdFALSE;
	usart_packet_t usart_packet;
	uint32_t hour,minute,second=0;
	RTC_TimeTypeDef RTC_TimeStructure;
	while(1)
	{
		dgb_printf_safe("test\r\n");
		xReturn=xQueueReceive(g_queue_usart1,
                              &usart_packet,
                              portMAX_DELAY);
		
		if(xReturn!=pdPASS)
			continue;
		if(NULL!=strstr((char *)usart_packet.rx_buf,"get"))
		{
			dgb_printf_safe("[app_task_usart1] xQueueSend %s\r\n", usart_packet.rx_buf);
		}
		if(NULL!=strstr((char *)usart_packet.rx_buf,"TIME SET"))
		{
			sscanf((char *)usart_packet.rx_buf,"TIME SET-%d-%d-%d#",&hour,&minute,&second);
			hour = hour + hour/10*6; //将十进制转换为十六进制
			minute = minute + minute/10*6; //将十进制转换为十六进制
			second = second + second/10*6; //将十进制转换为十六进制
			RTC_TimeStructure.RTC_H12     = RTC_H12_PM;
			RTC_TimeStructure.RTC_Hours   = hour;
			RTC_TimeStructure.RTC_Minutes = minute;
			RTC_TimeStructure.RTC_Seconds = second; 
			RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
		}	
		
	}
}

void app_task_rtc(void *pvParameters)
{
	uint8_t buf[16] = {0};
	oled_t oled_rtc;
	BaseType_t xReturn;
	EventBits_t EventValue;

	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	while(1)
	{
		EventValue=xEventGroupWaitBits(
                g_event_group,   
                EVENT_GROUP_RTC_WAKEUP,  
                pdTRUE,         
                pdFALSE,        
                portMAX_DELAY ); 
		if(EventValue==EVENT_GROUP_RTC_WAKEUP)
		{
			if(oled_show_time==OLED_SHOW_TIME)
			{
				RTC_GetTime(RTC_Format_BCD,&RTC_TimeStructure);
				memset(buf,0,sizeof(buf));
				sprintf((char*)buf,"%02x:%02x:%02x",
									RTC_TimeStructure.RTC_Hours,
									RTC_TimeStructure.RTC_Minutes,
									RTC_TimeStructure.RTC_Seconds);
				oled_rtc.x=0;
				oled_rtc.y=4;
				oled_rtc.ctrl=OLED_CTRL_SHOW_STRING;
				oled_rtc.font_size=16;
				oled_rtc.str=buf;
				dgb_printf_safe("show time %s\r\n",buf);
				xReturn=xQueueSend(g_queue_oled,
								   &oled_rtc,
								   portMAX_DELAY);
				if(xReturn!=pdPASS)
					continue;

				}
			if(oled_show_date==OLED_SHOW_DATE)
			{
				RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);
				memset(buf,0,sizeof(buf));
				sprintf((char *)buf, "20%02x-%02x-%02x-%d", RTC_DateStructure.RTC_Year,
															RTC_DateStructure.RTC_Month,
															RTC_DateStructure.RTC_Date,
															RTC_DateStructure.RTC_WeekDay);
				oled_rtc.x=0;
				oled_rtc.y=2;
				dgb_printf_safe("show date %s\r\n",buf);
				oled_rtc.ctrl=OLED_CTRL_SHOW_STRING;
				oled_rtc.font_size=16;
				oled_rtc.str=buf;
				xReturn=xQueueSend(g_queue_oled,
								   &oled_rtc,
								   portMAX_DELAY);
				if(xReturn!=pdPASS)
					continue;
			}
	

		}
	}
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}


void vApplicationTickHook( void )
{

}
