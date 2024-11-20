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
#include "beep.h"
#include "key.h"
#include "dh11.h"
#include "oled.h"
#include "rtc.h"
#include "eeprom.h"
#include "keyboard.h"
#include "esp8266.h"
#include "mfrc522.h"
#include "sfm.h"
#include "esp8266_mqtt.h"
#include "motor.h"
#include "mp3.h"

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

typedef enum __key_fn_t
{
	KEY_NONE=0,	
	KEY_UP,
	KEY_DOWN,
	KEY_ENTER,
	KEY_BACK,
}key_fn_t;

typedef struct __menu_t
{
    uint8_t *item;				//当前项目要显示的字符
    void(*fun)(void *);			//选择某一菜单后执行的功能函数
    struct __menu_t *parent;	//当前项目的父菜单	
    struct __menu_t *child;		//当前项目的子菜单
}menu_t;

 typedef struct __menu_ext_t
{
	menu_t 	*menu;
	uint32_t item_cursor;
	uint32_t item_total;
	key_fn_t key_fn;	
}menu_ext_t;

/* 互斥信号量句柄 */
extern SemaphoreHandle_t g_mutex_printf;
extern SemaphoreHandle_t g_mutex_oled;

/* 事件标志组句柄 */
extern EventGroupHandle_t g_event_group;
extern EventGroupHandle_t g_event_group2_handle;

/* 消息队列句柄 */
extern QueueHandle_t g_queue_usart1;
extern QueueHandle_t g_queue_oled;
extern QueueHandle_t g_queue_rtc;
extern QueueHandle_t g_queue_menu;
extern QueueHandle_t g_queue_kbd;
extern QueueHandle_t g_queue_led;
extern QueueHandle_t g_queue_beep;
extern QueueHandle_t g_queue_mfrc522;
extern QueueHandle_t g_queue_sfm;
extern QueueHandle_t g_queue_esp8266;
extern QueueHandle_t g_queue_motor;
extern QueueHandle_t g_queue_mp3;

/* 任务句柄 */
extern TaskHandle_t app_task_init_handle	;
extern TaskHandle_t app_task_dht_handle		;
extern TaskHandle_t app_task_oled_handle	;
extern TaskHandle_t app_task_usart1_handle	;
extern TaskHandle_t app_task_rtc_handle		;
extern TaskHandle_t app_task_menu_handle	;
extern TaskHandle_t app_task_key_handle		;
extern TaskHandle_t app_task_pass_man_handle;
extern TaskHandle_t app_task_kbd_handle		;
extern TaskHandle_t app_task_esp8266_handle	;
extern TaskHandle_t app_task_led_handle		;
extern TaskHandle_t app_task_beep_handle	;
extern TaskHandle_t app_task_mfrc522_handle	;
extern TaskHandle_t app_task_mfrc522_other_handle;
extern TaskHandle_t app_task_sfm_handle		;
extern TaskHandle_t app_task_send_heart_handle		;
extern TaskHandle_t app_task_motor_handle	;
extern TaskHandle_t app_task_mp3_handle	;

/* 任务 */ 
extern void app_task_dht				(void *pvParameters);
extern void app_task_oled				(void *pvParameters);
extern void app_task_usart1				(void *pvParameters);
void app_task_rtc						(void *pvParameters);
void app_task_menu						(void *pvParameters);
void app_task_key						(void *pvParameters);
void app_task_pass_man					(void *pvParameters);
void app_task_kbd						(void* pvParameters);
void app_task_esp8266					(void* pvParameters);
void app_task_led						(void* pvParameters);
void app_task_beep						(void* pvParameters);
void app_task_mfrc522					(void* pvParameters);
void app_task_mfrc522_other				(void* pvParameters);
void app_task_sfm						(void* pvParameters);
void app_task_send_heart				(void* pvParameters);
void app_task_motor						(void* pvParameters);

/* 变量 */
extern volatile uint8_t show_time;
extern volatile uint8_t show_date;
extern uint8_t  g_esp8266_tx_buf[512];
extern volatile uint8_t lock;
/* 函数 */
extern void dgb_printf_safe(const char *format, ...);
/* 宏定义 */
#define QUEUE_USART1_LEN    		4   	/* 队列的长度，最大可包含多少个消息 */
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



#define RFID_NONE   0
#define RFID_ADD_RFID   1
#define RFID_DEL_RFID   2
#define RFID_READ   3

#define RFID_SAVE_ADDR   0x12


#define SFM_NONE   0
#define SFM_ADD   1
#define SFM_SHOW   2
#define SFM_READ   3
#define SFM_DEL_ALL   4

#define SFM_SAVE_ADDR   0x12

/* 显示控制 */
#define OLED_SHOW_TEMPER            1
#define OLED_SHOW_TIME              1
#define OLED_SHOW_EEPROM_CHANGE     2
#define OLED_SHOW_EEPROM_SET        1
#define OLED_SHOW_EEPROM_NONE       0

#define FLAG_PASS_MAN_NONE			0
#define FLAG_PASS_MAN_AUTH			1
#define FLAG_PASS_MAN_MODIFY		2
#define PASS_MODIFY_STA_NONE	 0
#define PASS_MODIFY_STA_OLD_PASS 1
#define PASS_MODIFY_STA_NEW_PASS 2
#endif
