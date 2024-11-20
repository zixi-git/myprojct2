#ifndef __INCLUDES_H__
#define __INCLUDES_H__
/* 标准C库*/
#include <stdio.h>	
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "stack_macros.h"
#include "semphr.h"
#include "event_groups.h"
#include "queue.h"

/* 外设相关 */
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "dh11.h"
#include "oled.h"
#include "rtc.h"

/* 类型 */
typedef struct __task_t
{
	TaskFunction_t pxTaskCode;
	const char * const pcName;		
	const configSTACK_DEPTH_TYPE usStackDepth;
	void * const pvParameters;
	UBaseType_t uxPriority;
	TaskHandle_t * const pxCreatedTask;
}task_t;

typedef struct __usart_packet_t
{
	uint8_t rx_buf[64];
	uint8_t rx_len;
}usart_packet_t;

typedef struct __oled_t
{

	uint8_t ctrl;
	uint8_t x;
	uint8_t y;

	uint8_t *str;
	uint8_t font_size;
    uint8_t chinese;

	uint8_t clr_region_width;
	uint8_t clr_region_height;	
	
	const uint8_t *pic;
	uint8_t pic_width;
	uint8_t pic_height;
}oled_t;

/* 互斥信号量句柄 */
extern SemaphoreHandle_t g_mutex_printf;
extern SemaphoreHandle_t g_mutex_oled;

/* 事件标志组句柄 */
extern EventGroupHandle_t g_event_group;

/* 消息队列句柄 */
extern QueueHandle_t g_queue_usart1;
extern QueueHandle_t g_queue_oled;
extern QueueHandle_t g_queue_rtc;

/* 任务句柄 */
extern TaskHandle_t app_task_init_handle	;
extern TaskHandle_t app_task_dht_handle		;
extern TaskHandle_t app_task_oled_handle	;
extern TaskHandle_t app_task_usart1_handle	;
extern TaskHandle_t app_task_rtc_handle		;

/* 任务 */ 
extern void app_task_dht				(void *pvParameters);
extern void app_task_oled				(void *pvParameters);
extern void app_task_usart1				(void *pvParameters);
void app_task_rtc						(void *pvParameters);

/* 变量 */
extern volatile uint8_t show_time;
extern volatile uint8_t show_date;
/* 函数 */
extern void dgb_printf_safe(const char *format, ...);


/* 宏定义 */
#define QUEUE_USART1_LEN    			4   	/* 队列的长度，最大可包含多少个消息 */
#define QUEUE_LED_LEN    			4   	/* 队列的长度，最大可包含多少个消息 */
#define QUEUE_BEEP_LEN    			4   	/* 队列的长度，最大可包含多少个消息 */
#define QUEUE_OLED_LEN    			16   	/* 队列的长度，最大可包含多少个消息 */
#define QUEUE_KBD_LEN    			4   	/* 队列的长度，最大可包含多少个消息 */

#define OLED_CTRL_DISPLAY_ON        0x01
#define OLED_CTRL_DISPLAY_OFF       0x02
#define OLED_CTRL_INIT              0x03
#define OLED_CTRL_CLEAR             0x04
#define OLED_CTRL_CLEAR_REGION      0x05
#define OLED_CTRL_SHOW_STRING       0x06
#define OLED_CTRL_SHOW_CHINESE      0x07
#define OLED_CTRL_SHOW_PICTURE      0x08

#define DEBUG_PRINTF_EN             1

#define EVENT_GROUP_KEY1_DOWN		0x01
#define EVENT_GROUP_KEY2_DOWN		0x02
#define EVENT_GROUP_KEY3_DOWN		0x04
#define EVENT_GROUP_KEY4_DOWN		0x08
#define EVENT_GROUP_KEYALL_DOWN		0x0F

#define EVENT_GROUP_RTC_WAKEUP		0x10
#define EVENT_GROUP_RTC_ALARM		0x20

#define EVENT_GROUP_OLED_ON			0x100
#define EVENT_GROUP_OLED_OFF		0x200

#define EVENT_GROUP_SYSTEM_RESET	0x1000


#define EVENT_GROUP_FN_KEY_UP		0x10000
#define EVENT_GROUP_FN_KEY_DOWN		0x20000
#define EVENT_GROUP_FN_KEY_ENTER	0x40000
#define EVENT_GROUP_FN_KEY_BACK		0x80000

#define EVENT_GROUP_GESTURE			0x100000

#define OLED_SHOW_TIME              0
#define OLED_SHOW_DATE              1

#endif
