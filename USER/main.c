#include "includes.h"
static menu_ext_t menu_ext;

TaskHandle_t app_task_init_handle=NULL;
TaskHandle_t app_task_dht_handle=NULL;
TaskHandle_t app_task_oled_handle=NULL;
TaskHandle_t app_task_usart1_handle=NULL;
TaskHandle_t app_task_rtc_handle=NULL;
TaskHandle_t app_task_menu_handle=NULL;
TaskHandle_t app_task_key_handle=NULL;
TaskHandle_t app_task_pass_man_handle=NULL;
TaskHandle_t app_task_kbd_handle=NULL;
TaskHandle_t app_task_esp8266_handle=NULL;
TaskHandle_t app_task_led_handle=NULL;
TaskHandle_t app_task_beep_handle=NULL;
TaskHandle_t app_task_mfrc522_handle=NULL;
TaskHandle_t app_task_mfrc522_other_handle=NULL;
TaskHandle_t app_task_sfm_handle=NULL;
TaskHandle_t app_task_send_heart_handle=NULL;
TaskHandle_t app_task_motor_handle=NULL;
TaskHandle_t app_task_mp3_handle=NULL;


void menu_show(menu_ext_t *menu_ext);
void item_cursor_show(uint8_t x,uint8_t y,uint8_t *cursor);
uint32_t menu_item_total(menu_t *menu);
void motor_run(void);
void mp3_run(uint8_t i);

void respond(uint8_t buf[],uint8_t size,uint8_t display);

static void app_task_rtc_fun(void* pvParameters);
static void app_task_pass_fun(void* pvParameters);
static void app_task_init(void *pvParameters);
static void  app_task_rfid_fun(void *pvParameters);
static void app_task_sfm_fun(void *pvParameters);


void app_task_dht(void *pvParameters);
void app_task_oled(void *pvParameters);
void app_task_usart1(void *pvParameters);
void app_task_rtc(void *pvParameters);
void app_task_menu(void *pvParameters);
void app_task_key(void *pvParameters);
void app_task_eeprom(void *pvParameters);
void app_task_kbd(void *pvParameters);
void app_task_pass_man(void *pvParameters);
void app_task_esp8266(void *pvParameters);
void app_task_beep(void *pvParameters);
void app_task_led(void *pvParameters);
void app_task_mfrc522(void *pvParameters);
void app_task_sfm(void *pvParameters);
void app_task_send_heart(void *pvParameters);
void app_task_motor(void *pvParameters);
void app_task_mp3(void *pvParameters);

menu_t menu_main[];
static menu_t menu_rtc_time[];
static menu_t menu_eeprom_temp[];
static menu_t menu_eeprom_change[];
static menu_t menu_eeprom_set[];
static menu_t menu_rfid[];
static menu_t menu_rfid_read[];
static menu_t menu_rfid_del[];
static menu_t menu_rfid_add[];
static menu_t menu_sfm[];
static menu_t menu_sfm_read[];
static menu_t menu_sfm_del[];
static menu_t menu_sfm_add[];
static menu_t menu_sfm_del_all[];

void dgb_printf_safe(const char *format, ...)
{
#if DEBUG_PRINTF_EN

	va_list args;
	va_start(args, format);

	/* ��ȡ�����ź��� */
	xSemaphoreTake(g_mutex_printf, portMAX_DELAY);

	vprintf(format, args);

	/* �ͷŻ����ź��� */
	xSemaphoreGive(g_mutex_printf);

	va_end(args);
#else
	(void)0;
#endif
	
}
/* �����б� */
static const task_t task_tbl[] = {
	//{app_task_dht, 	"app_task_dht", 		512, NULL, 5, &app_task_dht_handle},
	{app_task_oled, 	"app_task_oled", 		512, NULL, 5, &app_task_oled_handle},
	{app_task_key,			"app_task_key",			512,NULL,5,&app_task_key_handle},
	{app_task_usart1, 	"app_task_usart1", 		512, NULL, 5, &app_task_usart1_handle},
	{app_task_rtc, 	"app_task_rtc", 		512, NULL, 5, &app_task_rtc_handle},
	{app_task_menu, 	"app_task_menu", 		512, &menu_ext, 5, &app_task_menu_handle},
	{app_task_pass_man, 	"app_task_pass_man", 		512, NULL, 5, &app_task_pass_man_handle},
	{app_task_kbd, 	"app_task_kbd", 		512, NULL, 5, &app_task_kbd_handle},
	{app_task_esp8266, 	"app_task_esp8266", 		512, NULL, 5, &app_task_esp8266_handle},
	{app_task_led, 	"app_task_led", 		512, NULL, 5, &app_task_led_handle},
	{app_task_beep, 	"app_task_beep", 		512, NULL, 5, &app_task_beep_handle},
	{app_task_mfrc522, 	"app_task_mfrc522", 		512, NULL, 5, &app_task_mfrc522_handle},
	{app_task_mfrc522_other, 	"app_task_mfrc522_other", 		512, NULL, 5, &app_task_mfrc522_other_handle},
	{app_task_sfm, 	"app_task_sfm", 		512, NULL, 5, &app_task_sfm_handle},
	{app_task_send_heart, 	"app_task_send_heart", 		512, NULL, 5, &app_task_send_heart_handle},
	{app_task_motor, 	"app_task_motor", 		512, NULL, 5, &app_task_motor_handle},
	{app_task_mp3, 	"app_task_mp3", 		512, NULL, 5, &app_task_mp3_handle},
	{0, 0, 0, 0, 0, 0}
};

/* �¼���־���� */
EventGroupHandle_t g_event_group;
EventGroupHandle_t g_event_group2_handle;
/* ������ */
SemaphoreHandle_t g_mutex_printf;
SemaphoreHandle_t g_mutex_oled;

/* ��Ϣ���о�� */
QueueHandle_t g_queue_usart1;
QueueHandle_t g_queue_oled;
QueueHandle_t g_queue_rtc;
QueueHandle_t g_queue_menu;
QueueHandle_t g_queue_kbd;
QueueHandle_t g_queue_beep;
QueueHandle_t g_queue_led;
QueueHandle_t g_queue_mfrc522;
QueueHandle_t g_queue_sfm;
QueueHandle_t g_queue_esp8266;
QueueHandle_t g_queue_motor;
QueueHandle_t g_queue_mp3;
/* ���� */
volatile uint8_t oled_show_time;
volatile uint8_t rfid;
volatile uint8_t sfm;
volatile uint32_t g_pass_man_what = FLAG_PASS_MAN_NONE;
uint32_t pass_modify_sta = PASS_MODIFY_STA_OLD_PASS;
static volatile uint32_t g_oled_display_flag=1;
static volatile uint32_t g_system_no_opreation_cnt=0;	
volatile uint8_t  Read_RFID_Card[6]   = {0}; //��IC���ж�ȡ�Ŀ���
volatile uint8_t  Read_RFID_Card_EEPROM[6]   = {0}; //��EEPROM���ж�ȡ�Ŀ���
volatile uint8_t  Write_RFID_Card[5]   = {0}; //׼��д�뵽EEPROM��IC����Ϣ
volatile uint8_t  RFID_People    =  0; //д��RFID����ʱ ���ÿ���¼��Ӧ����
volatile uint8_t  People      =  0; //��¼������Ϣ�е�������Ϣ
volatile uint8_t  Method      =  0; //��¼������Ϣ�еĿ�����ʽ
volatile uint32_t  tp       =  0; //��ʱ�����־λ
volatile uint8_t  clear_rfid[5]    = {0xFF,0xFF,0xFF,0xFF,0xFF};//���EEPROM ��8���ֽ�
volatile  uint8_t lock=0;


const char word[4]={5,6,7,8};
const char word_error[4]={3,4,9,10};
const char word_suc[2]={11,12};
/* �˵� */


/*	һ���˵�

*/
menu_t menu_main[]=
{	
	{(uint8_t*)"1.RTC "	,NULL,NULL,menu_rtc_time},
	{(uint8_t*)"2.LOCK_KEY "	,NULL,NULL,menu_eeprom_temp},
	{(uint8_t*)"3.LOCK_RFID "	,NULL,NULL,menu_rfid},	
	{(uint8_t*)"4.LOCK_SFM "	,NULL,NULL,menu_sfm},			
	{NULL,NULL,NULL,NULL},
};

/*
	�����˵�

*/

static menu_t menu_eeprom_temp[]=
{
	{(uint8_t*)"1.KEY",NULL,menu_main,menu_eeprom_change},
	{(uint8_t*)"2.KEY-CHANGE",NULL,menu_main,menu_eeprom_set},
	{NULL,NULL,NULL,NULL},	
};

static menu_t menu_rfid[]=
{
	{(uint8_t*)"1.RFID-ADD",NULL,menu_main,menu_rfid_add},
	{(uint8_t*)"2.RFID-DEL",NULL,menu_main,menu_rfid_del},
	{(uint8_t*)"3.RFID-READ",NULL,menu_main,menu_rfid_read},
	{NULL,NULL,NULL,NULL},	
};
static menu_t menu_sfm[]=
{
	{(uint8_t*)"1.SFM-ADD",NULL,menu_main,menu_sfm_add},
	{(uint8_t*)"2.SFM-SHOW",NULL,menu_main,menu_sfm_del},
	{(uint8_t*)"3.SFM-READ",NULL,menu_main,menu_sfm_read},
	{(uint8_t*)"4.SFM-DEL-ALL",NULL,menu_main,menu_sfm_del_all},
	{NULL,NULL,NULL,NULL},	
};
static menu_t menu_rfid_add[]=
{
	{(uint8_t*)"-",app_task_rfid_fun,menu_rfid,NULL},
	{NULL,NULL,NULL,NULL},	
};
static menu_t menu_sfm_add[]=
{
	{(uint8_t*)"-",app_task_sfm_fun,menu_sfm,NULL},
	{NULL,NULL,NULL,NULL},	
};
static menu_t menu_rfid_del[]=
{
	{(uint8_t*)"-",app_task_rfid_fun,menu_rfid,NULL},
	{NULL,NULL,NULL,NULL},	
};
static menu_t menu_sfm_del[]=
{
	{(uint8_t*)"-",app_task_sfm_fun,menu_sfm,NULL},
	{NULL,NULL,NULL,NULL},	
};
static menu_t menu_rfid_read[]=
{
	{(uint8_t*)"-",app_task_rfid_fun,menu_rfid,NULL},
	{NULL,NULL,NULL,NULL},	
};
static menu_t menu_sfm_read[]=
{
	{(uint8_t*)"-",app_task_sfm_fun,menu_sfm,NULL},
	{NULL,NULL,NULL,NULL},	
};
static menu_t menu_sfm_del_all[]=
{
	{(uint8_t*)"-",app_task_sfm_fun,menu_sfm,NULL},
	{NULL,NULL,NULL,NULL},	
};
static menu_t menu_rtc_time[]=
{
	{(uint8_t*)"-",app_task_rtc_fun,menu_main,NULL},
	{NULL,NULL,NULL,NULL},	
};

static menu_t menu_eeprom_change[]=
{
	{(uint8_t*)"-",app_task_pass_fun,menu_eeprom_temp,NULL},
	{NULL,NULL,NULL,NULL},
};
static menu_t menu_eeprom_set[]=
{
	{(uint8_t*)"-",app_task_pass_fun,menu_eeprom_temp,NULL},
	{NULL,NULL,NULL,NULL},
};


static void app_task_rtc_fun(void* pvParameters)
{
	oled_t oled;
	menu_ext_t *menu_ext = (menu_ext_t *)pvParameters;

	BaseType_t xReturn;

	dgb_printf_safe("[menu_rtc_fun] ...\r\n");

		oled.ctrl=OLED_CTRL_CLEAR;
		xReturn = xQueueSend( 	g_queue_oled,	/* ��Ϣ���еľ�� */
								&oled,			/* ���͵���Ϣ���� */
								100);			/* �ȴ�ʱ�� 100 Tick */
		if(xReturn != pdPASS)
			dgb_printf_safe("[menu_rtc_fun] xQueueSend oled picture error code is %d\r\n",xReturn);	
	/* ʶ��ENTER�������룬��ָ�DHT�������в���ȡ�¶�/ʪ�� */	
	if(menu_ext->key_fn == KEY_ENTER)
	{
		oled_show_time=OLED_SHOW_TIME;
		/* �ָ�DHT�������� */ 
		vTaskResume(app_task_rtc_handle);
	}

	/* ʶ��BACK�������룬��ֹͣDHT�������� */	
	if(menu_ext->key_fn == KEY_BACK)
		vTaskSuspend(app_task_rtc_handle);
}

static void app_task_rfid_fun(void* pvParameters)
{
	oled_t oled;
	menu_ext_t *menu_ext = (menu_ext_t *)pvParameters;
	uint8_t id;
	BaseType_t xReturn;

	dgb_printf_safe("[menu_rfid_fun] ...\r\n");

		oled.ctrl=OLED_CTRL_CLEAR;
		xReturn = xQueueSend( 	g_queue_oled,	/* ��Ϣ���еľ�� */
								&oled,			/* ���͵���Ϣ���� */
								100);			/* �ȴ�ʱ�� 100 Tick */
		if(xReturn != pdPASS)
			dgb_printf_safe("[menu_rfid_fun] xQueueSend oled picture error code is %d\r\n",xReturn);	
	/* ʶ��ENTER�������룬��ָ�DHT�������в���ȡ�¶�/ʪ�� */	
	if(menu_ext->key_fn == KEY_ENTER)
	{
		if(menu_ext->item_cursor==0)
		{
			id=RFID_ADD_RFID;
			xReturn = xQueueSend( g_queue_mfrc522,	/* ��Ϣ���еľ�� */
								&id,			/* ���͵���Ϣ���� */
								100);			/* �ȴ�ʱ�� 100 Tick */
			if(xReturn != pdPASS)
				dgb_printf_safe("[menu_rfid_fun] xQueueSend oled picture error code is %d\r\n",xReturn);	
			vTaskResume(app_task_mfrc522_other_handle);
			vTaskSuspend(app_task_mfrc522_handle);
		}
		if(menu_ext->item_cursor==1)
		{
			id=RFID_DEL_RFID;
			xReturn = xQueueSend( g_queue_mfrc522,	/* ��Ϣ���еľ�� */
								&id,			/* ���͵���Ϣ���� */
								100);			/* �ȴ�ʱ�� 100 Tick */
			if(xReturn != pdPASS)
				dgb_printf_safe("[menu_rfid_fun] xQueueSend oled picture error code is %d\r\n",xReturn);	
			vTaskResume(app_task_mfrc522_other_handle);
			vTaskSuspend(app_task_mfrc522_handle);
		}
		if(menu_ext->item_cursor==2)
		{
			rfid=RFID_READ;
			vTaskResume(app_task_mfrc522_handle);
			vTaskSuspend(app_task_mfrc522_other_handle);
		}
		/* �ָ�DHT�������� */ 
		
	}

	/* ʶ��BACK�������룬��ֹͣDHT�������� */	
	if(menu_ext->key_fn == KEY_BACK)
	{
		vTaskSuspend(app_task_mfrc522_other_handle);
		vTaskSuspend(app_task_mfrc522_handle);
		rfid=RFID_NONE;
	}

}
static void app_task_sfm_fun(void *pvParameters)
{
	oled_t oled;
	menu_ext_t *menu_ext = (menu_ext_t *)pvParameters;
	BaseType_t xReturn;

	dgb_printf_safe("[menu_sfm_fun] ...\r\n");

		oled.ctrl=OLED_CTRL_CLEAR;
		xReturn = xQueueSend( 	g_queue_oled,	/* ��Ϣ���еľ�� */
								&oled,			/* ���͵���Ϣ���� */
								100);			/* �ȴ�ʱ�� 100 Tick */
		if(xReturn != pdPASS)
			dgb_printf_safe("[menu_sfm_fun] xQueueSend oled picture error code is %d\r\n",xReturn);	
	/* ʶ��ENTER�������룬��ָ�DHT�������в���ȡ�¶�/ʪ�� */	
	if(menu_ext->key_fn == KEY_ENTER)
	{
		if(menu_ext->item_cursor==0)
		{
			sfm=SFM_ADD;
				
		}
		if(menu_ext->item_cursor==1)
		{
			sfm=SFM_SHOW;
		}
		if(menu_ext->item_cursor==2)
		{
			sfm=SFM_READ;

		}
		if(menu_ext->item_cursor==3)
		{
			sfm=SFM_DEL_ALL;

		}
		/* �ָ�DHT�������� */ 
		fpm_ctrl_led(FPM_LED_BLUE);/* ��ʾ��ɫ */
		vTaskResume(app_task_sfm_handle);
	}

	/* ʶ��BACK�������룬��ֹͣDHT�������� */	
	if(menu_ext->key_fn == KEY_BACK)
	{
		sfm=SFM_NONE;
		vTaskSuspend(app_task_sfm_handle);
		
	}
}

static void app_task_pass_fun(void* pvParameters)
{
	oled_t oled;
	menu_ext_t *menu_ext = (menu_ext_t *)pvParameters;

	BaseType_t xReturn;

	dgb_printf_safe("[menu_pass_fun] ...\r\n");

		oled.ctrl=OLED_CTRL_CLEAR;
		xReturn = xQueueSend( 	g_queue_oled,	/* ��Ϣ���еľ�� */
								&oled,			/* ���͵���Ϣ���� */
								100);			/* �ȴ�ʱ�� 100 Tick */
		if(xReturn != pdPASS)
			dgb_printf_safe("[menu_pass_fun] xQueueSend oled picture error code is %d\r\n",xReturn);	
	/* ʶ��ENTER�������룬��ָ�DHT�������в���ȡ�¶�/ʪ�� */	
	if(menu_ext->key_fn == KEY_ENTER)
	{
		if(menu_ext->item_cursor==0)
		{
			g_pass_man_what=FLAG_PASS_MAN_AUTH;
		}
		if(menu_ext->item_cursor==1)
		{
			g_pass_man_what=FLAG_PASS_MAN_MODIFY;
		}
		/* �ָ�DHT�������� */ 
		vTaskResume(app_task_pass_man_handle);
	}

	/* ʶ��BACK�������룬��ֹͣDHT�������� */	
	if(menu_ext->key_fn == KEY_BACK)
		vTaskSuspend(app_task_pass_man_handle);
}

int main(void)
{

	/* ����ϵͳ�ж����ȼ�����4 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* ��ʼ����ʱ���� */
	delay_init(168);								
	
	/* ��ʼ������1 */
	usart1_init(9600); //����������   
	//usart2_init(115200);	//ָ��ģ��ռ��
	//usart3_init(115200);//esp01sռ��
		/* ���� app_task_init���� */
	xTaskCreate((TaskFunction_t)app_task_init,			/* ������ں��� */
				(const char *)"app_task_init",			/* �������� */
				(uint16_t)512,							/* ����ջ��С */
				(void *)NULL,							/* ������ں������� */
				(UBaseType_t)5,							/* ��������ȼ� */
				(TaskHandle_t *)&app_task_init_handle); /* ������ƿ�ָ�� */
	
	
	/* ����������� */
	vTaskStartScheduler();
	
	while(1);
}
static void app_task_init(void *pvParameters)
{
	uint32_t i=0;
	/* �����ź������� */
	g_mutex_printf=xSemaphoreCreateMutex();
	g_mutex_oled=xSemaphoreCreateMutex();
	
	/* �����¼���־�� */
	g_event_group = xEventGroupCreate();
	g_event_group2_handle=xEventGroupCreate();
	
	/* ������Ϣ���� */
	g_queue_usart1=xQueueCreate(  QUEUE_USART1_LEN, sizeof(usart_packet_t) );
	
	g_queue_oled=xQueueCreate( QUEUE_OLED_LEN, sizeof(oled_t) );
	
	g_queue_menu=xQueueCreate( QUEUE_OLED_LEN, sizeof(oled_t) );
	
	g_queue_kbd=xQueueCreate( QUEUE_KBD_LEN, sizeof(char) );
	
	g_queue_led=xQueueCreate(4,  sizeof(uint8_t) );
	
	g_queue_beep=xQueueCreate( 4, sizeof(uint8_t));
	
    g_queue_mfrc522=xQueueCreate( 4,sizeof(uint8_t));
	
	g_queue_sfm=xQueueCreate(4,sizeof(uint8_t));
	
	g_queue_esp8266=xQueueCreate(4,sizeof(uint8_t));
	
	g_queue_motor=xQueueCreate(4,sizeof(uint8_t));
	
	g_queue_mp3=xQueueCreate(4,sizeof(uint8_t));
	
						   
	/* Ӳ����ʼ�� */	
	mp3_init();
	dht11_init();
	led_init();
	OLED_Init();
	rtc_init();
	key_init();
	at24c02_init();
	Matrix_Keyboard_Init();
	beep_init();
	MFRC522_SPI1_Init();
	MOTOR_Init();
	fpm_init();
	esp8266_init();
	/* �����õ������� */
	menu_ext.menu=menu_main;
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
	menu_ext.menu = menu_main;
	menu_show(&menu_ext);
	vTaskSuspend(app_task_rtc_handle);
	
	/* ɾ����ʼ������ */
	vTaskDelete(NULL);
}
void app_task_send_heart(void *pvParameters)
{
	uint8_t i=0;
	while(1)
	{
		vTaskDelay(10000);
		i++;
		if(i==5)
		{
			mqtt_send_heart();	
			i=0;			
		}
		//60��ʱ�䵽��
		/*	�豸���ڱ���ʱ������(����ʱ����mqtt_connect����Ϊ60s)��������Ҫ����һ�α��ģ�����ping����
				���ӱ���ʱ���ȡֵ��ΧΪ30��~1200�롣����ȡֵ300�����ϡ�
				��������ƽ̨����CONNACK��ӦCONNECT��Ϣʱ����ʼ������ʱ���յ�PUBLISH��SUBSCRIBE��PING�� PUBACK��Ϣʱ�������ü�ʱ����
			*/
			//����������������Ƶ���������������������������һ��ʱ�䲻������Ӧ��Ϣ[��ѡ]				
	}
}
void app_task_mfrc522(void *pvParameters)
{
	u8    status;
	u8    card_typebuf[2];
	while(1)
	{ 
		vTaskDelay(1000);
		if(rfid==RFID_READ)
		{
			MFRC522_Initializtion();
			status = MFRC522_Request(0x52, card_typebuf);
			if(status==0)  
			{
				memset((uint8_t*)Read_RFID_Card,0,sizeof(Read_RFID_Card));
				MFRC522_Anticoll((u8 *)Read_RFID_Card);
				dgb_printf_safe("read id \r\n");
				memset((uint8_t*)Read_RFID_Card_EEPROM,0,sizeof(Read_RFID_Card_EEPROM));
				at24c02_read(RFID_SAVE_ADDR,(uint8_t *)Read_RFID_Card_EEPROM,sizeof(Read_RFID_Card));
				Read_RFID_Card[5]='\0';
				Read_RFID_Card_EEPROM[5]='\0';
				if(0==strcmp((char *)Read_RFID_Card,(char *)Read_RFID_Card_EEPROM))
				{
					dgb_printf_safe("unlock\r\n");
					respond((uint8_t*)word,sizeof(word),1);
					mqtt_report_devices_status(4);
					motor_run();
					mp3_run(5);
				//PAout(8) = 0;
				}
			}
	
		}
	}
}

void app_task_mfrc522_other(void *pvParameters)
{
	oled_t 		oled;
	uint8_t     id_flage;
	BaseType_t	xReturn;
	uint8_t 	status;
	uint8_t 	timeout		= 0;
	uint8_t  	card_typebuf[2];
	while(1)
	{		
	
		xReturn=xQueueReceive(g_queue_mfrc522,
                              &id_flage,
                              portMAX_DELAY);
		
		if(xReturn!=pdPASS)
			continue;
		if(id_flage ==RFID_ADD_RFID)
		{
			vTaskSuspend(app_task_mfrc522_handle);
			oled.ctrl=OLED_CTRL_SHOW_STRING;
			oled.x=40;
			oled.y=4;
			oled.font_size=16;
			oled.str = "Add ID";	
			xReturn = xQueueSend( 	g_queue_oled,/* ��Ϣ���еľ�� */
									&oled,		/* ���͵���Ϣ���� */
									100);		/* �ȴ�ʱ�� 100 Tick */
			if(xReturn != pdPASS)
					dgb_printf_safe("[app_task_mfrc522_other] xQueueSend oled dht11 string error code is %d\r\n",xReturn);
			timeout = 0;
			MFRC522_Initializtion();
			while(MFRC522_Request(0x52, card_typebuf) != 0 && timeout<30)
			{
				timeout++;
				delay_ms(100);
			}
			if(timeout>1000)
				status = 1;
			else
				status = 0;
			if(status==0)		
			{
				memset((uint8_t*)Read_RFID_Card,0,sizeof(Read_RFID_Card));
				MFRC522_Anticoll((u8 *)Read_RFID_Card);
				taskENTER_CRITICAL();
				at24c02_write(RFID_SAVE_ADDR,(uint8_t *)Read_RFID_Card,sizeof(Read_RFID_Card));
				taskEXIT_CRITICAL();
				dgb_printf_safe("add id success\r\n");
				respond((uint8_t*)word_suc,sizeof(word_suc),1);
			}
		}

		if(id_flage ==RFID_DEL_RFID)
		{
			vTaskSuspend(app_task_mfrc522_handle);
			oled.ctrl=OLED_CTRL_SHOW_STRING;
			oled.x=16;
			oled.y=4;
			oled.font_size=16;
			oled.str = "Del ID";	
			xReturn = xQueueSend( 	g_queue_oled,/* ��Ϣ���еľ�� */
									&oled,		/* ���͵���Ϣ���� */
									100);		/* �ȴ�ʱ�� 100 Tick */
			if(xReturn != pdPASS)
					dgb_printf_safe("[app_task_mfrc522_other] xQueueSend oled dht11 string error code is %d\r\n",xReturn);
			timeout = 0;
			MFRC522_Initializtion();
			while(MFRC522_Request(0x52, card_typebuf) != 0 && timeout<30)
			{
				timeout++;
				delay_ms(100);
			}
			if(timeout>1000)
				status = 1;
			else
				status = 0;
			if(status==0)		
			{
				memset((uint8_t*)Read_RFID_Card,0,sizeof(Read_RFID_Card));
				MFRC522_Anticoll((u8 *)Read_RFID_Card);
				taskENTER_CRITICAL();
				at24c02_write(RFID_SAVE_ADDR,(uint8_t *)clear_rfid,sizeof(clear_rfid));
				taskEXIT_CRITICAL();
				dgb_printf_safe("del id success\r\n");
				respond((uint8_t*)word_suc,sizeof(word_suc),1);
			} 
		}
	}
}

void app_task_sfm(void *pvParameters)
{
	uint8_t fmp_error_code;
	uint16_t id_total;
	uint16_t 	id;
	oled_t 		oled;
	BaseType_t	xReturn;
	while(1)
	{
		if(sfm == SFM_ADD)//��ָ��
		{
			dgb_printf_safe("add\r\n");
			fpm_ctrl_led(FPM_LED_BLUE);/* ��ʾ��ɫ */
			sfm=SFM_NONE;
			oled.ctrl=OLED_CTRL_SHOW_STRING;
			oled.x=0;
			oled.y=4;
			oled.font_size=16;
			oled.str = (uint8_t*)"Add  Fingerprint";	
			xReturn = xQueueSend( 	g_queue_oled,/* ��Ϣ���еľ�� */
									&oled,		/* ���͵���Ϣ���� */
									100);		/* �ȴ�ʱ�� 100 Tick */
			if(xReturn != pdPASS)
					dgb_printf_safe("[app_task_sfm] xQueueSend oled dht11 string error code is %d\r\n",xReturn);
				fpm_ctrl_led(FPM_LED_BLUE);
			
			printf("\r\n\r\n=====================================\r\n\r\n");
			printf("ִ�����ָ�Ʋ���,�뽫��ָ�ŵ�ָ��ģ�鴥����Ӧ��\r\n");
			
			fmp_error_code =fpm_id_total(&id_total);

            if(fmp_error_code == 0)
            {
				sfm=SFM_NONE;
                printf("��ȡָ��������%04d\r\n",id_total);
				
				/* ���ָ��*/
				fmp_error_code=fpm_enroll_auto(id_total+1);

				if(fmp_error_code == 0)
				{
					fpm_ctrl_led(FPM_LED_GREEN);			
					respond((uint8_t *)word_suc,sizeof(word_suc),1);
					printf("�Զ�ע��ָ�Ƴɹ�\r\n");
			
				}
				else
				{
					fpm_ctrl_led(FPM_LED_RED);
					respond((uint8_t *)word_error,sizeof(word_error),1);
					
					oled.str =(uint8_t*)"add_fal";
					xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
										&oled,		   /* ���͵���Ϣ���� */
										100);		   /* �ȴ�ʱ�� 100 Tick */
					if (xReturn != pdPASS)
					dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);
				}
				
				delay_ms(100);	
				
				fpm_sleep();
				
				delay_ms(1000);					
            }
		}
		if(sfm == SFM_DEL_ALL)//ɾ������ָ��
		{
			sfm=SFM_NONE;
			fpm_ctrl_led(FPM_LED_BLUE);
			
			printf("\r\n\r\n=====================================\r\n\r\n");
			
			fmp_error_code=fpm_empty();
			
            if(fmp_error_code == 0)
            {
				fpm_ctrl_led(FPM_LED_GREEN);
				
                printf("���ָ�Ƴɹ�\r\n");
                 respond((uint8_t *)word_suc,sizeof(word_suc),1);
            }
			else
			{
				fpm_ctrl_led(FPM_LED_RED);
				respond((uint8_t *)word_error,sizeof(word_error),1);
			}			
			delay_ms(100);		
			
			fpm_sleep();	

			delay_ms(1000);		
		}
		if(sfm == SFM_SHOW)//��ʾָ��
		{
			sfm=SFM_NONE;
			fpm_ctrl_led(FPM_LED_BLUE);
			
			printf("\r\n\r\n=====================================\r\n\r\n");
			
			fmp_error_code =fpm_id_total(&id_total);

            if(fmp_error_code == 0)
            {
				fpm_ctrl_led(FPM_LED_GREEN);
				
                printf("��ȡָ��������%04d\r\n",id_total);
               respond((uint8_t *)word_suc,sizeof(word_suc),1);
            }
			else
			{
				fpm_ctrl_led(FPM_LED_RED);
				respond((uint8_t *)word_error,sizeof(word_error),1);
			}			
			
			delay_ms(100);		
			
			fpm_sleep();	

			delay_ms(1000);	
				
		}
		if(sfm == SFM_READ)//����
		{
			sfm=SFM_NONE;
			fpm_ctrl_led(FPM_LED_BLUE);
			
			printf("\r\n\r\n=====================================\r\n\r\n");
			printf("ִ��ˢָ�Ʋ���,�뽫��ָ�ŵ�ָ��ģ�鴥����Ӧ��\r\n");
			
			/* ����Ϊ0xFFFF����1:Nƥ�� */
			id = 0xFFFF;
			
			fmp_error_code=fpm_idenify_auto(&id);
            
            if(fmp_error_code == 0)
            {
				fpm_ctrl_led(FPM_LED_GREEN);
				
                printf("�Զ���ָ֤��%04d�ɹ�!\r\n",id);
				mqtt_report_devices_status(3);
				motor_run();
				mp3_run(5);
				respond((uint8_t *)word,sizeof(word),1);
            }
			else
			{
				fpm_ctrl_led(FPM_LED_RED);
				respond((uint8_t *)word_error,sizeof(word_error),1);
			}
			
			delay_ms(100);		
			
			fpm_sleep();	

			delay_ms(1000);		
			
		}
		vTaskDelay(1000);
	}
	
}
void app_task_led(void *pvParameters)
{
	uint8_t led_sta = 0;
	BaseType_t xReturn = pdFALSE;

	dgb_printf_safe("[app_task_led] create success\r\n");

	for (;;)
	{
		xReturn = xQueueReceive(g_queue_led,	/* ��Ϣ���еľ�� */
								&led_sta,		/* �õ�����Ϣ���� */
								portMAX_DELAY); /* �ȴ�ʱ��һֱ�� */
		if (xReturn != pdPASS)
			continue;
		if(led_sta==1)
		{
			PFout(9)=0;
			vTaskDelay(500);
			PFout(9)=1;
		}
	}
}
void app_task_beep(void *pvParameters)
{
	uint8_t beep = 0;
	BaseType_t xReturn = pdFALSE;

	dgb_printf_safe("[app_task_led] create success\r\n");

	for (;;)
	{
		xReturn = xQueueReceive(g_queue_beep,	/* ��Ϣ���еľ�� */
								&beep,		/* �õ�����Ϣ���� */
								portMAX_DELAY); /* �ȴ�ʱ��һֱ�� */
		if (xReturn != pdPASS)
			continue;
		if(beep==1)
		{
			PFout(8)=1;
			vTaskDelay(200);
			PFout(8)=0;
		}
	}
}
void app_task_motor(void *pvParameters)
{
	uint8_t motor = 0;
	BaseType_t xReturn = pdFALSE;

	dgb_printf_safe("[app_task_led] create success\r\n");

	for (;;)
	{
		xReturn = xQueueReceive(g_queue_motor,	/* ��Ϣ���еľ�� */
								&motor,		/* �õ�����Ϣ���� */
								portMAX_DELAY); /* �ȴ�ʱ��һֱ�� */
		if (xReturn != pdPASS)
			continue;
		if(motor==1)
		{
			motor_corotation_double_pos();
			vTaskDelay(1500);
			motor_corotation_double_rev();
			
		}
	}
}

void app_task_pass_man(void *pvParameters)
{
	#define PASS1_ADDR_IN_EEPROM	0
	#define PASS_LEN				6

	#define PASS_MODIFY_STA_NONE	 0
	#define PASS_MODIFY_STA_OLD_PASS 1
	#define PASS_MODIFY_STA_NEW_PASS 2
	oled_t	 oled;
	BaseType_t xReturn;
	int32_t rt;	
	char 	 key_val=0;
	char 	 key_buf[PASS_LEN]={0};
	uint32_t key_cnt=0;
	
	char 	 pass_old[PASS_LEN]={0};
	char 	 pass_new[PASS_LEN]={0};
	const 
	char 	 pass_auth_default[PASS_LEN]={'1','2','3','4','5','6'};
	char 	 pass_auth_eeprom[PASS_LEN]={0};
	const uint8_t x_start=66;	
	const uint8_t y_start=4;		
	uint8_t	 x=x_start,y=y_start;
	dgb_printf_safe("[app_task_pass_man] create success\r\n");
	while(1)
	{
		/* �ȴ����������Ϣ */
		xReturn = xQueueReceive(g_queue_kbd,&key_val,portMAX_DELAY);

		if(xReturn!=pdPASS)
			continue;
			if(key_val == 'C')
		{
			x=x_start;
			y=y_start;
			key_cnt=0;

			memset(key_buf, 0,sizeof key_buf);
			memset(pass_old,0,sizeof pass_old);	
			memset(pass_new,0,sizeof pass_new);
			oled.ctrl = OLED_CTRL_SHOW_STRING;
			oled.x = x;
			oled.y = y;
			oled.font_size = 16;

			oled.str =(uint8_t*)"------";
			xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
								&oled,		   /* ���͵���Ϣ���� */
								100);		   /* �ȴ�ʱ�� 100 Tick */
			if (xReturn != pdPASS)
				dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);			
		dgb_printf_safe("��λ\r\n");
			continue;
		}
		if(g_pass_man_what == FLAG_PASS_MAN_AUTH)
		{
			/* û�����룬��Чɾ������ */
			if(key_val == '*' && key_cnt==0)
				continue;

			/* ɾ����һ������ */
			if(key_val == '*' && key_cnt)
			{
				key_cnt--;
				key_buf[key_cnt]=0;

				x-=8;
				oled.ctrl = OLED_CTRL_SHOW_STRING;
				oled.x = x;
				oled.y = y;
				oled.font_size = 16;

				oled.str =(uint8_t*)"-";
				xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
									&oled,		   /* ���͵���Ϣ���� */
									100);		   /* �ȴ�ʱ�� 100 Tick */
				if (xReturn != pdPASS)
					dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);

				continue;				
			}
			if(key_cnt<6)
			{
				/* ��ʾ���� */
				key_buf[key_cnt] = key_val;
				
				oled.ctrl = OLED_CTRL_SHOW_STRING;
				oled.x = x;
				oled.y = y;
				oled.font_size = 16;

				oled.str =(uint8_t *) &key_buf[key_cnt];
				xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
									&oled,		   /* ���͵���Ϣ���� */
									100);		   /* �ȴ�ʱ�� 100 Tick */
				if (xReturn != pdPASS)
					dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);

				vTaskDelay(100);

				/* ��ʾ* */
				oled.str = (uint8_t*)"*";
				xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
									&oled,		   /* ���͵���Ϣ���� */
									100);		   /* �ȴ�ʱ�� 100 Tick */
				if (xReturn != pdPASS)
					dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);

				x+=8;
				key_cnt++;
			}
			else
			{
				if(key_val == '#')
				{
					dgb_printf_safe("[app_task_pass_man] auth key buf is %6s\r\n", key_buf);		
					
					/* ��ȡeeprom�洢������ */
					at24c02_read(PASS1_ADDR_IN_EEPROM,(uint8_t *)pass_auth_eeprom,PASS_LEN);

					/* ƥ��eeprom�洢������ */
					rt = memcmp(pass_auth_eeprom,key_buf,PASS_LEN);
					if(rt)
					{
						dgb_printf_safe("[app_task_pass_man] password auth by eeprom fail\r\n");
						
						/* ƥ��Ĭ������ */
						rt=memcmp(pass_auth_default,key_buf,PASS_LEN);
						if(rt)
							dgb_printf_safe("[app_task_pass_man] password auth by defalut fail too\r\n");
					}
					if(rt==0)
					{
						dgb_printf_safe("����ƥ��ɹ�\r\n");
						oled.ctrl=OLED_CTRL_CLEAR;
						xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
										&oled,		   /* ���͵���Ϣ���� */
										100);		   /* �ȴ�ʱ�� 100 Tick */
						if (xReturn != pdPASS)
						dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);
						respond((uint8_t*)word,sizeof(word),1);
						mqtt_report_devices_status(1);
						motor_run();
						mp3_run(5);
					}
					else
					{
						dgb_printf_safe("[app_task_pass_man] xQueueSend oled picture error code is %d\r\n",xReturn);
						respond((uint8_t*)word_error,sizeof(word_error),1);

					}
					/* ��ʱһ�� */
					vTaskDelay(2000);

					/* �ָ���ֵ */
					x=x_start;
					key_cnt=0;
					memset(key_buf,0,sizeof key_buf);	

					oled.ctrl=OLED_CTRL_CLEAR;
					xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
									&oled,		   /* ���͵���Ϣ���� */
									100);		   /* �ȴ�ʱ�� 100 Tick */
					if (xReturn != pdPASS)
					dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);
					/* ���* */
					oled.ctrl = OLED_CTRL_SHOW_STRING;
					oled.x = x_start;
					oled.y = 4;
					oled.str = (uint8_t*)"------";
					oled.font_size = 16;

					xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
										&oled,		   /* ���͵���Ϣ���� */
										100);		   /* �ȴ�ʱ�� 100 Tick */
					if (xReturn != pdPASS)
						dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);
				}
			}
		}
		/* �����޸Ĳ��� */
		if(g_pass_man_what == FLAG_PASS_MAN_MODIFY)
		{
				switch(pass_modify_sta)
			{
				case PASS_MODIFY_STA_OLD_PASS:
				{
					/* û�����룬��Чɾ������ */
					if(key_val == '*' && key_cnt==0)
						continue;

					/* ɾ����һ������ */
					if(key_val == '*' && key_cnt)
					{
						key_cnt--;
						key_buf[key_cnt]=0;

						x-=8;
						oled.ctrl = OLED_CTRL_SHOW_STRING;
						oled.x = x;
						oled.y = 2;
						oled.font_size = 16;

						oled.str =(uint8_t*)"-";
						xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
											&oled,		   /* ���͵���Ϣ���� */
											100);		   /* �ȴ�ʱ�� 100 Tick */
						if (xReturn != pdPASS)
							dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);

						continue;				
					}

					if(key_cnt<6)
					{
						/* ��ʾ���� */
						key_buf[key_cnt] = key_val;

						oled.ctrl = OLED_CTRL_SHOW_STRING;
						oled.x = x;
						oled.y = 2;
						oled.font_size = 16;

						oled.str =(uint8_t *) &key_buf[key_cnt];
						xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
											&oled,		   /* ���͵���Ϣ���� */
											100);		   /* �ȴ�ʱ�� 100 Tick */
						if (xReturn != pdPASS)
							dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);

						vTaskDelay(100);

						oled.str = (uint8_t*)"*";
						xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
											&oled,		   /* ���͵���Ϣ���� */
											100);		   /* �ȴ�ʱ�� 100 Tick */
						if (xReturn != pdPASS)
							dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);

						x+=8;
						key_cnt++;
					}
					else
					{
						if(key_val == '#')
						{
							pass_modify_sta=PASS_MODIFY_STA_NEW_PASS;
							key_cnt=0;
							memcpy(pass_old,key_buf,PASS_LEN);
							memset(key_buf,0,sizeof key_buf);
							x=x_start;
							y=6;
						}
					}	
				}break;
				case PASS_MODIFY_STA_NEW_PASS:
				{

					/* û�����룬��Чɾ������ */
					if(key_val == '*' && key_cnt==0)
						continue;

					/* ɾ����һ������ */
					if(key_val == '*' && key_cnt)
					{
						key_cnt--;
						key_buf[key_cnt]=0;

						x-=8;
						oled.ctrl = OLED_CTRL_SHOW_STRING;
						oled.x = x;
						oled.y = 6;
						oled.font_size = 16;

						oled.str =(uint8_t*)"-";
						xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
											&oled,		   /* ���͵���Ϣ���� */
											100);		   /* �ȴ�ʱ�� 100 Tick */
						if (xReturn != pdPASS)
							dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);

						continue;				
					}


					if(key_cnt<6)
					{
						/* ��ʾ'*' */
						key_buf[key_cnt] = key_val;

						oled.ctrl = OLED_CTRL_SHOW_STRING;
						oled.x = x;
						oled.y = 6;
						oled.font_size = 16;

						oled.str =(uint8_t *) &key_buf[key_cnt];
						xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
											&oled,		   /* ���͵���Ϣ���� */
											100);		   /* �ȴ�ʱ�� 100 Tick */
						if (xReturn != pdPASS)
							dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);

						vTaskDelay(100);

						oled.str = (uint8_t*)"*";
						xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
											&oled,		   /* ���͵���Ϣ���� */
											100);		   /* �ȴ�ʱ�� 100 Tick */
						if (xReturn != pdPASS)
							dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);

						x+=8;
						key_cnt++;
					}
					else
					{
						if(key_val == '#')
						{
							/* ���������� */
							memcpy(pass_new,key_buf,PASS_LEN);

							/* ��ȡeeprom�洢������ */
							at24c02_read(PASS1_ADDR_IN_EEPROM,(uint8_t *)pass_auth_eeprom,PASS_LEN);
							dgb_printf_safe("[app_task_pass_man] password eeprom is %6s\r\n",pass_auth_eeprom);
							/* ƥ��eeprom�洢������ */
							rt = memcmp(pass_auth_eeprom,pass_old,PASS_LEN);
							if(rt)
							{
								dgb_printf_safe("[app_task_pass_man] password old is %6s\r\n",pass_old);
								dgb_printf_safe("[app_task_pass_man] password auth by eeprom fail\r\n");
								
								/* ƥ��Ĭ������ */
								rt=memcmp(pass_auth_default,pass_old,PASS_LEN);
								dgb_printf_safe("[app_task_pass_man] password default is %6s\r\n",pass_auth_default);
								if(rt)
								{
									dgb_printf_safe("[app_task_pass_man] password auth by defalut fail too\r\n");
								}
									
							}
							/* ����ƥ��ɹ� */
							if(rt==0)
							{
								
								dgb_printf_safe("[app_task_pass_man] password auth success\r\n");
								
								/* �����ٽ������ɿ�����eepromд�������� */
								taskENTER_CRITICAL();
								rt = at24c02_write(PASS1_ADDR_IN_EEPROM,(uint8_t *)pass_new,PASS_LEN);
								taskEXIT_CRITICAL();
								respond((uint8_t*)word,sizeof(word),0);
								
									/* ����Ұ벿������ */
								oled.ctrl = OLED_CTRL_CLEAR_REGION;
								oled.x = 64;
								oled.y = 0;
								oled.clr_region_width=64;
								oled.clr_region_height=8;
								xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
													&oled,		   /* ���͵���Ϣ���� */
													100);		   /* �ȴ�ʱ�� 100 Tick */
								if (xReturn != pdPASS)
								dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);
									/* ��վ�������ʾ�� */
								oled.ctrl = OLED_CTRL_SHOW_STRING;
								oled.x = 66;
								oled.y = 2;
								oled.str = (uint8_t*)"change";
								oled.font_size = 16;
								xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
													&oled,		   /* ���͵���Ϣ���� */
													1000);		   /* �ȴ�ʱ�� 100 Tick */
								if (xReturn != pdPASS)
								dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);
								
										/* �����������ʾ�� */
								oled.ctrl = OLED_CTRL_SHOW_STRING;
								oled.x = 66;			
								oled.y = 6;
								oled.str = (uint8_t*)"success";
								oled.font_size = 16;
								xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
													&oled,		   /* ���͵���Ϣ���� */
													1000);		   /* �ȴ�ʱ�� 100 Tick */
								if (xReturn != pdPASS)
									dgb_printf_safe("[menu_pass_man_fun] xQueueSend oled string error code is %d\r\n", xReturn);
								
							}
							else
							{
								dgb_printf_safe("ƥ�������ʧ��\r\n");
								respond((uint8_t*)word_error,sizeof(word_error),1);
							}
							/* ��ʱһ�� */
							vTaskDelay(1000);	

						
							/* �ָ���ֵ */
							pass_modify_sta=PASS_MODIFY_STA_OLD_PASS;
							key_cnt=0;
							memset(key_buf, 0,sizeof key_buf);
							memset(pass_old,0,sizeof pass_old);	
							memset(pass_new,0,sizeof pass_new);	

							x=x_start;
						}
					}	
				}break;
				default:break;
			}
		}
	}
}
void app_task_menu(void *pvParameters)
{
	uint32_t item_total;
	uint32_t item_cursor=0;
	
	uint32_t y=0;

	uint32_t fun_run=0;

	EventBits_t EventValue;

	menu_ext_t *m_ext 	= (menu_ext_t *)pvParameters;
	menu_t *m 			= m_ext->menu;

	m_ext->key_fn		= KEY_NONE;
	m_ext->item_cursor 	= item_cursor;
	m_ext->item_total  	= menu_item_total(m_ext->menu);

	dgb_printf_safe("[app_task_menu] create success\r\n");

	for(;;)
	{
		/* �ȴ��¼����е���Ӧ�¼�λ����ͬ�� */
		EventValue=xEventGroupWaitBits((EventGroupHandle_t	)g_event_group,		
									   (EventBits_t			)EVENT_GROUP_FN_KEY_UP
									   						|EVENT_GROUP_FN_KEY_DOWN
															|EVENT_GROUP_FN_KEY_ENTER
															|EVENT_GROUP_FN_KEY_BACK,
									   (BaseType_t			)pdTRUE,				
									   (BaseType_t			)pdFALSE,
									   (TickType_t			)portMAX_DELAY);

		if(EventValue & EVENT_GROUP_FN_KEY_UP)
		{

			/* ������Ŀ���ܺ��������У���ʾ��Ҫ���ز��ܽ�����Ŀѡ�� */
			if(fun_run)
			{
				continue;
			}
				
			/* ����ϸ���Ŀ��� */
			item_cursor_show(5,y+item_cursor*2,(uint8_t *)" ");
			
			/* ��Ŀ���λ��ֵ�Լ�1 */
			if(item_cursor)
			{
				item_cursor--;

				/* ��Ŀ�˵�ָ����һ���˵� */
				m--;
			}

			/* ��Ŀ���λ����ʾ > */	
			item_cursor_show(5,y+item_cursor*2,(uint8_t *)">");

			/* ���浱ǰ�������� */
			m_ext->key_fn = KEY_UP;

			/* ���浱ǰ��Ŀ���λ��ֵ */
			m_ext->item_cursor = item_cursor;	
		}

		if(EventValue & EVENT_GROUP_FN_KEY_DOWN)
		{

			/* ������Ŀ���ܺ��������У���ʾ��Ҫ���ز��ܽ�����Ŀѡ�� */
			if(fun_run)
			{
				
				continue;
			}			
			
			/* ��ȡ��ǰ�˵��ж��ٸ���Ŀ */
			item_total = menu_item_total(m_ext->menu);
			
			/* ���浱ǰ�˵��ж��ٸ���Ŀ */			
			m_ext->item_total = item_total;

			
			/* ����ϸ���Ŀ��� */
			item_cursor_show(5,y+item_cursor*2,(uint8_t *)" ");
			if(item_cursor < (item_total-1))
			{
				item_cursor++;


				/* ��Ŀ�˵�ָ����һ���˵� */
				m++;
			}

			/* ��Ŀ���λ����ʾ > */
			item_cursor_show(5,y+item_cursor*2,(uint8_t *)">");	

			/* ���浱ǰ�������� */
			m_ext->key_fn = KEY_DOWN;

			/* ���浱ǰ��Ŀ���λ��ֵ */
			m_ext->item_cursor = item_cursor;
			
		}	

		if(EventValue & EVENT_GROUP_FN_KEY_ENTER)
		{
			m_ext->key_fn = KEY_ENTER;
			
			/* ������Ŀ���ܺ��������У���ʾ��Ҫ���ز��ܽ�����Ŀѡ�� */
			if(fun_run)
			{
				
				continue;
			}	
			
			/* �Ӳ˵���Ч */
			if(m->child)
			{
				/* ָ���Ӳ˵� */
				m=m->child;

				/* ���浱ǰ�˵� */
				m_ext->menu = m;

				/* ��ʾ�˵����� */
				menu_show(m_ext);

				/* ��λ���ֵ */
				item_cursor=0;				
			}
			
			/* ��û���Ӳ˵�,��ֱ��ִ�й��ܺ��� */
			if(!m->child && m->fun)
			{
				/* �������Ŀ���ܺ��������� */
				fun_run=1;

				m->fun(m_ext);				
			}		
		}	

		if(EventValue & EVENT_GROUP_FN_KEY_BACK)
		{
			m_ext->key_fn = KEY_BACK;
			/* ���Ӳ˵����ܺ�����Ч����ִ�У���Ҫ�ǹ����Ӧ���� */
			if(m->fun)
			{
				/* �������Ŀ���ܺ��������� */
				fun_run=1;

				m->fun(m_ext);
			}
				
			/* ���˵���Ч */
			if(m->parent)
			{
				/* ָ�򸸲˵� */
				m=m->parent;

				/* ���浱ǰ�˵� */
				m_ext->menu = m;

				/* ��λ���ֵ */
				item_cursor=0;
				m_ext->item_cursor=0;

				fun_run=0;

				/* ��ʾ��ǰ�˵� */
				//menu_show(m_ext);
			}
			menu_show(m_ext);
		}						
	}
}
	
void app_task_oled(void *pvParameters)
{
{
	oled_t oled;
	BaseType_t xReturn=pdFALSE;	
	dgb_printf_safe("[app_task_oled] create success\r\n");	
	
	for(;;)
	{
		xReturn = xQueueReceive( g_queue_oled,	/* ��Ϣ���еľ�� */
								&oled, 			/* �õ�����Ϣ���� */
								portMAX_DELAY);	/* �ȴ�ʱ��һֱ�� */
		if(xReturn != pdPASS)
			continue;
	
		switch(oled.ctrl)
		{
			case OLED_CTRL_DISPLAY_ON:
			{
				/* ���� */
				OLED_Display_On();

			}break;

			case OLED_CTRL_DISPLAY_OFF:
			{
				/* ���� */
				OLED_Display_Off();	
				
			}break;

			case OLED_CTRL_CLEAR:
			{
				/* ���� */
				OLED_Clear();
			}break;

			case OLED_CTRL_SHOW_STRING:
			{
				/* ��ʾ�ַ��� */
				OLED_ShowString(oled.x,
								oled.y,
								oled.str,
								oled.font_size);
			}break;

			case OLED_CTRL_SHOW_CHINESE:
			{
				/* ��ʾ���� */
				OLED_ShowCHinese(oled.x,
								oled.y,
								oled.chinese);
			}break;			
			case OLED_CTRL_SHOW_PICTURE:
			{
				/* ��ʾͼƬ */
				OLED_DrawBMP(oled.x,
							oled.y,
							oled.x + oled.pic_width,
							oled.y + oled.pic_height,
							(unsigned char *)oled.pic);
			}
			break;

			default:dgb_printf_safe("[app_task_oled] oled ctrl code is invalid\r\n");	
				break;
		}				
	}
}
}
void app_task_usart1(void *pvParameters)
{
	BaseType_t xReturn = pdFALSE;
	
	usart_packet_t usart_packet;
	uint32_t hour,minute,second=0;
	uint32_t year,month,date,weekdate=0;
	char 	 pass[9]={0};
	char pass_auth_eeprom[6]={0};
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef  RTC_DateStructure;
	while(1)
	{
		xReturn=xQueueReceive(g_queue_usart1,
                              &usart_packet,
                              portMAX_DELAY);
		
		if(xReturn!=pdPASS)
			continue;
		if(NULL!=strstr((char *)usart_packet.rx_buf,"DATE SET"))
		{
			sscanf((char *)usart_packet.rx_buf,"DATE SET-%d-%d-%d-%d#",&year,&month,&date,&weekdate);
			year = year + year/10*6; //��ʮ����ת��Ϊʮ������
			month = month + month/10*6; 
			date = date + date/10*6; 
			weekdate=weekdate+weekdate/10*6;
			RTC_DateStructure.RTC_Year = year;
			RTC_DateStructure.RTC_Month = month;
			RTC_DateStructure.RTC_Date = date;
			RTC_DateStructure.RTC_WeekDay = weekdate;
			RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
		}
		if(NULL!=strstr((char *)usart_packet.rx_buf,"TIME SET"))
		{
			sscanf((char *)usart_packet.rx_buf,"TIME SET-%d-%d-%d#",&hour,&minute,&second);
			hour = hour + hour/10*6; //��ʮ����ת��Ϊʮ������
			minute = minute + minute/10*6; 
			second = second + second/10*6; 
			RTC_TimeStructure.RTC_H12     = RTC_H12_PM;
			RTC_TimeStructure.RTC_Hours   = hour;
			RTC_TimeStructure.RTC_Minutes = minute;
			RTC_TimeStructure.RTC_Seconds = second; 
			RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
		}	
		if(NULL!=strstr((char *)usart_packet.rx_buf,"UNLOCK"))
		{
			sscanf((char *)usart_packet.rx_buf,"UNLOCK %s#",pass);
			pass[8]='\0';
			if(NULL!=strstr(pass,"888888"))
			{
				respond((uint8_t*)word,sizeof(word),1);	
				mqtt_report_devices_status(2);
				motor_run();
				mp3_run(5);
				continue;
			}
			else
			{
				at24c02_read(PASS1_ADDR_IN_EEPROM,(uint8_t *)pass_auth_eeprom,PASS_LEN);
				if(NULL!=strstr(pass,pass_auth_eeprom))
				{
					respond((uint8_t*)word,sizeof(word),1);
					mqtt_report_devices_status(2);
					motor_run();
					continue;
				}
				dgb_printf_safe("pass error\r\n");
			}
						
		}
		if(NULL!=strstr((char *)usart_packet.rx_buf,"UP"))
		{
			xReturn=xEventGroupSetBits(g_event_group,EVENT_GROUP_FN_KEY_UP);
		}
		if(NULL!=strstr((char *)usart_packet.rx_buf,"DOWN"))
		{
			xReturn=xEventGroupSetBits(g_event_group,EVENT_GROUP_FN_KEY_DOWN);
		}
		if(NULL!=strstr((char *)usart_packet.rx_buf,"ENTER"))
		{
			xReturn=xEventGroupSetBits(g_event_group,EVENT_GROUP_FN_KEY_ENTER);
		}
		if(NULL!=strstr((char *)usart_packet.rx_buf,"BACK"))
		{
			xReturn=xEventGroupSetBits(g_event_group,EVENT_GROUP_FN_KEY_BACK);
		
		}
	}
}

void app_task_esp8266(void *pvParameters)
{
	BaseType_t xReturn = pdFALSE;
	 uint32_t  i;
	uint8_t usart3;

	while(esp8266_mqtt_init())
	{
		dgb_printf_safe("esp8266_mqtt_init ...");
		
		vTaskDelay(1000);
	}
	dgb_printf_safe("esp8266 connect aliyun with mqtt success\r\n");	

	 while(1)
	 { 
		xReturn = xQueueReceive(g_queue_esp8266,	/* ��Ϣ���еľ�� */
									&usart3,		/* �õ�����Ϣ���� */
									portMAX_DELAY); /* �ȴ�ʱ��һֱ�� */
		if (xReturn != pdPASS)
				continue;
		  if(usart3 == 1)
		  {
			   for(i=0;i<g_esp8266_rx_cnt;i++)
			   { 
				   //�жϵĹؼ��ַ��Ƿ�Ϊ 1"
				//�������ݣ���{"switch_led_1":1}�еġ�1��
					if((g_esp8266_rx_buf[i]=='d') && (g_esp8266_rx_buf[i+1]==0x22))
					{
						  //�жϿ��Ʊ���
						  if( g_esp8266_rx_buf[i+3]=='1' )
						  {
							dgb_printf_safe("esp8266 !!1\r\n");
							respond((uint8_t*)word,sizeof(word),1);
							   //MQTT����������
							mqtt_send_heart();
							 mqtt_report_devices_status(0);
							  motor_run();
							  mp3_run(5);
	
						   break;
						  }
					}   
			   }
			   memset((void *)g_esp8266_rx_buf,0,sizeof g_esp8266_rx_buf);
			   g_esp8266_rx_cnt=0;
		 } 
		 vTaskDelay(1000);
		 //MQTT����������
		 mqtt_send_heart();
		 //�ϱ��豸״̬
	 }		
}


void app_task_rtc(void *pvParameters)
{
	uint8_t buf[16] = {0};
	uint8_t buf1[16] = {0};
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
				oled_rtc.x=32;
				oled_rtc.y=4;
				oled_rtc.ctrl=OLED_CTRL_SHOW_STRING;
				oled_rtc.font_size=16;
				oled_rtc.str=buf;
				xReturn=xQueueSend(g_queue_oled,
								   &oled_rtc,
								   portMAX_DELAY);
				if(xReturn!=pdPASS)
					continue;
				
				RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);
				memset(buf1,0,sizeof(buf1));
				sprintf((char *)buf1, "20%02x-%02x-%02x-%d", RTC_DateStructure.RTC_Year,
															RTC_DateStructure.RTC_Month,
															RTC_DateStructure.RTC_Date,
															RTC_DateStructure.RTC_WeekDay);
				oled_rtc.x=14;
				oled_rtc.y=2;
				oled_rtc.ctrl=OLED_CTRL_SHOW_STRING;
				oled_rtc.font_size=16;
				oled_rtc.str=buf1;
				xReturn=xQueueSend(g_queue_oled,
								   &oled_rtc,
								   portMAX_DELAY);
				if(xReturn!=pdPASS)
					continue;
			}
		}
	}
}

void app_task_key(void* pvParameters)
{
	EventBits_t EventValue;
	oled_t oled;
	uint32_t t=0;
	BaseType_t xReturn;
	
	dgb_printf_safe("[app_task_key] create success\r\n");
	
	for(;;)
	{
		/* �ȴ��¼����е���Ӧ�¼�λ����ͬ�� */
		EventValue=xEventGroupWaitBits((EventGroupHandle_t	)g_event_group,		
									   (EventBits_t			)EVENT_GROUP_KEYALL_DOWN,
									   (BaseType_t			)pdTRUE,				
									   (BaseType_t			)pdFALSE,
									   (TickType_t			)portMAX_DELAY);
		
		/* ��ʱ���� */
		vTaskDelay(20);

		/* ����ͳ��ϵͳ�޲�������ֵ */
		taskENTER_CRITICAL();
		g_system_no_opreation_cnt=0;
		taskEXIT_CRITICAL();			

		if(EventValue & EVENT_GROUP_KEY1_DOWN)
		{
			/* ��ֹEXTI0�����ж� */
			NVIC_DisableIRQ(EXTI0_IRQn);
			
			/* ȷ���ǰ��� */
			if(PAin(0) == 0)
			{				
				t=0;

				/* �������� */
				while(PAin(0)==0)
				{
					vTaskDelay(1);
					
					if(t++ >= 2000)											
						break;			
				}
				
				if(t>=2000)
				{
					dgb_printf_safe("[app_task_key] S1 Press Continue\r\n");

					/* ����ϵͳ��λ�¼� */
					xReturn=xEventGroupSetBits(g_event_group,EVENT_GROUP_SYSTEM_RESET);
					if(xReturn != pdPASS)
						dgb_printf_safe("[app_task_key] xEventGroupSetBits EVENT_GROUP_SYSTEM_RESET error code is %d\r\n",xReturn);						
				}
				else
				{
					dgb_printf_safe("[app_task_key] S1 Press\r\n");	

					/* ����KEY_UP�����¼� */
					xReturn=xEventGroupSetBits(g_event_group,EVENT_GROUP_FN_KEY_UP);
					if(xReturn != pdPASS)
						dgb_printf_safe("[app_task_key] xEventGroupSetBits EVENT_GROUP_FN_KEY_UP error code is %d\r\n",xReturn);						
				}				
			}

			/* ����EXTI0�����ж� */
			NVIC_EnableIRQ(EXTI0_IRQn);	
		}
		
		if(EventValue & EVENT_GROUP_KEY2_DOWN)
		{
			/* ��ֹEXTI2�����ж� */
			NVIC_DisableIRQ(EXTI2_IRQn);
				
			if(PEin(2) == 0)
			{
				dgb_printf_safe("[app_task_key] S2 Press\r\n");
				
				/* �ȴ������ͷ� */
				while(PEin(2)==0)
					vTaskDelay(1);
				
				/* ����KEY_DOWN�����¼� */
				xEventGroupSetBits(g_event_group,EVENT_GROUP_FN_KEY_DOWN);	
			}

			/* ����EXTI2�����ж� */
			NVIC_EnableIRQ(EXTI2_IRQn);	
		}	
		
		if(EventValue & EVENT_GROUP_KEY3_DOWN)
		{
			/* ��ֹEXTI3�����ж� */
			NVIC_DisableIRQ(EXTI3_IRQn);
						
			if(PEin(3) == 0)
			{
				dgb_printf_safe("[app_task_key] S3 Press\r\n");
		
				/* �ȴ������ͷ� */
				while(PEin(3)==0)
					vTaskDelay(1);
				
				/* ����KEY_ENTER�����¼� */
				xEventGroupSetBits(g_event_group,EVENT_GROUP_FN_KEY_ENTER);
			}
			
			/* ����EXTI3�����ж� */
			NVIC_EnableIRQ(EXTI3_IRQn);	
		}
		
		if(EventValue & EVENT_GROUP_KEY4_DOWN)
		{
			/* ��ֹEXTI4�����ж� */
			NVIC_DisableIRQ(EXTI4_IRQn);
						
			if(PEin(4) == 0)	
			{

				t=0;
				/* �������������ƶ�OLED�������� */
				while(PEin(4)==0)
				{
					vTaskDelay(1);
					
					if(t++ >= 2000)
						break;		
				}
				
				if(t>=2000)
				{

					dgb_printf_safe("[app_task_key] S4 Press Continue\r\n");

					/* �����ٽ��� */
					taskENTER_CRITICAL();
					
					g_oled_display_flag=!g_oled_display_flag;
					
					/* �˳��ٽ��� */
					taskEXIT_CRITICAL();
					
					/* OLED��/�������� */
					oled.ctrl=g_oled_display_flag?OLED_CTRL_DISPLAY_ON:OLED_CTRL_DISPLAY_OFF;

					xReturn = xQueueSend( 	g_queue_oled,	/* ��Ϣ���еľ�� */
											&oled,			/* ���͵���Ϣ���� */
											100);			/* �ȴ�ʱ�� 100 Tick */
					if(xReturn != pdPASS)
						dgb_printf_safe("[app_task_key] xQueueSend oled error code is %d\r\n",xReturn);						
				}
				else
				{
					dgb_printf_safe("[app_task_key] S4 Press\r\n");
			
					/* ����KEY_BACK�����¼� */
					xEventGroupSetBits(g_event_group,EVENT_GROUP_FN_KEY_BACK);
				}				
			}
			
			/* ����EXTI4�����ж� */
			NVIC_EnableIRQ(EXTI4_IRQn);	
		}			
	}
} 

void app_task_kbd(void* pvParameters)
{
	char key_val='N';
	BaseType_t xReturn;
	dgb_printf_safe("[app_task_kbd] create success\r\n");
	//vTaskSuspend(app_task_kbd_handle);
	while(1)
	{
		/* ��ȡ������̰���ֵ */
		key_val=Get_Key_Borad();
		if(key_val != 'N')
		{
			dgb_printf_safe("[app_task_kbd] kbd press %c \r\n",key_val);

			xReturn = xQueueSend(g_queue_kbd, 	/* ��Ϣ���еľ�� */
								&key_val,		/* ���͵���Ϣ���� */
								100);		   	/* �ȴ�ʱ�� 100 Tick */
			if (xReturn != pdPASS)
				dgb_printf_safe("[app_task_kbd] xQueueSend kbd error code is %d\r\n", xReturn);	
		
		}
		vTaskDelay(100);
	}
}


uint32_t menu_item_total(menu_t *menu)
{
	menu_t *m = menu;

	uint32_t item_count=0;

	while(m->item)
	{
		/* ָ����һ���˵� */
		m++;

		/* ͳ����Ŀ���� */
		item_count++;
	}
		
	return item_count;
}


void item_cursor_show(uint8_t x,uint8_t y,uint8_t *cursor)
{
	oled_t   oled;
	oled.ctrl=OLED_CTRL_SHOW_STRING;
	oled.font_size=16;	
	oled.x=x;
	oled.y=y;
	oled.str=cursor;
	xQueueSend(g_queue_oled,&oled,100);
}

void menu_show(menu_ext_t *menu_ext)
{
	oled_t   oled;
	uint8_t  x,y=0;

	menu_ext_t *m_ext = menu_ext;
	menu_t *m = m_ext->menu;

	/* ���� */
	oled.ctrl=OLED_CTRL_CLEAR;
	xQueueSend(g_queue_oled,&oled,100);	

	/* ӵ���Ӳ˵�����ʾ��ͷ */
	if(m->child)
	{
		m_ext->item_cursor = 0;
		item_cursor_show(5,y,(uint8_t*)">");
	}
		
	/* ��ʾ��ǰ�˵�������Ŀ���� */
	oled.ctrl=OLED_CTRL_SHOW_STRING;
	oled.font_size=16;
	x=20;
	y=0;

	while(y<8)
	{
		/* ���˵���Ŀ������Ч�� */
		if(m->item == NULL || m->item[0]=='-' )
			break;		

		dgb_printf_safe("[menu_show] m->item is %s \r\n",m->item);

		oled.x=x;
		oled.y=y;
		oled.str=m->item;
		xQueueSend(g_queue_oled,&oled,100);	

		/* ָ����һ���˵� */
		m++;

		/* y������һ�� */
		y+=2;
	}
}

void respond(uint8_t buf[],uint8_t size,uint8_t display)
{
	oled_t oled;
	BaseType_t xReturn;
	uint8_t flage=1;
	uint8_t i=0;
	oled.ctrl=OLED_CTRL_CLEAR;
	xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
					&oled,		   /* ���͵���Ϣ���� */
					100);		   /* �ȴ�ʱ�� 100 Tick */
	if (xReturn != pdPASS)
	dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);
	oled.ctrl=OLED_CTRL_SHOW_CHINESE;
	oled.y=3;		
	if(display==1)
	{
		for(i=0;i<size;i++)
		{
			oled.chinese=buf[i];	
			oled.x=27+i*18;
			xReturn = xQueueSend(g_queue_oled, /* ��Ϣ���еľ�� */
						&oled,		   /* ���͵���Ϣ���� */
						100);		   /* �ȴ�ʱ�� 100 Tick */
			if (xReturn != pdPASS)
			dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);
		}
	}

		
	flage=1;
	xReturn = xQueueSend(g_queue_led, /* ��Ϣ���еľ�� */
					&flage,		   /* ���͵���Ϣ���� */
					100);		   /* �ȴ�ʱ�� 100 Tick */
	if (xReturn != pdPASS)
	dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);
	xReturn = xQueueSend(g_queue_beep, /* ��Ϣ���еľ�� */
					&flage,		   /* ���͵���Ϣ���� */
					100);		   /* �ȴ�ʱ�� 100 Tick */
	if (xReturn != pdPASS)
	dgb_printf_safe("[app_task_pass_man] xQueueSend oled string error code is %d\r\n", xReturn);
	
}

void motor_run()
{
	uint8_t i=1;
	xQueueSend(g_queue_motor, /* ��Ϣ���еľ�� */
					&i,		   /* ���͵���Ϣ���� */
					100);		   /* �ȴ�ʱ�� 100 Tick */
}

void mp3_run(uint8_t i)
{
	uint8_t j=i;
	xQueueSend(g_queue_mp3, /* ��Ϣ���еľ�� */
					&j,		   /* ���͵���Ϣ���� */
					100);		   /* �ȴ�ʱ�� 100 Tick */
}
void app_task_mp3(void *pvParameters)
{
		uint8_t j = 0;
	BaseType_t xReturn = pdFALSE;

	dgb_printf_safe("[app_task_led] create success\r\n");

	for (;;)
	{
		xReturn = xQueueReceive(g_queue_mp3,	/* ��Ϣ���еľ�� */
								&j,		/* �õ�����Ϣ���� */
								portMAX_DELAY); /* �ȴ�ʱ��һֱ�� */
		if (xReturn != pdPASS)
			continue;
		if(j==1)
		{
			PCout(7) = 0;
			vTaskDelay(100);
			PCout(7) = 1;
		}
		if(j==2)
		{
			PCout(9) = 0;
			vTaskDelay(100);
			PCout(9) = 1;
		}
		if(j==3)
		{
			PBout(6) = 0;
			vTaskDelay(100);
			PBout(6) = 1;
		}
		if(j==4)
		{
			PEout(6) = 0;
			vTaskDelay(100);
			PEout(6) = 1;
		}
		if(j==5)
		{
			PAout(8) = 0;
			vTaskDelay(100);
			PAout(8) = 1;
		}
		
		
		vTaskDelay(500);
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
